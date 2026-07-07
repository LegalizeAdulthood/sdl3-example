# Shadercross

Shadercross bridges an HLSL authoring pipeline to the shader formats
consumed by SDL GPU backends.  It lets the project keep HLSL as the source
language while producing the bytecode or source form expected by each
graphics API.

The bridge has two halves:

```text
HLSL -> DXC -> SPIR-V
SPIR-V -> backend shader format
```

The first half uses DXC as the HLSL front end.  The second half uses
SPIRV-Cross or a D3D compiler path to produce the final format.  For
Metal, the practical path is:

```text
HLSL -> DXC -> SPIR-V -> SPIRV-Cross -> MSL -> SDL Metal
```

For Vulkan, SPIR-V is already the final shader format.  For D3D12, the
final format is DXIL or DXBC.

This distinction matters because SDL GPU only accepts final backend shader
formats:

```text
Vulkan: SPIR-V
D3D12:  DXIL or DXBC
Metal:  MSL or metallib
```

`SDL_GPU_SHADERFORMAT_SPIRV` means final shader code for the Vulkan
backend.  Shadercross may also use SPIR-V as an intermediate format, but
that is distinct from the final SDL shader format.

Shadercross can run in the asset build, in the application, or both.  The
central design choice is where DXC, SPIRV-Cross, and shadercross are
allowed to exist.

## Static HLSL Assets

Static shaders are known at build time.  The simplest shipped application
contains final backend assets and no shader compiler stack.

Build one shader specification per logical shader:

```text
name
stage: vertex, fragment, or compute
source: HLSL file
entrypoint
defines
debug name
```

For each specification, generate final assets:

```text
HLSL -> SPIR-V        for Vulkan
HLSL -> DXIL or DXBC  for D3D12
HLSL -> MSL/metallib  for Metal
```

Preferred static outputs:

```text
Vulkan: SPIR-V
D3D12:  DXIL, unless DXBC compatibility is needed
Metal:  metallib when an Apple build step is available, otherwise MSL
```

A practical asset layout is:

```text
assets/shaders/triangle.vertex.spv
assets/shaders/triangle.vertex.dxil
assets/shaders/triangle.vertex.msl
assets/shaders/triangle.vertex.json
```

The JSON metadata file is application-owned.  Shadercross does not
generate or consume this file directly.  It records shadercross reflection
output and the final asset mapping needed by this project.

The metadata should contain enough SDL creation data to avoid runtime
reflection:

```text
name
stage
entrypoint
source hash
defines
output files and formats
num_samplers
num_storage_textures
num_storage_buffers
num_uniform_buffers
threadcount_x, threadcount_y, threadcount_z for compute
```

For graphics shaders, use SPIR-V reflection to fill:

```text
SDL_GPUShaderCreateInfo::num_samplers
SDL_GPUShaderCreateInfo::num_storage_textures
SDL_GPUShaderCreateInfo::num_storage_buffers
SDL_GPUShaderCreateInfo::num_uniform_buffers
```

For compute shaders, fill the matching fields in
`SDL_GPUComputePipelineCreateInfo`, including thread counts.

Shadercross can provide this build pipeline:

```text
HLSL -> SDL_ShaderCross_CompileSPIRVFromHLSL
SPIR-V -> SDL_ShaderCross_ReflectGraphicsSPIRV
SPIR-V -> SDL_ShaderCross_ReflectComputeSPIRV
SPIR-V -> SDL_ShaderCross_TranspileMSLFromSPIRV
HLSL -> SDL_ShaderCross_CompileDXILFromHLSL
HLSL -> SDL_ShaderCross_CompileDXBCFromHLSL
```

The Metal build can stop at MSL, or run Apple's tools to produce metallib:

```text
MSL -> xcrun metal -> AIR -> xcrun metallib -> metallib
```

At runtime, create the device with the union of final formats the
application ships:

```c
SDL_GPUShaderFormat formats =
    SDL_GPU_SHADERFORMAT_SPIRV |
    SDL_GPU_SHADERFORMAT_DXIL |
    SDL_GPU_SHADERFORMAT_MSL;

SDL_GPUDevice *device = SDL_CreateGPUDevice(formats, debug, NULL);
```

