
void FrameParamsInit(frame_params      *FrameParams,
                     u32                Frame,
                     u64                FrameStartTime,
                     free_allocator    *Allocator,
                     renderer_t         Renderer,
                     resource_registry *ResourceRegistry,
                     asset_registry    *AssetRegistry)
{
    FrameParams->Frame                = Frame;
    FrameParams->FrameStartTime       = FrameStartTime;
    FrameParams->GameStageEndTime     = 0;
    FrameParams->RenderStageStartTime = 0;
    FrameParams->RenderStageEndTime   = 0;
    FrameParams->RenderCommandsCount  = 0;
    
    //FrameParams->FrameHeapTag = { Frame, TAG_ID_GAME, 0 };
    //FrameParams->FrameHeap = TaggedHeapRequestAllocation(TaggedHeap, FrameParams->FrameHeapTag);
    
    FrameParams->Renderer = Renderer;
    
    CopyAssets(&FrameParams->Assets, &FrameParams->AssetsCount, AssetRegistry, Allocator);
    CopyResources(&FrameParams->Resources, &FrameParams->ResourcesCount, ResourceRegistry, Allocator);
}

void FrameParamsFree(frame_params *FrameParams, free_allocator *Allocator)
{
    pfree<asset>(Allocator, FrameParams->Assets);
    pfree<resource>(Allocator, FrameParams->Resources);
    
    FrameParams->Frame                = 0;
    FrameParams->FrameStartTime       = 0;
    FrameParams->GameStageEndTime     = 0;
    FrameParams->RenderStageStartTime = 0;
    FrameParams->RenderStageEndTime   = 0;
    FrameParams->RenderCommandsCount  = 0;
    FrameParams->AssetsCount          = 0;
    FrameParams->ResourcesCount       = 0;
    FrameParams->Assets               = NULL;
    FrameParams->Resources            = NULL;
    
    FrameParams->TextureBackbuffer = 0;
    FrameParams->TextureWidth = 0;
    FrameParams->TextureHeight = 0;
    FrameParams->PixelXOffset = 0;
    FrameParams->PixelYOffset = 0;
    FrameParams->ScanWidth = 0;
    FrameParams->ScanHeight = 0;
    
    
    //TaggedHeapReleaseAllocation(FrameParams->FrameHeap.TaggedHeap, FrameParams->FrameHeapTag);
    //FrameParams->FrameHeap.TaggedHeap = NULL;
}
