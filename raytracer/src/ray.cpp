
inline void SetFaceNormal(hit_record *Record, ray *Ray)
{
    Record->IsFrontFacing = dot(Ray->Dir, Record->Normal) < 0.0f;
    if (!Record->IsFrontFacing) Record->Normal *= -1.0f;
}

file_internal ray CameraGetRay(camera *Camera, r32 S, r32 T)
{
    ray Result = {};
    
    vec3 Rd = Camera->LensRadius * RandomInUnitDisc();
    vec3 Offset = Camera->U * Rd.x + Camera->V * Rd.y;
    
    Result.Origin = Camera->Origin + Offset;
    Result.Dir    = Camera->LowerLeftCorner + S * Camera->Horizontal + T * Camera->Vertical - Camera->Origin - Offset;
    Result.Time = Random(Camera->Time0, Camera->Time1);
    
    return Result;
}

file_internal vec3 MoveAlongRay(ray *Ray, r32 t)
{
    vec3 Result;
    Result = Ray->Origin + t * Ray->Dir;
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
        else if (Asset.Id.Type == Asset_DynamicSphere)
        {
            if (DynamicSphereIntersection(&TmpRecord, Ray, &Asset.DynamicSphere, Tmin, ClosestHit))
            {
                HitAnything = true;
                ClosestHit = TmpRecord.t;
                *Record = TmpRecord;
            }
        }
    }
    
    return HitAnything;
}