After device creation, query the actual formats and select an asset:

```c
SDL_GPUShaderFormat supported = SDL_GetGPUShaderFormats(device);
const char *driver = SDL_GetGPUDeviceDriver(device);
```

The selection rule should be explicit and deterministic:

```text
if Metal and metallib exists: use metallib
else if Metal and MSL exists: use MSL
else if D3D12 and DXIL exists: use DXIL
else if D3D12 and DXBC exists: use DXBC
else if Vulkan and SPIR-V exists: use SPIR-V
else fail with a missing shader asset error
```

Then load bytes from the selected file and call `SDL_CreateGPUShader` or
`SDL_CreateGPUComputePipeline` with the final format and metadata.

### Platform-Specific Packaging

The build does not have to package every generated backend asset into each
application bundle.  CMake platform logic can select only the needed
assets for the target platform:

```text
Windows package: DXIL or DXBC assets
macOS package:   MSL or metallib assets
Linux package:   SPIR-V assets
```

The same platform decision can select a small shader-loading shim.  The
shim owns the platform asset extension and final SDL shader format:

```text
Windows shim: load .dxil, create SDL_GPU_SHADERFORMAT_DXIL
macOS shim:   load .metallib or .msl, create METALLIB or MSL
Linux shim:   load .spv, create SDL_GPU_SHADERFORMAT_SPIRV
```

With that layout, common rendering code asks for a logical shader name and
does not branch on every backend format.  The platform shim maps that name
to the only packaged asset format for the build.

This model also narrows the `SDL_CreateGPUDevice` format mask.  A macOS
bundle that only ships metallib should advertise only
`SDL_GPU_SHADERFORMAT_METALLIB`; a Linux build that only ships SPIR-V
should advertise only `SDL_GPU_SHADERFORMAT_SPIRV`.

In this model the application does not need DXC, SPIRV-Cross, or
shadercross at runtime.  Shadercross is a build-system convenience:

- one HLSL source set
- one CLI/API for SPIR-V, DXIL/DXBC, and MSL output
- reflection data for SDL shader resource counts
- one place for entry points, profiles, defines, and debug flags
- fewer direct calls to DXC, SPIRV-Cross, `metal`, and `metallib`

Static assets should be covered by build tests:

- compile every shader specification
- reflect and write metadata
- verify every advertised backend has an asset
- fail on stale metadata by checking source hashes
- optionally create SDL shaders in a headless smoke test per backend

A different static model is possible:

```text
build:   HLSL -> SPIR-V
runtime: SPIR-V -> selected backend format
```

That avoids a DXC runtime dependency for Vulkan and Metal, but it still
puts shadercross and SPIRV-Cross in the application for Metal.  It also
requires a D3D shader compiler path at runtime if D3D output is produced
from SPIR-V.  Use this only when shipping one intermediate asset is worth
the runtime compiler dependency.

## Dynamic HLSL Assets

Dynamic shaders are not fully known at build time.  The application emits
or modifies HLSL at runtime.  That moves the compiler boundary into the
application:

```text
runtime: HLSL -> DXC -> SPIR-V -> platform format -> SDL GPU
```

Runtime dependencies follow from the requested input and output:

```text
HLSL -> SPIR-V       needs DXC
HLSL -> MSL          needs DXC and SPIRV-Cross
HLSL -> DXIL         needs DXC
SPIR-V -> MSL        needs SPIRV-Cross, not DXC
SPIR-V -> DXIL       needs a D3D compiler path
SPIR-V -> Vulkan     needs no translation
```

For dynamic HLSL, ship shadercross with the backends needed by the
application.  For HLSL input, shadercross must be built with DXC support.
For Metal output, shadercross must have SPIRV-Cross support.  The packaged
application must include the platform libraries those features require.

Initialize shadercross before creating the GPU device:

```c
if (!SDL_ShaderCross_Init()) {
    SDL_Log("shadercross init failed: %s", SDL_GetError());
    return false;
}

SDL_GPUShaderFormat formats = SDL_ShaderCross_GetHLSLShaderFormats();
SDL_GPUDevice *device = SDL_CreateGPUDevice(formats, debug, NULL);
```

