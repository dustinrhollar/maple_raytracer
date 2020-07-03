#ifndef RAYTRACING_SRC_INTERSECTIONS_H
#define RAYTRACING_SRC_INTERSECTIONS_H


file_internal bool SphereIntersection(hit_record   *Record,
                                      ray          *Ray,
                                      asset_sphere *Sphere,
                                      r32           Tmin,
                                      r32           Tmax);

file_internal bool DynamicSphereIntersection(hit_record           *Record,
                                             ray                  *Ray,
                                             asset_dynamic_sphere *Sphere,
                                             r32                   Tmin,
                                             r32                   Tmax);



#endif //RAYTRACING_SRC_INTERSECTIONS_H
