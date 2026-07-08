#include "MandelRenderHost.h"

#include "assets.h"

#include <core/MandelCpu.h>
#include <core/MandelImage.h>

#include <sdlcpp/GpuBuffer.h>
#include <sdlcpp/GpuCommandBuffer.h>
#include <sdlcpp/GpuComputePass.h>
#include <sdlcpp/GpuComputePipeline.h>
#include <sdlcpp/GpuCopyPass.h>
#include <sdlcpp/GpuGraphicsPipeline.h>
#include <sdlcpp/GpuRenderPass.h>
#include <sdlcpp/GpuShader.h>
#include <sdlcpp/GpuSwapchainTexture.h>
#include <sdlcpp/GpuTexture.h>
#include <sdlcpp/GpuTransferBuffer.h>
#include <sdlcpp/sdl.h>

#include <wx/dcclient.h>
#include <wx/event.h>
#include <wx/image.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace mandel
{

namespace
{

using Matrix4x4 = std::array<float, 16>;

constexpr double zoom_scale_per_wheel_click = 0.8;
constexpr float height_orbit_radians_per_pixel = 0.01f;
constexpr float initial_height_yaw_radians = 0.72f;
constexpr float initial_height_pitch_radians = 0.58f;
constexpr float initial_height_distance = 3.1f;
constexpr float min_height_pitch_radians = -1.2f;
constexpr float max_height_pitch_radians = 1.2f;
constexpr float min_height_distance = 1.4f;
constexpr float max_height_distance = 8.0f;
constexpr float height_camera_target_y = 0.16f;
constexpr float height_field_of_view_radians = 0.785398163f;

constexpr SDL_GPUShaderFormat gpu_shader_formats =
    SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL;

constexpr Uint32 mandel_workgroup_size = 16;
constexpr Uint32 height_grid_max_width = 192;
constexpr Uint32 height_grid_max_height = 144;
constexpr float height_scale = 0.55f;
constexpr float potential_height_bailout = 65536.0f;
constexpr float potential_height_scale = 64.0f;
constexpr float potential_height_log_scale = 1.0f / 6.0f;
constexpr SDL_GPUTextureFormat height_depth_format = SDL_GPU_TEXTUREFORMAT_D16_UNORM;
constexpr SDL_FColor gpu_clear_color{0.0f, 0.0f, 0.0f, 1.0f};

struct ShaderAssetFormat
{
    SDL_GPUShaderFormat format;
    const char *extension;
    const char *entrypoint;
};

struct alignas(16) GpuMandelParams
{
    float view[4];
    float constants[4];
    float potential[4];
    int image[4];
    int periodicity[4];
};

struct alignas(16) GpuBlitParams
{
    float max_iterations;
    Uint32 palette_size;
    Uint32 padding[2];
};

struct alignas(16) GpuHeightParams
{
    float world_to_clip[16];
    Uint32 texture_size[2];
    Uint32 grid_size[2];
    float height_value_max;
    float height_scale;
    float padding[2];
};

struct HeightGridSize
{
    Uint32 width;
    Uint32 height;
};

struct Vec3
{
    float x;
    float y;
    float z;
};

constexpr std::array shader_asset_formats{
    ShaderAssetFormat{SDL_GPU_SHADERFORMAT_DXIL, ".dxil", "main"},
    ShaderAssetFormat{SDL_GPU_SHADERFORMAT_MSL, ".msl", "main0"},
    ShaderAssetFormat{SDL_GPU_SHADERFORMAT_SPIRV, ".spv", "main"},
};

wxWindow *event_window(wxEvent &event)
{
    return dynamic_cast<wxWindow *>(event.GetEventObject());
}

core::MandelPalette load_chroma_palette()
{
    const std::string chroma_map = std::string{assets_dir} + "/chroma.map";
    std::ifstream input(chroma_map);
    if (!input)
    {
        throw std::runtime_error("Unable to open chroma.map");
    }

    core::MandelPalette palette = core::read_mandel_palette(input);
    if (palette.colors.empty())
    {
        throw std::runtime_error("chroma.map must contain RGB entries");
    }

    return palette;
}

const ShaderAssetFormat &shader_asset_format(const sdlcpp::GpuDevice &device)
{
    const SDL_GPUShaderFormat supported_formats = device.GetGPUShaderFormats();
    for (const ShaderAssetFormat &asset_format : shader_asset_formats)
    {
        if ((supported_formats & asset_format.format) != 0)
        {
            return asset_format;
        }
    }

    throw std::runtime_error("No matching static shader asset format for SDL GPU backend");
}

Uint32 ceil_div(Uint32 value, Uint32 divisor)
{
    return (value + divisor - 1) / divisor;
}

std::vector<Uint8> read_shader_code(const std::string &path)
{
    std::ifstream input(path, std::ios::binary);
    if (!input)
    {
        throw std::runtime_error("Unable to open shader asset " + path);
    }

    const std::string code{std::istreambuf_iterator<char>{input}, std::istreambuf_iterator<char>{}};
    return std::vector<Uint8>{code.begin(), code.end()};
}

sdlcpp::GpuShader load_graphics_shader(const sdlcpp::GpuDevice &device, const char *asset_name,
    SDL_GPUShaderStage stage, Uint32 num_storage_textures = 0, Uint32 num_storage_buffers = 0,
    Uint32 num_uniform_buffers = 0)
{
    const ShaderAssetFormat &asset_format = shader_asset_format(device);
    const auto path = std::string{assets_dir} + "/" + asset_name + asset_format.extension;
    const std::vector<Uint8> code = read_shader_code(path);

    SDL_GPUShaderCreateInfo create_info{};
    create_info.code_size = code.size();
    create_info.code = code.data();
    create_info.entrypoint = asset_format.entrypoint;
    create_info.format = asset_format.format;
    create_info.stage = stage;
    create_info.num_storage_textures = num_storage_textures;
    create_info.num_storage_buffers = num_storage_buffers;
    create_info.num_uniform_buffers = num_uniform_buffers;

    return device.CreateGPUShader(create_info);
}

sdlcpp::GpuComputePipeline load_mandel_compute_pipeline(const sdlcpp::GpuDevice &device)
{
    const ShaderAssetFormat &asset_format = shader_asset_format(device);
    const auto path = std::string{assets_dir} + "/mandel.comp" + asset_format.extension;
    const std::vector<Uint8> code = read_shader_code(path);

    SDL_GPUComputePipelineCreateInfo create_info{};
    create_info.code_size = code.size();
    create_info.code = code.data();
    create_info.entrypoint = asset_format.entrypoint;
    create_info.format = asset_format.format;
    create_info.num_readwrite_storage_textures = 2;
    create_info.num_uniform_buffers = 1;
    create_info.threadcount_x = mandel_workgroup_size;
    create_info.threadcount_y = mandel_workgroup_size;
    create_info.threadcount_z = 1;

    return device.CreateGPUComputePipeline(create_info);
}

GpuMandelParams make_gpu_mandel_params(const core::MandelParams &params)
{
    GpuMandelParams gpu_params{};
    gpu_params.view[0] = static_cast<float>(params.x_min);
    gpu_params.view[1] = static_cast<float>(params.y_min);
    gpu_params.view[2] = static_cast<float>(params.dx);
    gpu_params.view[3] = static_cast<float>(params.dy);
    gpu_params.constants[0] = static_cast<float>(params.z0_real);
    gpu_params.constants[1] = static_cast<float>(params.z0_imag);
    gpu_params.constants[2] = static_cast<float>(params.bailout);
    gpu_params.constants[3] = static_cast<float>(params.close_enough);
    gpu_params.potential[0] = potential_height_bailout;
    gpu_params.potential[1] = potential_height_scale;
    gpu_params.potential[2] = potential_height_log_scale;
    gpu_params.image[0] = params.width;
    gpu_params.image[1] = params.height;
    gpu_params.image[2] = params.max_iterations;
    gpu_params.image[3] = params.first_saved_and;
    gpu_params.periodicity[0] = params.periodicity_next_saved_incr;
    gpu_params.periodicity[1] = params.periodicity_check;
    return gpu_params;
}

GpuBlitParams make_gpu_blit_params(const core::MandelParams &params, const core::MandelPalette &palette)
{
    GpuBlitParams gpu_params{};
    gpu_params.max_iterations = static_cast<float>(params.max_iterations);
    gpu_params.palette_size = static_cast<Uint32>(palette.colors.size());
    return gpu_params;
}

HeightGridSize make_height_grid_size(const wxSize &size)
{
    const int grid_width = std::clamp(size.GetWidth() / 4, 2, static_cast<int>(height_grid_max_width));
    const int grid_height = std::clamp(size.GetHeight() / 4, 2, static_cast<int>(height_grid_max_height));
    return HeightGridSize{static_cast<Uint32>(grid_width), static_cast<Uint32>(grid_height)};
}

Uint32 height_field_vertex_count(HeightGridSize grid_size)
{
    return (grid_size.width - 1) * (grid_size.height - 1) * 6;
}

float dot(Vec3 left, Vec3 right)
{
    return left.x * right.x + left.y * right.y + left.z * right.z;
}

Vec3 cross(Vec3 left, Vec3 right)
{
    return Vec3{
        left.y * right.z - left.z * right.y, left.z * right.x - left.x * right.z, left.x * right.y - left.y * right.x};
}

Vec3 normalize(Vec3 value)
{
    const float length = std::sqrt(dot(value, value));
    if (length <= 0.0f)
    {
        return Vec3{0.0f, 0.0f, 0.0f};
    }

    return Vec3{value.x / length, value.y / length, value.z / length};
}

Matrix4x4 multiply_matrix(const Matrix4x4 &left, const Matrix4x4 &right)
{
    Matrix4x4 result{};
    for (int row = 0; row < 4; ++row)
    {
        for (int column = 0; column < 4; ++column)
        {
            for (int index = 0; index < 4; ++index)
            {
                result[row * 4 + column] += left[row * 4 + index] * right[index * 4 + column];
            }
        }
    }

    return result;
}

Matrix4x4 make_look_at_matrix(Vec3 eye, Vec3 target)
{
    const Vec3 forward = normalize(Vec3{target.x - eye.x, target.y - eye.y, target.z - eye.z});
    const Vec3 backward = Vec3{-forward.x, -forward.y, -forward.z};
    const Vec3 right = normalize(cross(Vec3{0.0f, 1.0f, 0.0f}, backward));
    const Vec3 up = cross(backward, right);

    return Matrix4x4{right.x, right.y, right.z, -dot(right, eye), up.x, up.y, up.z, -dot(up, eye),
        backward.x, backward.y, backward.z, -dot(backward, eye), 0.0f, 0.0f, 0.0f, 1.0f};
}

Matrix4x4 make_perspective_matrix(float aspect_ratio)
{
    constexpr float near_z = 0.05f;
    constexpr float far_z = 20.0f;

    const float y_scale = 1.0f / std::tan(height_field_of_view_radians * 0.5f);
    const float x_scale = y_scale / std::max(aspect_ratio, 0.001f);
    const float depth_scale = far_z / (near_z - far_z);
    const float depth_offset = near_z * far_z / (near_z - far_z);

    return Matrix4x4{x_scale, 0.0f, 0.0f, 0.0f, 0.0f, y_scale, 0.0f, 0.0f, 0.0f, 0.0f, depth_scale,
        depth_offset, 0.0f, 0.0f, -1.0f, 0.0f};
}

Matrix4x4 make_height_world_to_clip(float yaw_radians, float pitch_radians, float distance, wxSize size)
{
    const float aspect_ratio =
        static_cast<float>(std::max(size.GetWidth(), 1)) / static_cast<float>(std::max(size.GetHeight(), 1));
    const Vec3 target{0.0f, height_camera_target_y, 0.0f};
    const float pitch_cosine = std::cos(pitch_radians);
    const Vec3 eye{target.x + distance * pitch_cosine * std::sin(yaw_radians),
        target.y + distance * std::sin(pitch_radians), target.z + distance * pitch_cosine * std::cos(yaw_radians)};

    return multiply_matrix(make_perspective_matrix(aspect_ratio), make_look_at_matrix(eye, target));
}

GpuHeightParams make_gpu_height_params(
    const wxSize &size, float yaw_radians, float pitch_radians, float distance, float height_value_max)
{
    const HeightGridSize grid_size = make_height_grid_size(size);
    const Matrix4x4 world_to_clip = make_height_world_to_clip(yaw_radians, pitch_radians, distance, size);

    GpuHeightParams gpu_params{};
    std::copy(world_to_clip.begin(), world_to_clip.end(), gpu_params.world_to_clip);
    gpu_params.texture_size[0] = static_cast<Uint32>(std::max(size.GetWidth(), 1));
    gpu_params.texture_size[1] = static_cast<Uint32>(std::max(size.GetHeight(), 1));
    gpu_params.grid_size[0] = grid_size.width;
    gpu_params.grid_size[1] = grid_size.height;
    gpu_params.height_value_max = height_value_max;
    gpu_params.height_scale = height_scale;
    return gpu_params;
}

Uint32 pack_palette_color(const core::Rgba8 &color)
{
    return static_cast<Uint32>(color.red) | (static_cast<Uint32>(color.green) << 8) |
        (static_cast<Uint32>(color.blue) << 16) | (static_cast<Uint32>(color.alpha) << 24);
}

sdlcpp::GpuBuffer make_palette_buffer(const sdlcpp::GpuDevice &device, const core::MandelPalette &palette)
{
    const auto buffer_size = static_cast<Uint32>(palette.colors.size() * sizeof(Uint32));

    SDL_GPUBufferCreateInfo buffer_create_info{};
    buffer_create_info.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ;
    buffer_create_info.size = buffer_size;
    sdlcpp::GpuBuffer palette_buffer = device.CreateGPUBuffer(buffer_create_info);

    SDL_GPUTransferBufferCreateInfo transfer_create_info{};
    transfer_create_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transfer_create_info.size = buffer_size;
    sdlcpp::GpuTransferBuffer transfer_buffer = device.CreateGPUTransferBuffer(transfer_create_info);

    auto *mapped = static_cast<Uint32 *>(device.MapGPUTransferBuffer(transfer_buffer, false));

    for (std::size_t index = 0; index < palette.colors.size(); ++index)
    {
        mapped[index] = pack_palette_color(palette.colors[index]);
    }
    device.UnmapGPUTransferBuffer(transfer_buffer);

    sdlcpp::GpuCommandBuffer command_buffer = device.AcquireGPUCommandBuffer();
    sdlcpp::GpuCopyPass copy_pass = command_buffer.BeginGPUCopyPass();
    copy_pass.UploadToGPUBuffer(transfer_buffer, 0, palette_buffer, 0, buffer_size, false);
    copy_pass.EndGPUCopyPass();
    command_buffer.SubmitGPUCommandBuffer();

    return palette_buffer;
}

sdlcpp::GpuTexture make_iteration_texture(const sdlcpp::GpuDevice &device, const wxSize &size)
{
    SDL_GPUTextureCreateInfo create_info{};
    create_info.type = SDL_GPU_TEXTURETYPE_2D;
    create_info.format = SDL_GPU_TEXTUREFORMAT_R32_FLOAT;
    create_info.usage = SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_WRITE | SDL_GPU_TEXTUREUSAGE_GRAPHICS_STORAGE_READ;
    create_info.width = static_cast<Uint32>(size.GetWidth());
    create_info.height = static_cast<Uint32>(size.GetHeight());
    create_info.layer_count_or_depth = 1;
    create_info.num_levels = 1;
    create_info.sample_count = SDL_GPU_SAMPLECOUNT_1;

    return device.CreateGPUTexture(create_info);
}

sdlcpp::GpuTexture make_height_depth_texture(const sdlcpp::GpuDevice &device, const wxSize &size)
{
    SDL_GPUTextureCreateInfo create_info{};
    create_info.type = SDL_GPU_TEXTURETYPE_2D;
    create_info.format = height_depth_format;
    create_info.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
    create_info.width = static_cast<Uint32>(size.GetWidth());
    create_info.height = static_cast<Uint32>(size.GetHeight());
    create_info.layer_count_or_depth = 1;
    create_info.num_levels = 1;
    create_info.sample_count = SDL_GPU_SAMPLECOUNT_1;

    return device.CreateGPUTexture(create_info);
}

sdlcpp::GpuGraphicsPipeline make_blit_pipeline(
    const sdlcpp::GpuDevice &device, SDL_Window *window, SDL_GPUShader *vertex_shader, SDL_GPUShader *fragment_shader)
{
    SDL_GPUColorTargetDescription color_target_description{};
    color_target_description.format = device.GetGPUSwapchainTextureFormat(window);

    SDL_GPUGraphicsPipelineCreateInfo create_info{};
    create_info.vertex_shader = vertex_shader;
    create_info.fragment_shader = fragment_shader;
    create_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    create_info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    create_info.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
    create_info.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    create_info.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
    create_info.target_info.color_target_descriptions = &color_target_description;
    create_info.target_info.num_color_targets = 1;

    return device.CreateGPUGraphicsPipeline(create_info);
}

sdlcpp::GpuGraphicsPipeline make_height_pipeline(
    const sdlcpp::GpuDevice &device, SDL_Window *window, SDL_GPUShader *vertex_shader, SDL_GPUShader *fragment_shader)
{
    SDL_GPUColorTargetDescription color_target_description{};
    color_target_description.format = device.GetGPUSwapchainTextureFormat(window);

    SDL_GPUGraphicsPipelineCreateInfo create_info{};
    create_info.vertex_shader = vertex_shader;
    create_info.fragment_shader = fragment_shader;
    create_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    create_info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    create_info.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
    create_info.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    create_info.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
    create_info.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_LESS_OR_EQUAL;
    create_info.depth_stencil_state.enable_depth_test = true;
    create_info.depth_stencil_state.enable_depth_write = true;
    create_info.target_info.color_target_descriptions = &color_target_description;
    create_info.target_info.num_color_targets = 1;
    create_info.target_info.depth_stencil_format = height_depth_format;
    create_info.target_info.has_depth_stencil_target = true;

    return device.CreateGPUGraphicsPipeline(create_info);
}

std::size_t pixel_index(int width, int px, int py)
{
    return static_cast<std::size_t>(py) * static_cast<std::size_t>(width) + static_cast<std::size_t>(px);
}

wxBitmap make_bitmap(const core::MandelImage &image)
{
    wxImage wx_image(image.width, image.height);
    unsigned char *data = wx_image.GetData();

    for (int py = 0; py < image.height; ++py)
    {
        for (int px = 0; px < image.width; ++px)
        {
            const core::Rgba8 color = image.pixels[pixel_index(image.width, px, py)];
            const std::size_t offset = 3 * pixel_index(image.width, px, py);
            data[offset + 0] = color.red;
            data[offset + 1] = color.green;
            data[offset + 2] = color.blue;
        }
    }

    return wxBitmap(wx_image);
}

} // namespace

MandelRenderHost::MandelRenderHost(wxWindow &frame, wxWindow &cpu_display, wxsdl::SdlCanvas &canvas) :
    m_frame(frame),
    m_cpuDisplay(cpu_display),
    m_canvas(canvas),
    m_palette(load_chroma_palette()),
    m_gpuDevice(sdlcpp::make_gpu_device(gpu_shader_formats)),
    m_gpuWindow(m_gpuDevice.ClaimWindowForGPUDevice(m_canvas.window().get())),
    m_mandelComputePipeline(load_mandel_compute_pipeline(m_gpuDevice)),
    m_blitVertexShader(load_graphics_shader(m_gpuDevice, "blit.vert", SDL_GPU_SHADERSTAGE_VERTEX)),
    m_blitFragmentShader(load_graphics_shader(m_gpuDevice, "blit.frag", SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 1, 1)),
    m_blitPipeline(
        make_blit_pipeline(m_gpuDevice, m_canvas.window().get(), m_blitVertexShader.get(), m_blitFragmentShader.get())),
    m_heightVertexShader(load_graphics_shader(m_gpuDevice, "height.vert", SDL_GPU_SHADERSTAGE_VERTEX, 1, 0, 1)),
    m_heightFragmentShader(load_graphics_shader(m_gpuDevice, "height.frag", SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 1, 1)),
    m_heightPipeline(make_height_pipeline(
        m_gpuDevice, m_canvas.window().get(), m_heightVertexShader.get(), m_heightFragmentShader.get())),
    m_paletteBuffer(make_palette_buffer(m_gpuDevice, m_palette)),
    m_heightYawRadians(initial_height_yaw_radians),
    m_heightPitchRadians(initial_height_pitch_radians),
    m_heightDistance(initial_height_distance),
    m_timer(&frame)
{
    m_cpuDisplay.SetBackgroundStyle(wxBG_STYLE_PAINT);
    m_frame.Bind(wxEVT_SIZE, &MandelRenderHost::OnFrameSize, this);
    m_cpuDisplay.Bind(wxEVT_PAINT, &MandelRenderHost::OnCpuPaint, this);
    m_canvas.Bind(wxEVT_PAINT, &MandelRenderHost::OnCanvasPaint, this);
    BindMouseEvents(m_cpuDisplay);
    BindMouseEvents(m_canvas);
    m_frame.Bind(wxEVT_TIMER, &MandelRenderHost::OnTimer, this, m_timer.GetId());
    LayoutDisplays();
    SelectCpuPresentation();
    SubmitEmptyGpuCommandBuffer();
    m_timer.Start(16);
}

MandelRenderHost::~MandelRenderHost()
{
    ReleaseMouse();
    m_timer.Stop();
    m_frame.Unbind(wxEVT_TIMER, &MandelRenderHost::OnTimer, this, m_timer.GetId());
    UnbindMouseEvents(m_canvas);
    UnbindMouseEvents(m_cpuDisplay);
    m_canvas.Unbind(wxEVT_PAINT, &MandelRenderHost::OnCanvasPaint, this);
    m_cpuDisplay.Unbind(wxEVT_PAINT, &MandelRenderHost::OnCpuPaint, this);
    m_frame.Unbind(wxEVT_SIZE, &MandelRenderHost::OnFrameSize, this);
}

void MandelRenderHost::BindMouseEvents(wxWindow &window)
{
    window.Bind(wxEVT_LEFT_DOWN, &MandelRenderHost::OnMouseLeftDown, this);
    window.Bind(wxEVT_LEFT_UP, &MandelRenderHost::OnMouseLeftUp, this);
    window.Bind(wxEVT_MOTION, &MandelRenderHost::OnMouseMove, this);
    window.Bind(wxEVT_MOUSEWHEEL, &MandelRenderHost::OnMouseWheel, this);
    window.Bind(wxEVT_MOUSE_CAPTURE_LOST, &MandelRenderHost::OnMouseCaptureLost, this);
}

wxSize MandelRenderHost::DisplaySize() const
{
    return m_frame.GetClientSize();
}

void MandelRenderHost::InvalidateRender()
{
    m_cpuDirty = true;
    m_gpuDirty = true;
    RefreshActiveDisplay();
}

void MandelRenderHost::EnsureGpuTexture(const wxSize &size)
{
    if (m_iterationTexture && m_gpuTextureSize == size)
    {
        return;
    }

    m_iterationTexture = make_iteration_texture(m_gpuDevice, size);
    m_potentialTexture = make_iteration_texture(m_gpuDevice, size);
    m_gpuTextureSize = size;
    m_gpuDirty = true;
}

void MandelRenderHost::EnsureHeightDepthTexture(const wxSize &size)
{
    if (m_heightDepthTexture && m_heightDepthTextureSize == size)
    {
        return;
    }

    m_heightDepthTexture = make_height_depth_texture(m_gpuDevice, size);
    m_heightDepthTextureSize = size;
}

void MandelRenderHost::RefreshActiveDisplay()
{
    if (m_presentation == Presentation::Cpu)
    {
        m_cpuDisplay.Refresh(false);
    }
    else
    {
        m_canvas.Refresh(false);
    }
}

void MandelRenderHost::LayoutDisplays()
{
    const wxSize size = DisplaySize();
    m_cpuDisplay.SetSize(0, 0, size.GetWidth(), size.GetHeight());
    m_canvas.SetSize(0, 0, size.GetWidth(), size.GetHeight());
    ApplyPresentationVisibility();
    InvalidateRender();
}

void MandelRenderHost::ApplyPresentationVisibility()
{
    if (m_presentation == Presentation::Cpu)
    {
        m_canvas.Hide();
        m_cpuDisplay.Show();
    }
    else
    {
        m_cpuDisplay.Hide();
        m_canvas.Show();
    }
}

void MandelRenderHost::ReleaseMouse()
{
    if (m_mouseCapture != nullptr && m_mouseCapture->HasCapture())
    {
        m_mouseCapture->ReleaseMouse();
    }
    m_mouseCapture = nullptr;
}

void MandelRenderHost::RenderCpuImage()
{
    if (!m_cpuDirty)
    {
        return;
    }

    const wxSize size = m_cpuDisplay.GetClientSize();
    if (size.GetWidth() <= 0 || size.GetHeight() <= 0)
    {
        m_cpuBitmap = wxBitmap();
        m_cpuDirty = false;
        return;
    }

    const core::MandelParams params = m_viewport.params(size.GetWidth(), size.GetHeight());
    const core::MandelIterationBuffer iterations = core::render_mandel_cpu(params);
    const core::MandelImage image = core::map_mandel_colors(iterations, params.max_iterations, m_palette);
    m_cpuBitmap = make_bitmap(image);
    m_cpuDirty = false;
}

void MandelRenderHost::RenderGpuImage(sdlcpp::GpuCommandBuffer &command_buffer, const wxSize &size)
{
    if (!m_gpuDirty)
    {
        return;
    }

    const core::MandelParams params = m_viewport.params(size.GetWidth(), size.GetHeight());
    const GpuMandelParams gpu_params = make_gpu_mandel_params(params);
    command_buffer.PushGPUComputeUniformData(0, &gpu_params, sizeof(gpu_params));

    const std::array output_textures{
        SDL_GPUStorageTextureReadWriteBinding{m_iterationTexture.get(), 0, 0, true, 0, 0, 0},
        SDL_GPUStorageTextureReadWriteBinding{m_potentialTexture.get(), 0, 0, true, 0, 0, 0}};
    sdlcpp::GpuComputePass compute_pass =
        command_buffer.BeginGPUComputePass(output_textures.data(), static_cast<Uint32>(output_textures.size()));
    compute_pass.BindGPUComputePipeline(m_mandelComputePipeline);
    compute_pass.DispatchGPUCompute(ceil_div(static_cast<Uint32>(size.GetWidth()), mandel_workgroup_size),
        ceil_div(static_cast<Uint32>(size.GetHeight()), mandel_workgroup_size), 1);
    m_gpuDirty = false;
}

void MandelRenderHost::RenderHeightField(sdlcpp::GpuCommandBuffer &command_buffer, sdlcpp::GpuRenderPass &render_pass,
    const wxSize &size, sdlcpp::GpuTexture &height_texture, float height_value_max)
{
    const core::MandelParams params = m_viewport.params(size.GetWidth(), size.GetHeight());
    const GpuHeightParams height_params =
        make_gpu_height_params(size, m_heightYawRadians, m_heightPitchRadians, m_heightDistance, height_value_max);
    const GpuBlitParams blit_params = make_gpu_blit_params(params, m_palette);
    const HeightGridSize grid_size = make_height_grid_size(size);

    command_buffer.PushGPUVertexUniformData(0, &height_params, sizeof(height_params));
    command_buffer.PushGPUFragmentUniformData(0, &blit_params, sizeof(blit_params));
    render_pass.BindGPUGraphicsPipeline(m_heightPipeline);
    render_pass.BindGPUVertexStorageTextures(0, height_texture);
    render_pass.BindGPUFragmentStorageTextures(0, m_iterationTexture);
    render_pass.BindGPUFragmentStorageBuffers(0, m_paletteBuffer);
    render_pass.DrawGPUPrimitives(height_field_vertex_count(grid_size), 1, 0, 0);
}

void MandelRenderHost::RenderHeightFieldFrame(bool use_potential_height)
{
    const wxSize size = m_canvas.GetClientSize();
    if (size.GetWidth() <= 0 || size.GetHeight() <= 0)
    {
        return;
    }

    EnsureGpuTexture(size);
    EnsureHeightDepthTexture(size);

    sdlcpp::GpuCommandBuffer command_buffer = m_gpuDevice.AcquireGPUCommandBuffer();
    RenderGpuImage(command_buffer, size);
    const sdlcpp::GpuSwapchainTexture swapchain_texture =
        command_buffer.WaitAndAcquireGPUSwapchainTexture(m_canvas.window().get());

    if (swapchain_texture)
    {
        const SDL_GPUColorTargetInfo target_info{swapchain_texture.texture, 0, 0, gpu_clear_color, SDL_GPU_LOADOP_CLEAR,
            SDL_GPU_STOREOP_STORE, nullptr, 0, 0, false, false, 0, 0};
        const SDL_GPUDepthStencilTargetInfo depth_info{m_heightDepthTexture.get(), 1.0f, SDL_GPU_LOADOP_CLEAR,
            SDL_GPU_STOREOP_DONT_CARE, SDL_GPU_LOADOP_DONT_CARE, SDL_GPU_STOREOP_DONT_CARE, true, 0, 0, 0};
        sdlcpp::GpuRenderPass render_pass = command_buffer.BeginGPURenderPass(&target_info, 1, &depth_info);
        if (use_potential_height)
        {
            RenderHeightField(command_buffer, render_pass, size, m_potentialTexture, 1.0f);
        }
        else
        {
            const core::MandelParams params = m_viewport.params(size.GetWidth(), size.GetHeight());
            RenderHeightField(
                command_buffer, render_pass, size, m_iterationTexture, static_cast<float>(params.max_iterations));
        }
    }

    command_buffer.SubmitGPUCommandBuffer();
}

void MandelRenderHost::RenderGpuFrame()
{
    const wxSize size = m_canvas.GetClientSize();
    if (size.GetWidth() <= 0 || size.GetHeight() <= 0)
    {
        return;
    }

    EnsureGpuTexture(size);

    sdlcpp::GpuCommandBuffer command_buffer = m_gpuDevice.AcquireGPUCommandBuffer();
    RenderGpuImage(command_buffer, size);
    const sdlcpp::GpuSwapchainTexture swapchain_texture =
        command_buffer.WaitAndAcquireGPUSwapchainTexture(m_canvas.window().get());

    if (swapchain_texture)
    {
        const SDL_GPUColorTargetInfo target_info{swapchain_texture.texture, 0, 0, gpu_clear_color, SDL_GPU_LOADOP_CLEAR,
            SDL_GPU_STOREOP_STORE, nullptr, 0, 0, false, false, 0, 0};
        sdlcpp::GpuRenderPass render_pass = command_buffer.BeginGPURenderPass(&target_info, 1);
        const core::MandelParams params = m_viewport.params(size.GetWidth(), size.GetHeight());
        const GpuBlitParams gpu_params = make_gpu_blit_params(params, m_palette);
        command_buffer.PushGPUFragmentUniformData(0, &gpu_params, sizeof(gpu_params));
        render_pass.BindGPUGraphicsPipeline(m_blitPipeline);
        render_pass.BindGPUFragmentStorageTextures(0, m_iterationTexture);
        render_pass.BindGPUFragmentStorageBuffers(0, m_paletteBuffer);
        render_pass.DrawGPUPrimitives(3, 1, 0, 0);
    }

    command_buffer.SubmitGPUCommandBuffer();
}

void MandelRenderHost::SubmitEmptyGpuCommandBuffer()
{
    sdlcpp::GpuCommandBuffer command_buffer = m_gpuDevice.AcquireGPUCommandBuffer();
    command_buffer.SubmitGPUCommandBuffer();
}

void MandelRenderHost::UnbindMouseEvents(wxWindow &window)
{
    window.Unbind(wxEVT_MOUSE_CAPTURE_LOST, &MandelRenderHost::OnMouseCaptureLost, this);
    window.Unbind(wxEVT_MOUSEWHEEL, &MandelRenderHost::OnMouseWheel, this);
    window.Unbind(wxEVT_MOTION, &MandelRenderHost::OnMouseMove, this);
    window.Unbind(wxEVT_LEFT_UP, &MandelRenderHost::OnMouseLeftUp, this);
    window.Unbind(wxEVT_LEFT_DOWN, &MandelRenderHost::OnMouseLeftDown, this);
}

void MandelRenderHost::SelectCpuPresentation()
{
    ReleaseMouse();
    m_presentation = Presentation::Cpu;
    ApplyPresentationVisibility();
    RefreshActiveDisplay();
}

void MandelRenderHost::SelectGpuPresentation()
{
    ReleaseMouse();
    m_presentation = Presentation::Gpu;
    ApplyPresentationVisibility();
    RefreshActiveDisplay();
}

void MandelRenderHost::SelectHeightFieldPresentation()
{
    ReleaseMouse();
    m_presentation = Presentation::HeightField;
    ApplyPresentationVisibility();
    RefreshActiveDisplay();
}

void MandelRenderHost::SelectPotentialFieldPresentation()
{
    ReleaseMouse();
    m_presentation = Presentation::PotentialField;
    ApplyPresentationVisibility();
    RefreshActiveDisplay();
}

void MandelRenderHost::OnCanvasPaint(wxPaintEvent &)
{
    wxPaintDC paint{&m_canvas};
    if (m_presentation == Presentation::HeightField)
    {
        RenderHeightFieldFrame(false);
    }
    else if (m_presentation == Presentation::PotentialField)
    {
        RenderHeightFieldFrame(true);
    }
    else if (m_presentation == Presentation::Gpu)
    {
        RenderGpuFrame();
    }
}

void MandelRenderHost::OnCpuPaint(wxPaintEvent &)
{
    wxPaintDC paint{&m_cpuDisplay};
    RenderCpuImage();
    if (m_cpuBitmap.IsOk())
    {
        paint.DrawBitmap(m_cpuBitmap, 0, 0, false);
    }
}

void MandelRenderHost::OnFrameSize(wxSizeEvent &event)
{
    LayoutDisplays();
    event.Skip();
}

void MandelRenderHost::OnMouseCaptureLost(wxMouseCaptureLostEvent &)
{
    m_mouseCapture = nullptr;
}

void MandelRenderHost::OnMouseLeftDown(wxMouseEvent &event)
{
    wxWindow *window = event_window(event);
    if (window == nullptr)
    {
        return;
    }

    m_mouseCapture = window;
    m_lastMousePosition = event.GetPosition();
    if (!window->HasCapture())
    {
        window->CaptureMouse();
    }
}

void MandelRenderHost::OnMouseLeftUp(wxMouseEvent &)
{
    ReleaseMouse();
}

void MandelRenderHost::OnMouseMove(wxMouseEvent &event)
{
    if (m_mouseCapture == nullptr || !event.LeftIsDown())
    {
        return;
    }

    const wxPoint position = event.GetPosition();
    const int delta_x = position.x - m_lastMousePosition.x;
    const int delta_y = position.y - m_lastMousePosition.y;
    if (delta_x == 0 && delta_y == 0)
    {
        return;
    }

    if (m_presentation == Presentation::HeightField || m_presentation == Presentation::PotentialField)
    {
        m_heightYawRadians += static_cast<float>(delta_x) * height_orbit_radians_per_pixel;
        m_heightPitchRadians =
            std::clamp(m_heightPitchRadians + static_cast<float>(delta_y) * height_orbit_radians_per_pixel,
                min_height_pitch_radians, max_height_pitch_radians);
        m_lastMousePosition = position;
        RefreshActiveDisplay();
        return;
    }

    const wxSize size = DisplaySize();
    m_viewport.pan_pixels(delta_x, delta_y, size.GetWidth(), size.GetHeight());
    m_lastMousePosition = position;
    InvalidateRender();
}

void MandelRenderHost::OnMouseWheel(wxMouseEvent &event)
{
    if (event.GetWheelDelta() == 0 || event.GetWheelRotation() == 0)
    {
        return;
    }

    const double clicks = static_cast<double>(event.GetWheelRotation()) / static_cast<double>(event.GetWheelDelta());
    const double scale = std::pow(zoom_scale_per_wheel_click, clicks);

    if (m_presentation == Presentation::HeightField || m_presentation == Presentation::PotentialField)
    {
        m_heightDistance =
            std::clamp(m_heightDistance * static_cast<float>(scale), min_height_distance, max_height_distance);
        RefreshActiveDisplay();
        return;
    }

    const wxPoint position = event.GetPosition();
    const wxSize size = DisplaySize();

    m_viewport.zoom_at(scale, position.x, position.y, size.GetWidth(), size.GetHeight());
    InvalidateRender();
}

void MandelRenderHost::OnTimer(wxTimerEvent &)
{
    RefreshActiveDisplay();
}

} // namespace mandel
