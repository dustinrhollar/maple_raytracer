#ifndef ENGINE_FRAME_INFO_FRAME_PARAMS_H
#define ENGINE_FRAME_INFO_FRAME_PARAMS_H

struct camera;

struct frame_params
{
    u64             Frame;
    
    //~ Timing
    u64             FrameStartTime;
    
    u64             GameStageEndTime;
    
    u64             RenderStageStartTime;
    u64             RenderStageEndTime;
    
    camera         *Camera;
    
    //~ Assets
    asset_t         Assets;
    u32             AssetsCount;
    
    //~ Texture Memory
    void *TextureBackbuffer;
    u32   TextureWidth;
    u32   TextureHeight;
    u32   PixelXOffset;
    u32   PixelYOffset;
    u32   ScanWidth;
    u32   ScanHeight;
    
    //~ Render Commands
    render_command  RenderCommands[100];
    u32             RenderCommandsCount;
};

void FrameParamsInit(frame_params      *FrameParams,
                     u32                Frame,
                     u64                FrameStartTime,
                     free_allocator    *Allocator,
                     renderer_t         Renderer,
                     resource_registry *ResourceRegistry,
                     asset_registry    *AssetRegistry);
void FrameParamsFree(frame_params *FrameParams, free_allocator *Allocator);

#endif //ENGINE_FRAME_INFO_FRAME_PARAMS_H
