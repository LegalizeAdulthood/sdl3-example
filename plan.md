# SDL3 GPU Mandelbrot Demo

Use a Mandelbrot set compute shader for the compute-to-texture
demonstration.  Build a clean standalone sample repository.

## Goal

Build a small C++17 wxWidgets sample program that demonstrates SDL3 GPU
API by:

1. opening a wxWidgets frame,
2. embedding an SDL-backed `wxsdl::SdlCanvas`,
3. creating an SDL GPU device,
4. dispatching a compute shader,
5. writing Mandelbrot iteration counts into a GPU texture,
6. displaying that texture through a simple graphics pipeline,
7. controlling the Mandelbrot view with wx mouse events.

The code should remain small enough to explain in one meeting.

## Current progress

The repository now has the foundation targets needed for the demo:

* `sdlcpp` owns small RAII wrappers around SDL handles and SDL GPU
  handles.
* `wxsdl` owns `wxsdl::SdlCanvas`, a `wxWindow` that attaches an SDL
  window to the native wx widget on Win32, Cocoa, and GTK/X11.
* `mandel` is a wxWidgets GUI executable.  Its main frame creates and
  lays out an `SdlCanvas`.
* Unit tests cover the move-only wrapper shape and the `SdlCanvas`
  inheritance contract.

Future Mandelbrot code should build on this structure.  Do not go back to
a raw SDL-only application unless the wx host is deliberately removed.

## Existing support targets

The current low-level targets are:

```cmake
add_subdirectory(core)
add_subdirectory(sdlcpp)
add_subdirectory(wxsdl)
```

`sdlcpp` is the SDL C API wrapper layer.  It already provides wrappers for
the SDL library lifetime, properties, windows, GPU devices, command
buffers, textures, buffers, shaders, samplers, pipelines, passes, fences,
transfer buffers, swapchain textures, and GPU window claims.

`core` is the domain layer.  Keep Mandelbrot parameters, viewport mapping,
iteration/color data, CPU reference code, and render input/output types
there.

`wxsdl` is the wx/SDL bridge.  Its public surface is currently
`wxsdl::SdlCanvas`, which exposes the attached `sdlcpp::Window`.

## Core Mandelbrot code

Add Mandelbrot code to the existing `core` target.  Do not create a
separate Mandelbrot library target.  Drop the stub `core.cpp` and
`core.h` files when the new code is added.

`libs/core/CMakeLists.txt`:

```cmake
add_library(core
    include/core/MandelParams.h
    include/core/MandelReference.h
    include/core/Palette.h
    MandelReference.cpp
    Palette.cpp
)
```

## Mandel parameter block

Use a simple parameter block for both the CPU reference code and the GPU
shader.

C++ version:

```cpp
#pragma once

namespace core
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

} // namespace core
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

#include "core/MandelParams.h"

namespace core
{

int mandel_reference_pixel(const MandelParams& params, int px, int py);

} // namespace core
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
find_package(wxWidgets CONFIG REQUIRED)

add_executable(mandel main.cpp)
set_target_properties(
    mandel
    PROPERTIES WIN32_EXECUTABLE "$<PLATFORM_ID:Windows>")
target_link_libraries(mandel PUBLIC core wxsdl)
target_folder(mandel "Tools")
```

The current tool is only the wx application shell.  Add core Mandelbrot
code and shader targets when the CPU reference and GPU render path are
introduced.  Keep SDL window access through
`wxsdl::SdlCanvas::window()`.

## SDL3 GPU frame flow

The runtime frame flow should be driven by the wx event loop.  The main
frame owns the canvas; rendering should live in a canvas-owned object or a
small controller attached to the canvas.

```text
wx dispatches paint, size, timer, and mouse events

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

Handle resize from wx size events.  Handle mouse input through wx mouse
events on the canvas.  Do not add a second raw SDL event loop.

## Input ownership

Mouse input is handled by wxWidgets.  SDL is used for the window handle
and GPU presentation path, not for application mouse input.  Do not poll
or process SDL mouse events in the Mandelbrot tool.

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
drag      pan
wheel     zoom in/out
```

Handle these through wx mouse events on the canvas.  Do not implement a
full UI.  This is a graphics API sample, not a full fractal application
UI.

## Implementation Slices

### 1. CPU reference

* Implement `MandelParams`.
* Implement `mandel_reference_pixel`.
* Add unit tests for a few known points:
  * `c = 0 + 0i` should be inside.
  * `c = 2 + 2i` should escape quickly.
  * periodicity on/off should both produce valid bounded results.

### 2. Canvas render host

* Add a canvas-owned render controller or small frame-owned controller.
* Drive rendering from wx paint, size, and timer events.
* Keep mouse input in wx.
* Use the existing `SdlCanvas` window; do not create a second SDL window.

### 3. SDL3 GPU device

* Create the SDL GPU device with the existing `sdlcpp` wrappers.
* Claim `SdlCanvas::window()` for the GPU device.
* Acquire and submit an empty command buffer.
* Clear the swapchain to verify presentation.

### 4. Fullscreen triangle

* Add `blit.vert.hlsl`.
* Add `blit.frag.hlsl`.
* Create a graphics pipeline.
* Draw a fullscreen triangle.

### 5. Compute output texture

* Create an `R32_UINT` texture with compute storage usage.
* Create the Mandelbrot compute pipeline.
* Dispatch `ceil(width / 16), ceil(height / 16), 1`.
* Display the result through the fragment shader.

### 6. Mouse interaction

* Add mouse drag panning.
* Add mouse wheel zoom.
* Recompute parameters after mouse interaction.

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
3. run wrapper and CPU unit tests.

CI should not require a real GPU device.  GPU runtime testing should be
manual or opt-in.  Building the wx tool is fine; launching it in CI is
not required.

If Linux SDL3 dependencies become noisy, either:

1. install the required X11/Wayland development packages in the
   workflow, or
2. make the SDL tool target optional with a CMake option.

Suggested option if needed:

```cmake
option(
    SDL3_GPU_MANDEL_BUILD_TOOL
    "Build SDL3 GPU Mandelbrot demo executable"
    ON)
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
8. Mouse-driven navigation.
9. Practical vcpkg/CMake/wxWidgets/shadercross issues.
10. Where SDL3 GPU is useful and where it is not.

## Done

The sample is complete when:

```text
The program opens a wx frame with an SdlCanvas, dispatches a Mandelbrot
compute shader, writes iteration counts into an R32_UINT texture,
displays the result, and lets the user pan and zoom with the mouse.
```
