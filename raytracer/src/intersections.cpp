
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

file_internal bool DynamicSphereIntersection(hit_record           *Record,
                                             ray                  *Ray,
                                             asset_dynamic_sphere *Sphere,
                                             r32                   Tmin,
                                             r32                   Tmax)
{
    bool Result = false;
    vec3 SphereCenter = GetSphereCenter(Sphere, Ray->Time);
    
    vec3 Dir = Ray->Origin - SphereCenter;
    
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
            Record->Normal = (Record->Point - SphereCenter) / Sphere->Radius;
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
                Record->Normal = (Record->Point - SphereCenter) / Sphere->Radius;
                Record->Material = Sphere->Material;
                SetFaceNormal(Record, Ray);
                Result = true;
            }
        }
    }
    
    return Result;
}
