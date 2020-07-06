#ifndef ENGINE_GRAPHICS_RENDERER_H
#define ENGINE_GRAPHICS_RENDERER_H

typedef struct renderer* renderer_t;

struct render_settings
{
    // Window Settings
    // TODO(Dustin): Restrict window sizes by aspect ratios
    i32 WindowWidth;
    i32 WindowHeight;
    
    // Render Settings
    i32     ScanWidth;
    i32     ScanHeight;
    i32     SamplesPerPixel;
    i32     SampleDepth;
    
    // Scene Settings
    scene   Scenes[2];
    u32     ScenesCount = 2;
    scene  *ActiveScene;
    
    // Camera Settings
    struct camera Camera;
    vec3 LookFrom, LookAt, Up;
    r32 vFov, AspectRatio, Aperture, FocusDist, Time0, Time1;
};

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
    
    render_settings RenderSettings;
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

void RendererClearRaytracedTexture(renderer_t Renderer, resource_registry *Registry);

#endif //ENGINE_GRAPHICS_RENDERER_H
