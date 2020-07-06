#ifndef ENGINE_ASSETS_SCENE_H
#define ENGINE_ASSETS_SCENE_H

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

struct dynamic_sphere
{
    r32      Radius;
    material Material;
    r32      Time0;
    r32      Time1;
    vec3     Center0;
    vec3     Center1;
    aabb     BoundingBox1;
};

struct sphere
{
    vec3     Origin;
    r32      Radius;
    material Material;
};

struct bvh_node
{
    struct primitive *Left;
    struct primitive *Right;
};

enum primitive_type
{
    Primitive_None,
    Primitive_Sphere,
    Primitive_DynamicSphere,
    Primitive_Bvh,
};

struct primitive
{
    primitive_type Type;
    aabb           BoundingBox;
    
    union
    {
        sphere         Sphere;
        dynamic_sphere DynamicSphere;
        bvh_node       BvhNode;
    };
};

struct scene
{
    primitive  Bvh; // bvh tree, if one has been made
    primitive *Primitives;
    u32        PrimitivesCount;
    u32        PrimitivesMax;
    
    // TODO(Dustin): Debug mode only?
    mstring    Name;
};

void SceneInit(scene *Scene, free_allocator *Allocator, u32 MaxAssets);
void SceneFree(scene *Scene, free_allocator *Allocator);
bool SceneAddPrimitive(scene *Scene, primitive_type Type, void *Data);

inline vec3 GetSphereCenter(dynamic_sphere *Sphere, r32 Time)
{
    return Sphere->Center0
        + ((Time - Sphere->Time0) / (Sphere->Time1 - Sphere->Time0))*(Sphere->Center1 - Sphere->Center0);
}

file_internal void MakeSphere(scene *Registry, vec3 Origin, r32 Radius,  material Material);
file_internal void MakeDynamicSphere(scene *Registry,
                                     vec3 Center0, vec3 Center1,
                                     r32 Time0, r32 Time1,
                                     r32 Radius,
                                     material Material);
file_internal material MakeLambertian(vec3 Albedo);
file_internal material MakeMetal(vec3 Albedo, r32 Fuzz);
file_internal material MakeDielectric(r32 IoR);
void BuildRandomScene(scene *Scene);

#endif //ENGINE_ASSETS_SCENE_H
