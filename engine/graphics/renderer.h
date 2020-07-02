#ifndef ENGINE_GRAPHICS_RENDERER_H
#define ENGINE_GRAPHICS_RENDERER_H

typedef struct renderer* renderer_t;

struct renderer
{
    resource_id Device;
    resource_id RenderTarget;
    
    resource_id Pipeline;
    
    u32 TextureWidth;
    u32 TextureHeight;
    resource_id RaytracedTexture;
    
    u64           TextureSize;
    void         *NotSafeMemory;
    safe_memory_t Texture;
};

void RendererInit(renderer_t        *Renderer,
                  free_allocator    *Allocator,
                  resource_registry *Registry,
                  window_t           Window,
                  u32                Width,
                  u32                Height,
                  u32                RefreshRate);
void RendererShutdown(renderer_t     *Renderer,
                      free_allocator *Allocator);

void RendererResize(renderer_t Renderer, resource_registry *Registry);
void RendererEntry(renderer_t Renderer, resource_registry *Registry, frame_params *FrameParams);

#endif //ENGINE_GRAPHICS_RENDERER_H
