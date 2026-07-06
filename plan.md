# SDL3 GPU Mandelbrot Demo

Use a Mandelbrot set compute shader for the compute-to-texture
demonstration.  Build a clean standalone sample repository.

## Goal

Build a small C++17 sample program that demonstrates SDL3 GPU API by:

1. creating an SDL3 window,
2. creating an SDL GPU device,
3. dispatching a compute shader,
4. writing Mandelbrot iteration counts into a GPU texture,
5. displaying that texture through a simple graphics pipeline,
6. toggling periodicity checking at runtime.

The code should remain small enough to explain in one meeting.

## Library target

`libs/CMakeLists.txt`:

```cmake
add_library(mandel STATIC
    include/mandel/MandelParams.h
    include/mandel/MandelReference.h
    include/mandel/Palette.h
    MandelReference.cpp
    Palette.cpp
)

target_include_directories(mandel PUBLIC include)
target_folder(mandel "Libraries")
```

## Mandel parameter block

Use a simple parameter block for both the CPU reference code and the GPU
shader.

C++ version:

```cpp
#pragma once

namespace mandel
{

struct MandelParams
{
    double x_min;
    double y_min;
    double dx;
    double dy;
    double z0_real;
    double z0_imag;
    double bailout;
    double close_enough;
    int width;
    int height;
    int max_iterations;
    int first_saved_and;
    int periodicity_next_saved_incr;
    int periodicity_check;
};

} // namespace mandel
```

Initial values:

```cpp
MandelParams params{
    -2.5,     // x_min
    -1.5,     // y_min
    4.0 / 800.0,
    3.0 / 600.0,
    0.0,      // z0_real
    0.0,      // z0_imag
    4.0,      // bailout
    1.0e-7,   // close_enough
    800,
    600,
    1000,
    15,       // first_saved_and
    10,       // periodicity_next_saved_incr
    1         // periodicity_check
};
```

## CPU reference API

`MandelReference.h`:

```cpp
#pragma once

#include "mandel/MandelParams.h"

namespace mandel
{

int mandel_reference_pixel(const MandelParams& params, int px, int py);

} // namespace mandel
```

`MandelReference.cpp` should implement a straightforward Mandelbrot pixel
evaluator:

1. map pixel coordinate to complex coordinate `c`,
2. initialize `z = c + z0`,
3. iterate `z = z*z + c`,
4. stop when `x*x + y*y >= bailout`,
5. optionally stop early when periodicity is detected,
6. return the iteration count or `max_iterations` for inside points.

The GPU shader should match this CPU reference implementation.  Avoid
unrelated engine state.

## CPU periodicity checking

Use per-pixel periodicity checking in the sample.  Do not add cross-pixel
adaptive state.

Reference pseudocode:

```text
saved_x = 0
saved_y = 0
saved_and = first_saved_and
saved_incr = 1

for iter in 1 .. max_iterations:
    z = z*z + c

    if norm(z) >= bailout:
        return iter

    if periodicity_check:
        if (iter & saved_and) == 0:
            saved_x = z.x
            saved_y = z.y
            saved_incr = saved_incr - 1

            if saved_incr == 0:
                saved_and = (saved_and << 1) + 1
                saved_incr = periodicity_next_saved_incr
        else:
            if abs(saved_x - z.x) < close_enough and
               abs(saved_y - z.y) < close_enough:
                return max_iterations

return max_iterations
```

This keeps the algorithm simple and GPU-friendly.

## Tool target

`tools/CMakeLists.txt`:

```cmake
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_shadercross CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_executable(sdl3-gpu-mandel
    main.cpp
    SdlGpuContext.cpp
    Shader.cpp
)

target_link_libraries(sdl3-gpu-mandel
    PRIVATE
        mandel
        SDL3::SDL3
        SDL3_shadercross::SDL3_shadercross
        fmt::fmt
)

target_folder(sdl3-gpu-mandel "Tools")
```

## SDL3 GPU frame flow

The runtime frame flow should be:

```text
poll SDL events

if parameters changed:
    update MandelParams buffer

command_buffer = SDL_AcquireGPUCommandBuffer(device)

begin compute pass
    bind Mandelbrot compute pipeline
    bind output texture as writable storage texture
    bind or push MandelParams
    dispatch ceil(width / 16), ceil(height / 16), 1
end compute pass

acquire swapchain texture

begin render pass
    bind fullscreen blit pipeline
    bind iteration texture
    draw fullscreen triangle
end render pass

submit command buffer
```

## Texture strategy

Start with a two-stage output path:

1. compute shader writes raw iteration counts to an `R32_UINT` texture,
2. fragment shader maps iteration count to color.

This keeps the fractal math separate from palette display.

Possible future texture formats:

```text
R32_UINT       raw iteration count
RGBA8         direct color output
RGBA16_FLOAT  HDR-ish display path
RGBA32_FLOAT  full float accumulation experiments
```

Use `R32_UINT` first.

## Shader language

Use HLSL first and route through SDL_shadercross.

Shader files:

```text
tools/shaders/mandel.comp.hlsl
tools/shaders/blit.vert.hlsl
tools/shaders/blit.frag.hlsl
```

Use `float`, not `double`, in the shader.  That keeps the demo focused on
SDL3 GPU portability.  Deep zoom and double precision can be discussed as
limitations and future work.

## Compute shader sketch

`mandel.comp.hlsl`:

