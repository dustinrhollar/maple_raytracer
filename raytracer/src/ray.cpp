
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

file_internal bool RayPrimitiveIntersection(hit_record *Record,
                                            ray        *Ray,
                                            primitive  *Primitive,
                                            r32 Tmin, r32 *Tmax)
{
    hit_record TmpRecord = {};
    bool HitAnything = false;
    
    if (Primitive->Type == Primitive_Sphere)
    {
        if (SphereIntersection(&TmpRecord, Ray, &Primitive->Sphere, Tmin, *Tmax))
        {
            HitAnything = true;
            *Tmax = TmpRecord.t;
            *Record = TmpRecord;
        }
    }
    else if (Primitive->Type == Primitive_DynamicSphere)
    {
        if (DynamicSphereIntersection(&TmpRecord, Ray, &Primitive->DynamicSphere, Tmin, *Tmax))
        {
            HitAnything = true;
            *Tmax = TmpRecord.t;
            *Record = TmpRecord;
        }
    }
    else if (Primitive->Type == Primitive_Bvh)
    {
        if (BvhNodeIntersection(&TmpRecord, Ray, &Primitive->BvhNode, &Primitive->BoundingBox, Tmin, *Tmax))
        {
            HitAnything = true;
            *Tmax = TmpRecord.t;
            *Record = TmpRecord;
        }
    }
    
    return HitAnything;
}

file_internal bool RaySceneIntersection(hit_record *Record, ray *Ray, scene *Scene,
                                        r32 Tmin, r32 Tmax)
{
    hit_record TmpRecord;
    bool HitAnything = false;
    r32 ClosestHit = Tmax;
    
    if (Scene->Bvh.Type != Primitive_None)
    {
        if (RayPrimitiveIntersection(Record, Ray, &Scene->Bvh, Tmin, &ClosestHit)) 
            HitAnything = true;
    }
    else
    {
        for (u32 i = 0; i < Scene->PrimitivesCount; ++i)
        {
            primitive Primitive = Scene->Primitives[i];
            if (RayPrimitiveIntersection(Record, Ray, &Primitive, Tmin, &ClosestHit)) 
                HitAnything = true;
        }
    }
    
    return HitAnything;
}
