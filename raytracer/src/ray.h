#ifndef RAYTRACING_SRC_RAY_H
#define RAYTRACING_SRC_RAY_H

struct ray
{
    vec3 Origin;
    vec3 Dir;
    r32  Time;
};

struct hit_record
{
    material Material;
    vec3     Point;
    vec3     Normal;
    bool     IsFrontFacing;
    r32      t;
};

file_internal bool RayIntersection(hit_record *Record,
                                   ray        *Ray,
                                   asset      *Assets,
                                   u32         AssetsCount,
                                   r32 Tmin, r32 Tmax);

inline void SetFaceNormal(hit_record *Record, ray *Ray);
file_internal ray CameraGetRay(camera *Camera, r32 S, r32 T);
file_internal vec3 MoveAlongRay(ray *Ray, r32 t);

#endif //RAYTRACING_SRC_RAY_H
