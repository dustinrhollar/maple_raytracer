#ifndef RAYTRACING_SRC_MATERIAL_H
#define RAYTRACING_SRC_MATERIAL_H

file_internal bool Scatter(hit_record *Record, ray *Ray, ray *ScatteredRay, vec3 *Attentuation);

#endif //RAYTRACING_SRC_MATERIAL_H
