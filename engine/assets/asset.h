#ifndef ENGINE_ASSETS_ASSET_H
#define ENGINE_ASSETS_ASSET_H

struct resource_id;
struct resource_registry;
typedef struct asset* asset_t;

struct lambertian
{
    vec3 Albedo;
};

struct metal
{
    vec3 Albedo;
    r32  Fuzz;
};

struct dielectric
{
    r32 IndexOfRefraction;
};

enum material_type
{
    Material_Lambertian,
    Material_Metal,
    Material_Dielectric,
};

struct material
{
    material_type Type;
    
    union
    {
        lambertian Lambertian;
        metal      Metal;
        dielectric Dielectric;
    };
};

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
    Asset_DynamicSphere,
};

struct sphere_create_info
{
    vec3     Origin;
    r32      Radius;
    material Material;
};

struct dynamic_sphere_create_info
{
    r32      Radius;
    material Material;
    r32      Time0 = 0.0f;
    r32      Time1 = 0.0f;
    vec3     Center0 = {0};
    vec3     Center1 = {0};
};

struct asset_sphere
{
    vec3     Origin;
    r32      Radius;
    material Material;
};

struct asset_dynamic_sphere
{
    // center of the sphere at Time = 0
    r32      Radius;
    material Material;
    // If the sphere is moving, then these two times are different
    r32      Time0;
    r32      Time1;
    // Position at Time = Time0
    vec3     Center0;
    // Position at Time = Time
    vec3     Center1;
};

struct asset
{
    asset_id Id;
    union
    {
        asset_sphere         Sphere;
        asset_dynamic_sphere DynamicSphere;
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
void CopyAssets(asset_t *Assets, u32 *AssetsCount, asset_registry *AssetRegistry, tag_block_t Block);

inline bool IsValidAsset(asset_t Assets, asset_id Id);

inline vec3 GetSphereCenter(asset_dynamic_sphere *Sphere, r32 Time)
{
    return Sphere->Center0
        + ((Time - Sphere->Time0) / (Sphere->Time1 - Sphere->Time0))*(Sphere->Center1 - Sphere->Center0);
}

#endif //ENGINE_ASSETS_ASSET_H