If `formats` is zero, the runtime compiler stack is not usable.  Fail
early with a diagnostic that names the missing compiler feature.

Each generated graphics shader follows this path:

```text
generate HLSL source
fill SDL_ShaderCross_HLSL_Info
call SDL_ShaderCross_CompileSPIRVFromHLSL
call SDL_ShaderCross_ReflectGraphicsSPIRV
call SDL_ShaderCross_CompileGraphicsShaderFromSPIRV
store SDL_GPUShader
```

For compute:

```text
generate HLSL source
fill SDL_ShaderCross_HLSL_Info
call SDL_ShaderCross_CompileSPIRVFromHLSL
call SDL_ShaderCross_ReflectComputeSPIRV
call SDL_ShaderCross_CompileComputePipelineFromSPIRV
store SDL_GPUComputePipeline
```

`SDL_ShaderCross_CompileGraphicsShaderFromSPIRV` is device aware.  It uses
the SDL GPU device formats to decide the final format:

```text
Vulkan: SPIR-V -> SDL_CreateGPUShader(SPIR-V)
Metal:  SPIR-V -> MSL -> SDL_CreateGPUShader(MSL)
D3D12:  SPIR-V -> D3D format -> SDL_CreateGPUShader(DXIL/DXBC)
```

Dynamic compilation should run off the render path.  A typical runtime
system has:

- a shader generation step that produces deterministic HLSL
- a compile queue running on worker threads
- a cache keyed by all inputs that affect output
- a main-thread handoff that installs completed SDL shader objects
- a fallback shader or disabled draw path on failure

The cache key must include:

```text
generated HLSL source
entrypoint
stage
defines
target SDL shader format
selected SDL GPU driver
shadercross version
DXC version when HLSL input is used
SPIRV-Cross version when translation is used
debug flags
resource binding policy
```

The cache value may store final backend bytes, SPIR-V bytes, reflection
metadata, or created SDL objects.  Persisted caches should store bytes and
metadata, not raw SDL handles.

Keep generated HLSL inside the portable subset expected by the pipeline:

- explicit entry points and stages
- stable resource bindings
- stable vertex inputs and fragment outputs
- no backend-specific assumptions unless guarded by the generator
- generated debug names tied to the cache key

Error handling is part of the pipeline.  On failure, record:

- generated HLSL
- entrypoint, stage, and defines
- selected SDL GPU driver
- selected final shader format
- full `SDL_GetError()` text
- cache key

Dynamic HLSL should be reserved for cases that really require source
generation.  If the dynamic part is data, prefer uniforms or buffers.  If
the permutation set is bounded, prefer prebuilt variants and the static
pipeline.

### Platform-Specific Runtime

The dynamic pipeline also does not have to expose every backend path in
every application build.  CMake platform logic can select the dynamic
shader path and runtime files needed by the target platform:

```text
Windows package: shadercross plus DXC runtime for DXIL or DXBC
macOS package:   shadercross plus DXC dylibs for HLSL -> MSL
Linux package:   shadercross plus DXC runtime for HLSL -> SPIR-V
```

The same platform decision can select a shader-compilation shim.  The shim
owns the final target format and the path used to get there:

```text
Windows shim: HLSL -> DXIL, create SDL_GPU_SHADERFORMAT_DXIL
macOS shim:   HLSL -> SPIR-V -> MSL, create SDL_GPU_SHADERFORMAT_MSL
Linux shim:   HLSL -> SPIR-V, create SDL_GPU_SHADERFORMAT_SPIRV
```

With that layout, common rendering code submits generated HLSL and a
logical shader description.  The platform shim chooses the entry point,
stage, target format, reflection path, and final SDL creation call.

This also narrows the `SDL_CreateGPUDevice` format mask for dynamic
builds.  A macOS build that only compiles to MSL should advertise only
`SDL_GPU_SHADERFORMAT_MSL`; a Linux Vulkan build should advertise only
`SDL_GPU_SHADERFORMAT_SPIRV`; a D3D12 build should advertise DXIL or DXBC.

