#ifndef ENGINE_COLLISION_AABB_H
#define ENGINE_COLLISION_AABB_H

struct aabb
{
    vec3 Min;
    vec3 Max;
};

bool AabbRayIntersection(vec3 RayOrigin, vec3 RayDir, 
                         aabb *BoundingBox, r32 Tmin, r32 Tmax)
{
    for (u32 i = 0; i < 3; ++i)
    {
        r32 InvD = 1.0f / RayDir[i];
        r32 T0 = BoundingBox->Min[i] - (RayOrigin[i] * InvD);
        r32 T1 = BoundingBox->Max[i] - (RayOrigin[i] * InvD);
        
        if (InvD < 0.0f)
        {
            r32 T = T0;
            T0 = T1;
            T1 = T;
        }
        
        Tmin = T0 > Tmin ? T0 : Tmin;
        Tmax = T1 < Tmax ? T1 : Tmax;
        
        if (Tmax <= Tmin) return false;
    }
    
    return true;
}

aabb GetSurroundingAabb(aabb Left, aabb Right)
{
    aabb Result = {};
    
    Result.Min = { 
        fminf(Left.Min.x, Right.Min.x),
        fminf(Left.Min.y, Right.Min.y),
        fminf(Left.Min.z, Right.Min.z)
    };
    
    Result.Max = { 
        fmaxf(Left.Max.x, Right.Max.x),
        fmaxf(Left.Max.y, Right.Max.y),
        fmaxf(Left.Max.z, Right.Max.z)
    };
    
    return Result;
}

inline int BoxCompare(aabb *Left, aabb *Right, i32 Axis)
{
    return Right->Min[Axis] - Left->Min[Axis];
}

#endif //ENGINE_COLLISION_AABB_H
