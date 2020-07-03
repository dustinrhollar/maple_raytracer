
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
            ScatteredRay->Time = Ray->Time;
            *Attentuation = Record->Material.Lambertian.Albedo;
            Result = true;
        } break;
        
        case Material_Metal:
        {
            vec3 Reflected = Reflect(norm(Ray->Dir), Record->Normal);
            
            ScatteredRay->Origin = Record->Point;
            ScatteredRay->Dir    = Reflected + Record->Material.Metal.Fuzz * RandomInUnitSphere();
            ScatteredRay->Time = Ray->Time;
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
                ScatteredRay->Time = Ray->Time;
            }
            else
            {
                r32 ReflectProb = Schlick(CosTheta, Ratio);
                if (Random() < ReflectProb)
                {
                    vec3 Reflected = Reflect(UnitDir, Record->Normal);
                    ScatteredRay->Origin = Record->Point;
                    ScatteredRay->Dir = Reflected;
                    ScatteredRay->Time = Ray->Time;
                }
                else
                {
                    ScatteredRay->Origin = Record->Point;
                    ScatteredRay->Dir = Refract(UnitDir, Record->Normal, Ratio);
                    ScatteredRay->Time = Ray->Time;
                }
            }
            
            Result = true;
        };
    }
    
    return Result;
}
