
#define MAX_FLOAT 3.402823466e38
#define PI 3.1415926535897932385

#define DegreesToRadian(d) (((d) * PI) / 180.0f)

struct ray
{
    vec3 Origin;
    vec3 Dir;
};

struct hit_record
{
    vec3 Point;
    vec3 Normal;
    r32  t;
    
    bool IsFrontFacing;
};

// Ray function
file_internal vec3 MoveAlongRay(ray *Ray, r32 t);
file_internal vec3 RayColor(ray *Ray, asset *Assets, u32 AssetsCount);

// Hit Record Functions
inline void SetFaceNormal(hit_record *Record, ray *Ray);

// Intersection Functions
file_internal bool SphereIntersection(hit_record *Record, ray *Ray, asset_sphere *Sphere, r32 Tmin, r32 Tmax);

inline r32 Random()
{
    return rand() / (RAND_MAX + 1.0f);
}

inline r32 Random(r32 Min, r32 Max)
{
    return Min + (Max - Min) * Random();
}

inline r32 Clamp(r32 Val, r32 Min, r32 Max)
{
    if (Val < Min) return Min;
    if (Val > Max) return Max;
    return Val;
}

ray CameraGetRay(camera *Camera, r32 U, r32 V)
{
    ray Result = {};
    
    Result.Origin = Camera->Origin;
    Result.Dir    = Camera->LowerLeftCorner + U * Camera->Horizontal + V * Camera->Vertical - Camera->Origin;
    
    return Result;
}

file_internal vec3 MoveAlongRay(ray *Ray, r32 t)
{
    vec3 Result;
    Result = Ray->Origin + t * Ray->Dir;
    return Result;
}

inline void SetFaceNormal(hit_record *Record, ray *Ray)
{
    Record->IsFrontFacing = dot(Ray->Dir, Record->Normal) < 0.0f;
    if (!Record->IsFrontFacing) Record->Normal *= -1.0f;
}

file_internal bool SphereIntersection(hit_record *Record, ray *Ray, asset_sphere *Sphere, r32 Tmin, r32 Tmax)
{
    bool Result = false;
    
    vec3 Dir = Ray->Origin - Sphere->Origin;
    
    r32 A = mag_sq(Ray->Dir);
    r32 B = dot(Dir, Ray->Dir);
    r32 C = mag_sq(Dir) - Sphere->Radius * Sphere->Radius;
    
    r32 Discriminant = (B * B) - (A * C);
    if (Discriminant > 0)
    {
        r32 Root = sqrt(Discriminant);
        r32 Tmp = (-B - Root) / (A);
        
        if (Tmp < Tmax && Tmp > Tmin)
        {
            Record->t = Tmp;
            Record->Point = MoveAlongRay(Ray, Record->t);
            Record->Normal = (Record->Point - Sphere->Origin) / Sphere->Radius;
            SetFaceNormal(Record, Ray);
            Result = true;
        }
        else
        {
            Tmp = (-B + Root) / (A);
            if (Tmp < Tmax && Tmp > Tmin)
            {
                Record->t = Tmp;
                Record->Point = MoveAlongRay(Ray, Record->t);
                Record->Normal = (Record->Point - Sphere->Origin) / Sphere->Radius;
                SetFaceNormal(Record, Ray);
                Result = true;
            }
        }
    }
    
    return Result;
}

file_internal bool RayIntersection(hit_record *Record, ray *Ray, asset *Assets, u32 AssetsCount,
                                   r32 Tmin, r32 Tmax)
{
    hit_record TmpRecord;
    bool HitAnything = false;
    r32 ClosestHit = Tmax;
    
    for (u32 i = 0; i < AssetsCount; ++i)
    {
        TmpRecord = {};
        
        asset Asset = Assets[i];
        if (Asset.Id.Type == Asset_Sphere)
        {
            if (SphereIntersection(&TmpRecord, Ray, &Asset.Sphere, Tmin, ClosestHit))
            {
                HitAnything = true;
                ClosestHit = TmpRecord.t;
                *Record = TmpRecord;
            }
        }
    }
    
    return HitAnything;
}

file_internal vec3 RayColor(ray *Ray, asset *Assets, u32 AssetsCount)
{
    vec3 Result = {};
    
    hit_record Record = {0};
    if (RayIntersection(&Record, Ray, Assets, AssetsCount, 0, MAX_FLOAT))
    {
        vec3 Color = { 1.0f, 1.0f, 1.0f };
        return 0.5f * (Record.Normal + Color);
    }
    else
    {
        vec3 UnitDirection = norm(Ray->Dir);
        r32 t = 0.5f * (UnitDirection.y + 1.0f);
        
        vec3 White = { 1.0f, 1.0f, 1.0f };
        
        vec3 Blue  = {  0.5f, 0.7f, 1.0f };
        //vec3 Blue  = {  0.9f, 0.7f, 0.4f };
        
        Result = (1.0f - t) * White + t * Blue;
    }
    
    return Result;
}

extern "C" void GameStageEntry(frame_params* FrameParams)
{
    u32 ImageWidth  = FrameParams->TextureWidth;
    u32 ImageHeight = FrameParams->TextureHeight;
    
    u32 SamplesPerPixel = 100;
    
    vec3 *Image = (vec3*)FrameParams->TextureBackbuffer;
    u32 Idx = 0;
    for (i32 j = FrameParams->ScanHeight - 1; j >= 0; --j)
    {
        i32 RemapY = (FrameParams->ScanHeight - 1) - j;
        vec3 *RowOffset = Image + (RemapY * ImageWidth);
        
        for (i32 i = 0; i < FrameParams->ScanWidth; ++i)
        {
            u32 RealPixelX = FrameParams->PixelXOffset + i;
            u32 RealPixelY = FrameParams->PixelYOffset + j;
            
            vec3 Color = {0.0f, 0.0f, 0.0f};
            for (u32 s = 0; s < SamplesPerPixel; ++s)
            {
                
                r32 U = float(RealPixelX + Random()) / float(ImageWidth - 1);
                r32 V = float(RealPixelY + Random()) / float(ImageHeight - 1);
                
                ray EyeRay = CameraGetRay(FrameParams->Camera, U, V);
                Color += RayColor(&EyeRay, FrameParams->Assets, FrameParams->AssetsCount);
            }
            
            r32 Scale = 1.0f / (r32)SamplesPerPixel;
            Color *= Scale;
            
            Color.r = Clamp(Color.r, 0.0f, 0.9999f);
            Color.g = Clamp(Color.g, 0.0f, 0.9999f);
            Color.b = Clamp(Color.b, 0.0f, 0.9999f);
            
            RowOffset[i] = Color;
        }
    }
}
