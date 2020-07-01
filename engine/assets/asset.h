#ifndef ENGINE_ASSETS_ASSET_H
#define ENGINE_ASSETS_ASSET_H

struct resource_id;
struct resource_registry;
typedef struct asset* asset_t;


struct asset_id
{
    u64 Index:48; // Allows for 2^48 assets
    u64 Gen:8;    // Allows for 2^8 generations before overflow
    u64 Type:7;   // Allows for 2^7 different types of of assets
    u64 Active:1; // Whether or not this asset id is active. NOTE(Dustin): Needed?
};

enum asset_type
{
    Asset_Sphere,
};

struct sphere_create_info
{
    vec3 Origin;
    r32  Radius;
};

struct asset_sphere
{
    vec3 Origin;
    r32  Radius;
};

struct asset
{
    asset_id Id;
    union
    {
        asset_sphere Sphere;
    };
};

struct asset_registry
{
    // the asset registry does not *own* this pointer.
    renderer_t      Renderer;
    
    // Allocator managing device resources
    pool_allocator  AssetAllocator;
    
    // Resource list -  non-resizable, from global memory
    asset_t        *Assets;
    u32             AssetsMax;
    u32             AssetsCount;
    
    // TODO(Dustin): Free Indices
    // See note left in the resource_registry
};


void AssetRegistryInit(asset_registry *Registry, renderer_t Renderer, free_allocator *GlobalMemoryAllocator,
                       u32 MaximumAssets);
void AssetRegistryFree(asset_registry *Registry, free_allocator *GlobalMemoryAllocator);

asset_id CreateAsset(asset_registry *Registry, asset_type Type, void *CreateInfo);
void CopyAssets(asset_t *Assets, u32 *AssetsCount, asset_registry *AssetRegistry, free_allocator *Allocator);

inline bool IsValidAsset(asset_t Assets, asset_id Id);

#endif //ENGINE_ASSETS_ASSET_H
