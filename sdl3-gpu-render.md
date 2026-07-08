# SDL3 GPU Render Loop

Typical SDL3 GPU frame:

1. One-time setup:
   - `SDL_Init`
   - create window
   - create `SDL_GPUDevice`
   - claim the window for the GPU device
   - create shaders
   - create pipelines
   - create textures, buffers, samplers, transfer buffers

2. Upload static data:
   - map upload transfer buffer
   - write CPU data
   - acquire command buffer
   - begin copy pass
   - upload or copy into GPU buffers or textures
   - end copy pass
   - submit command buffer

3. Per frame:
   - acquire command buffer
   - acquire swapchain texture for the window
   - update uniforms or upload changed data

4. Optional compute:
   - begin compute pass
   - bind compute pipeline
   - bind storage buffers or textures
   - push compute uniforms
   - dispatch workgroups
   - end compute pass

5. Render:
   - begin render pass targeting the swapchain texture
   - bind graphics pipeline
   - bind vertex buffers, index buffers, textures, samplers, or storage
     resources
   - push vertex or fragment uniforms
   - issue draw call
   - end render pass

6. Submit:
   - submit command buffer
   - GPU executes recorded work in order
   - acquired swapchain texture is presented

7. CPU sync only when needed:
   - use a fence or wait if CPU must observe GPU completion
   - otherwise keep CPU and GPU running independently
