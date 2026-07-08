# When To Use SDL3 GPU

SDL3 GPU is useful when:

- You need portable modern GPU access over Vulkan, D3D12, and Metal.
- You need compute, storage textures/buffers, custom shaders, render passes.
- SDL_Renderer is too small, but native Vulkan/D3D12/Metal is too much.
- You can live inside SDL's common feature subset.
- You want one application GPU path, with per-backend shader assets.

SDL3 GPU is not useful when:

- You need full native API control.
- You need advanced/native-only features: ray tracing, mesh shaders, bindless,
  sparse resources, explicit residency, complex queue ownership, vendor
  extensions.
- You are building a serious engine that already owns Vulkan/D3D12/Metal
  backends.
- You only need simple 2D drawing; use SDL_Renderer.
- You want SDL to hide shader portability completely. It does not.