```hlsl
struct MandelParams
{
    float x_min;
    float y_min;
    float dx;
    float dy;
    float z0_real;
    float z0_imag;
    float bailout;
    float close_enough;
    int width;
    int height;
    int max_iterations;
    int first_saved_and;
    int periodicity_next_saved_incr;
    int periodicity_check;
};

RWTexture2D<uint> output_tex : register(u0);
ConstantBuffer<MandelParams> params : register(b0);

[numthreads(16, 16, 1)]
void main(uint3 tid : SV_DispatchThreadID)
{
    if (tid.x >= params.width || tid.y >= params.height)
    {
        return;
    }

    float cx = params.x_min + float(tid.x) * params.dx;
    float cy = params.y_min + float(tid.y) * params.dy;

    float x = cx + params.z0_real;
    float y = cy + params.z0_imag;

    float saved_x = 0.0;
    float saved_y = 0.0;
    int saved_and = params.first_saved_and;
    int saved_incr = 1;

    for (int iter = 1; iter < params.max_iterations; ++iter)
    {
        float x2 = x * x;
        float y2 = y * y;
        float xy = x * y;

        x = x2 - y2 + cx;
        y = 2.0 * xy + cy;

        if (x * x + y * y >= params.bailout)
        {
            output_tex[tid.xy] = uint(iter);
            return;
        }

        if (params.periodicity_check != 0)
        {
            if ((iter & saved_and) == 0)
            {
                saved_x = x;
                saved_y = y;
                --saved_incr;

                if (saved_incr == 0)
                {
                    saved_and = (saved_and << 1) + 1;
                    saved_incr = params.periodicity_next_saved_incr;
                }
            }
            else if (abs(saved_x - x) < params.close_enough &&
                     abs(saved_y - y) < params.close_enough)
            {
                output_tex[tid.xy] = uint(params.max_iterations);
                return;
            }
        }
    }

    output_tex[tid.xy] = uint(params.max_iterations);
}
```

## Blit shaders

Use a fullscreen triangle.  The vertex shader can synthesize positions
from `SV_VertexID`; no vertex buffer is needed.

The fragment shader should sample or load the iteration texture and
convert iteration counts to color.

Initial coloring can be simple:

```text
iteration == max_iterations -> black
otherwise                   -> palette(iteration)
```

Keep palette logic deliberately small for the talk.

## Runtime controls

Minimal controls:

```text
Esc       quit
arrows    pan
+ / -     zoom in/out
[ / ]     decrease/increase max iterations
p         toggle periodicity checking
r         reset view
1         show GPU compute output
2         show CPU reference output, optional later
```

Do not implement a full UI.  This is a graphics API sample, not a full
fractal application UI.

## Build order

### 1. CPU reference

* Implement `MandelParams`.
* Implement `mandel_reference_pixel`.
* Add unit tests for a few known points:
  * `c = 0 + 0i` should be inside.
  * `c = 2 + 2i` should escape quickly.
  * periodicity on/off should both produce valid bounded results.

### 2. SDL3 window

* Initialize SDL.
* Create an SDL window.
* Handle events.
* Quit cleanly.

### 3. SDL3 GPU device

* Create the SDL GPU device.
* Claim the SDL window for the GPU device.
* Acquire and submit an empty command buffer.
* Clear the swapchain to verify presentation.

### 4. fullscreen triangle

* Add `blit.vert.hlsl`.
* Add `blit.frag.hlsl`.
* Create a graphics pipeline.
* Draw a fullscreen triangle.

### 5. compute output texture

* Create an `R32_UINT` texture with compute storage usage.
* Create the Mandelbrot compute pipeline.
* Dispatch `ceil(width / 16), ceil(height / 16), 1`.
* Display the result through the fragment shader.

### 6. periodicity toggle

* Add `periodicity_check` to the parameter buffer.
* Toggle with `p`.
* Show difference in runtime or behavior.

### 7. CPU/GPU comparison, optional

* Add texture readback.
* Compare selected pixels against `mandel_reference_pixel`.
* Keep this out of the default render path.

## CI

Keep CI simple.

Build on:

```text
ubuntu-latest
windows-latest
macos-latest
```

Keep that matrix.

CI should:

1. configure,
2. build,
3. run CPU unit tests.

CI should not require a real GPU device.  GPU runtime testing should be
manual or opt-in.

If Linux SDL3 dependencies become noisy, either:

1. install the required X11/Wayland development packages in the
   workflow, or
2. make the SDL tool target optional with a CMake option.

Suggested option if needed:

```cmake
option(SDL3_GPU_MANDEL_BUILD_TOOL "Build SDL3 GPU Mandelbrot demo executable" ON)
```

## Out of scope

Do not include:

* no deep zoom,
* no arbitrary precision,
* no OpenEXR output,
* no production application integration,
* no general fractal plugin system,
* no complete palette editor,
* no GPU runtime tests in CI,
* no attempt to reproduce unrelated global-state behavior.

## Presentation outline

Use this structure:

1. Why SDL3 GPU exists.
2. How it differs from SDL_Renderer.
3. How it resembles Vulkan/D3D12/Metal.
4. CPU Mandelbrot reference.
5. First compute shader.
6. Compute-to-texture path.
7. Presenting the texture.
8. Adding periodicity checking.
9. Practical vcpkg/CMake/shadercross issues.
10. Where SDL3 GPU is useful and where it is not.

## Done

The sample is complete when:

```text
The program opens a window, dispatches a Mandelbrot compute shader,
writes iteration counts into an R32_UINT texture, displays the result,
and lets the user toggle periodicity checking with the p key.
```