If the shim uses `SDL_ShaderCross_GetHLSLShaderFormats()`, intersect that
mask with the platform policy before creating the device.  The shadercross
library may be capable of more formats than a given package intends to
support.

### Runtime Compiler Packaging

`needs DXC` does not mean shadercross searches for a `dxc` executable.
Shadercross is built with `SDLSHADERCROSS_DXC` and links to the
DirectXShaderCompiler library target.  The HLSL path calls
`DxcCreateInstance` from that library and passes compiler arguments in
process.

The exact binding is set when shadercross is built:

```text
SDLSHADERCROSS_DXC=ON
target_link_libraries(... DirectXShaderCompiler::dxcompiler)
compile definition: SDL_SHADERCROSS_DXC
```

The vcpkg port in this tree builds shadercross against vcpkg packages, not
vendored shadercross dependencies:

```text
dependencies: directx-dxc, sdl3[vulkan], spirv-cross
SDLSHADERCROSS_INSTALL=ON
SDLSHADERCROSS_INSTALL_RUNTIME=OFF
SDLSHADERCROSS_SPIRVCROSS_SHARED=OFF
SDLSHADERCROSS_VENDORED=OFF
```

It does not pass `SDLSHADERCROSS_DXC`, so shadercross keeps the upstream
default, which is `ON`.  The port applies
`fix-directx-shader-compiler-includes.patch`; that patch removes the
`dxcapi.h` include-path requirement from shadercross's DXC finder.  The
shadercross source declares the minimal DXC interfaces it calls and links
the DXC library target.

For a dynamic application, package the DXC runtime libraries needed by the
link.  In practice that means the platform `dxcompiler` library and, when
required by the DXC package, its `dxil` validation library:

```text
Windows: dxcompiler.dll and dxil.dll
macOS:   libdxcompiler.dylib and libdxil.dylib
Linux:   libdxcompiler.so and libdxil.so
```

They must be reachable by the platform dynamic loader through the bundle
layout, RPATH, PATH, or equivalent loader search path.  Shadercross does
not locate them by path at shader compile time.  The process must already
be able to load the linked libraries.

This repository enables `vcpkg-overlay-ports` in
`vcpkg-configuration.json`, so `directx-dxc` resolves to the overlay port.
That overlay supports Windows, Linux x64, and macOS.  On macOS it builds
DXC from source instead of downloading the Windows/Linux binary package.

The overlay `directx-dxc` port installs the DXC shared libraries and also
installs the `dxc` command-line tool under `tools/directx-dxc`.  The tool
is useful for manual builds, but shadercross does not invoke it.  The
runtime API path uses the linked `dxcompiler` library.

The same port declares that DXC always requires dynamic runtime linkage,
even with a static vcpkg triplet.  On macOS it installs
`libdxcompiler*.dylib` and `libdxil*.dylib`; those dylibs must be packaged
with any application that compiles dynamic HLSL through shadercross.

`needs SPIRV-Cross` has the same shape.  Shadercross links to the
SPIRV-Cross C API library and calls `spvc_*` functions in process.  It
does not execute the `spirv-cross` command-line tool.

The exact binding is also chosen at shadercross build time:

```text
shared SPIRV-Cross: target_link_libraries(... spirv-cross-c-shared)
static SPIRV-Cross: target_link_libraries(... spirv-cross-c)
```

The local `spirv-cross` vcpkg port enforces static library linkage.
Because the shadercross port sets `SDLSHADERCROSS_SPIRVCROSS_SHARED=OFF`,
SPIRV-Cross is linked statically into shadercross.  The `spirv-cross` tool
may be copied to `tools/spirv-cross`, but shadercross does not invoke it.
This vcpkg build needs no additional SPIRV-Cross runtime files packaged
with the application.

If shadercross is linked to shared SPIRV-Cross, package the SPIRV-Cross C
runtime library beside the application.  If shadercross is linked
statically to SPIRV-Cross, there is no separate SPIRV-Cross runtime file
to package.

The same rule applies to DXC.  A static or fully vendored shadercross
build can reduce runtime files.  A shared dependency build requires those
shared libraries to be shipped and made loader-visible.
