//asset.cpp

void AssetRegistryInit(asset_registry *Registry, renderer_t Renderer, free_allocator *GlobalMemoryAllocator,
                       u32 MaximumAssets)
{
    Registry->Renderer    = Renderer;
    Registry->AssetsMax   = MaximumAssets;
    Registry->AssetsCount = 0;
    
    u64 RequiredAllocatorMemory = Registry->AssetsMax * sizeof(asset);
    u64 RequiredResourceTMemory = Registry->AssetsMax * sizeof(asset_t);
    
    void *AllocatorMemory = FreeListAllocatorAlloc(GlobalMemoryAllocator, RequiredAllocatorMemory);
    PoolAllocatorInit(&Registry->AssetAllocator, AllocatorMemory, RequiredAllocatorMemory, sizeof(asset));
    
    // Resource list -  resizable, from global memory
    Registry->Assets = (asset_t*)FreeListAllocatorAlloc(GlobalMemoryAllocator, RequiredResourceTMemory);
}

void AssetRegistryFree(asset_registry *Registry, free_allocator *GlobalMemoryAllocator)
{
    FreeListAllocatorAllocFree(GlobalMemoryAllocator, Registry->AssetAllocator.Start);
    PoolAllocatorFree(&Registry->AssetAllocator);
    
    FreeListAllocatorAllocFree(GlobalMemoryAllocator, Registry->Assets);
    Registry->Assets = NULL;
    
    Registry->AssetsMax   = 0;
    Registry->AssetsCount = 0;
    Registry->Renderer    = NULL;
}


asset_id CreateAsset(asset_registry *Registry, asset_type Type, void *CreateInfo)
{
    asset_id Result = {};
    Result.Type        = Type;
    Result.Gen         = 0;
    Result.Active      = 0; // start off as not active
    
    if (Registry->AssetsCount+1 > Registry->AssetsMax)
    {
        mprinte("asset max has been reached. Cannot add anymore assets!\n");
        return Result;
    }
    
    switch (Type)
    {
        case Asset_Sphere:
        {
            sphere_create_info *Info = (sphere_create_info*)CreateInfo;
            
            asset_sphere Sphere = {};
            Sphere.Origin   = Info->Origin;
            Sphere.Radius   = Info->Radius;
            Sphere.Material = Info->Material;
            
            // Active the Id
            Result.Index  = Registry->AssetsCount++;
            Result.Active = 1;
            
            // Create the resource
            asset *Asset       = (asset*)PoolAllocatorAlloc(&Registry->AssetAllocator);
            Asset->Id          = Result;
            Asset->Sphere      = Sphere;
            
            // Insert it into the list
            Registry->Assets[Asset->Id.Index] = Asset;
            
        } break;
        
        
        default: mprinte("Unknown asset type!\n"); break;
    };
    
    return Result;
}

void CopyAssets(asset_t *Assets, u32 *AssetsCount, asset_registry *AssetRegistry, tag_block_t Block)
{
    if (AssetRegistry->AssetsCount > 0)
    {
        asset_t AssetsCopy = (asset_t)TaggedHeapBlockAlloc(Block, sizeof(asset) * AssetRegistry->AssetsCount);
        if (AssetsCopy)
        {
            for (u32 i = 0; i < AssetRegistry->AssetsCount; ++i)
            {
                AssetsCopy[i] = *AssetRegistry->Assets[i];
            }
            
            *AssetsCount = AssetRegistry->AssetsCount;
            *Assets = AssetsCopy;
        }
        else
        {
            mprinte("Unable to allocate from the heap block!\n");
        }
    }
}

inline bool IsValidAsset(asset_t Assets, asset_id Id)
{
    return Id.Active && (Assets[Id.Index].Id.Gen == Id.Gen);
}
