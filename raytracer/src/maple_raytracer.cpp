
struct ray
{
    vec3 Origin;
    vec3 Dir;
};

struct sphere
{
    vec3 Origin;
    r32  Radius;
};

file_internal vec3 MoveAlongRay(ray *Ray, r32 t);
file_internal vec3 RayColor(ray *Ray);
file_internal bool SphereIntersection(sphere *Sphere, ray *Ray);

vec3 MoveAlongRay(ray *Ray, r32 t)
{
    vec3 Result;
    Result = Ray->Origin + t * Ray->Dir;
    return Result;
}

vec3 RayColor(ray *Ray)
{
    vec3 Result = {};
    
    sphere Sphere = {};
    Sphere.Origin = { 0.0f, 0.0f, -1.0f };
    Sphere.Radius = 0.5f;
    
    if (SphereIntersection(&Sphere, Ray))
    {
        Result = { 1.0f, 0.0f, 0.0f };
    }
    else
    {
        vec3 UnitDirection = norm(Ray->Dir);
        r32 t = 0.5f * UnitDirection.y + 1.0f;
        
        vec3 White = { 1.0f, 1.0f, 1.0f };
        vec3 Blue  = {  0.5f, 0.7f, 1.0f };
        
        Result = (1.0f - t) * White + t * Blue;
    }
    
    return Result;
}

bool SphereIntersection(sphere *Sphere, ray *Ray)
{
    vec3 Dir = Ray->Origin - Sphere->Origin;
    r32 A = dot(Ray->Dir, Ray->Dir);
    r32 B = 2.0f * dot(Dir, Ray->Dir);
    r32 C = dot(Dir, Dir) - Sphere->Radius * Sphere->Radius;
    r32 Discriminant = B * B - 4 * A * C;
    return Discriminant > 0;
}

extern "C" void GameStageEntry(frame_params* FrameParams)
{
    u32 ImageWidth  = FrameParams->TextureWidth;
    u32 ImageHeight = FrameParams->TextureHeight;
    
    r32 AspectRatio = ImageWidth / ImageHeight;
    
    r32 ViewportHeight = 2.0f;
    r32 ViewportWidth  = AspectRatio * ViewportHeight * 1.5f;
    r32 FocalLength = 1.0f;
    
    vec3 Origin = { 0.0f, 0.0f, 0.0f };
    vec3 Horizontal = { ViewportWidth,           0.0f, 0.0f };
    vec3 Vertical   = {          0.0f, ViewportHeight, 0.0f };
    vec3 FocalVec   = { 0.0f, 0.0f, FocalLength};
    vec3 LowerLeftCorner = Origin - (Horizontal / 2.0f) - (Vertical / 2.0f) - FocalVec;
    
    vec3 *Image = (vec3*)FrameParams->TextureBackbuffer;
    u32 Idx = 0;
    for (i32 j = ImageHeight - 1; j >= 0; --j)
    {
        for (i32 i = 0; i < ImageWidth; ++i)
        {
            r32 U = float(i) / float(ImageWidth - 1);
            r32 V = float(j) / float(ImageHeight - 1);
            
            ray EyeRay = {};
            EyeRay.Origin = Origin;
            EyeRay.Dir    = LowerLeftCorner + U * Horizontal + V * Vertical - Origin;
            
            vec3 Color = RayColor(&EyeRay);
            Image[Idx++] = Color;
        }
    }
}
