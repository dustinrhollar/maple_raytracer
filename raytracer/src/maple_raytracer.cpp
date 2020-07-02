
#define MAX_FLOAT 3.402823466e38

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
    material Material;
    
    bool IsFrontFacing;
};

// Ray function
file_internal vec3 MoveAlongRay(ray *Ray, r32 t);
file_internal vec3 RayColor(ray *Ray, asset *Assets, u32 AssetsCount, u32 Depth);
file_internal bool Scatter(hit_record *Record, ray *Ray, ray *ScatteredRay, vec3 *Attentuation);

// Hit Record Functions
inline void SetFaceNormal(hit_record *Record, ray *Ray);

// Intersection Functions
file_internal bool SphereIntersection(hit_record *Record, ray *Ray, asset_sphere *Sphere, r32 Tmin, r32 Tmax);

ray CameraGetRay(camera *Camera, r32 S, r32 T)
{
    ray Result = {};
    
    vec3 Rd = Camera->LensRadius * RandomInUnitDisc();
    vec3 Offset = Camera->U * Rd.x + Camera->V * Rd.y;
    
    Result.Origin = Camera->Origin + Offset;
    Result.Dir    = Camera->LowerLeftCorner + S * Camera->Horizontal + T * Camera->Vertical - Camera->Origin - Offset;
    
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
            Record->Material = Sphere->Material;
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
                Record->Material = Sphere->Material;
                SetFaceNormal(Record, Ray);
                Result = true;
            }
        }
    }
    
    return Result;
}

file_internal bool Scatter(hit_record *Record, ray *Ray, ray *ScatteredRay, vec3 *Attentuation)
{
    bool Result = false;
    
    switch (Record->Material.Type)
    {
        case Material_Lambertian:
        {
            vec3 ScatterDir = Record->Normal + RandomUnitVector();
            
            ScatteredRay->Origin = Record->Point;
            ScatteredRay->Dir = ScatterDir;
            *Attentuation = Record->Material.Lambertian.Albedo;
            Result = true;
        } break;
        
        case Material_Metal:
        {
            vec3 Reflected = Reflect(norm(Ray->Dir), Record->Normal);
            
            ScatteredRay->Origin = Record->Point;
            ScatteredRay->Dir    = Reflected + Record->Material.Metal.Fuzz * RandomInUnitSphere();
            *Attentuation = Record->Material.Metal.Albedo;
            Result = (dot(ScatteredRay->Dir, Record->Normal) > 0.0f);
        } break;
        
        case Material_Dielectric:
        {
            *Attentuation = { 1, 1, 1 };
            
            r32 Ratio;
            if (Record->IsFrontFacing) Ratio = 1.0f / Record->Material.Dielectric.IndexOfRefraction;
            else                      Ratio = Record->Material.Dielectric.IndexOfRefraction;
            
            vec3 UnitDir = norm(Ray->Dir);
            r32 CosTheta = fmin(dot(-1 * UnitDir, Record->Normal), 1.0f);
            r32 SinTheta = sqrt(1.0f - CosTheta * CosTheta);
            
            if (Ratio * SinTheta > 1.0f)
            {
                vec3 Reflected = Reflect(UnitDir, Record->Normal);
                ScatteredRay->Origin = Record->Point;
                ScatteredRay->Dir = Reflected;
            }
            else
            {
                r32 ReflectProb = Schlick(CosTheta, Ratio);
                if (Random() < ReflectProb)
                {
                    vec3 Reflected = Reflect(UnitDir, Record->Normal);
                    ScatteredRay->Origin = Record->Point;
                    ScatteredRay->Dir = Reflected;
                }
                else
                {
                    ScatteredRay->Origin = Record->Point;
                    ScatteredRay->Dir = Refract(UnitDir, Record->Normal, Ratio);
                }
            }
            
            Result = true;
        };
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

file_internal vec3 RayColor(ray *Ray, asset *Assets, u32 AssetsCount, u32 Depth)
{
    vec3 Result = {0, 0, 0};
    
    hit_record Record = {0};
    if (Depth <= 0)
    {
        Result = {0, 0, 0};
    }
    else if (RayIntersection(&Record, Ray, Assets, AssetsCount, 0.001, MAX_FLOAT))
    {
        ray ScatteredRay = {0};
        vec3 Attentuation;
        
        if (Scatter(&Record, Ray, &ScatteredRay, &Attentuation))
        {
            Result = Attentuation * RayColor(&ScatteredRay, Assets, AssetsCount, Depth - 1);
        }
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
    u32 MaxDepth = 50;
    
    u32 MaxXIdx = FrameParams->PixelXOffset + FrameParams->ScanWidth - 1;
    
    vec3 *Image = (vec3*)FrameParams->TextureBackbuffer;
    u32 Idx = 0;
    for (i32 j = 0; j < FrameParams->ScanHeight; ++j)
    {
        i32 RemapY = (FrameParams->ScanHeight - 1) - j;
        //vec3 *RowOffset = Image + (RemapY * ImageWidth);
        vec3 *RowOffset = Image + (j * FrameParams->ScanWidth);
        
        for (i32 i = 0; i < FrameParams->ScanWidth; ++i)
        {
            u32 RealPixelX = MaxXIdx - i;
            u32 RealPixelY = FrameParams->PixelYOffset + j;
            
            vec3 Color = {0.0f, 0.0f, 0.0f};
            for (u32 s = 0; s < SamplesPerPixel; ++s)
            {
                
                r32 U = float(RealPixelX + Random()) / float(ImageWidth - 1);
                r32 V = float(RealPixelY + Random()) / float(ImageHeight - 1);
                
                ray EyeRay = CameraGetRay(FrameParams->Camera, U, V);
                Color += RayColor(&EyeRay, FrameParams->Assets, FrameParams->AssetsCount, MaxDepth);
            }
            
            r32 Scale = 1.0f / (r32)SamplesPerPixel;
            Color *= Scale;
            
            Color.r = sqrt(Color.r);
            Color.g = sqrt(Color.b);
            Color.b = sqrt(Color.g);
            
            Color.r = Clamp(Color.r, 0.0f, 0.9999f);
            Color.g = Clamp(Color.g, 0.0f, 0.9999f);
            Color.b = Clamp(Color.b, 0.0f, 0.9999f);
            
            Image[Idx++] = Color;
        }
    }
}
