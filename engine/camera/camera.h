#ifndef ENGINE_CAMERA_CAMERA_H
#define ENGINE_CAMERA_CAMERA_H

struct camera
{
    vec3 Origin;
    vec3 LowerLeftCorner;
    vec3 Horizontal;
    vec3 Vertical;
    
    vec3 U, V, W;
    r32 LensRadius;
};

void CameraInit(camera *Camera,
                vec3 LookFrom, vec3 LookAt, vec3 Up,
                r32 vFov, r32 AspectRatio, r32 Aperture, r32 FocusDist)
{
    r32 Theta = DegreesToRadians(vFov);
    r32 H = tanf(Theta / 2.0f);
    
    r32 ViewportHeight = 2.0f * H;
    r32 ViewportWidth  = AspectRatio * ViewportHeight;
    r32 FocalLength = 1.0f;
    
    Camera->W = norm(LookFrom - LookAt);
    Camera->U = norm(cross(Up, Camera->W));
    Camera->V = cross(Camera->W, Camera->U);
    
    Camera->Origin = LookFrom;
    Camera->Horizontal = FocusDist * ViewportWidth * Camera->U;
    Camera->Vertical   = FocusDist * ViewportHeight * Camera->V;
    
    Camera->LowerLeftCorner
        = Camera->Origin
        - (Camera->Horizontal / 2.0f)
        - (Camera->Vertical / 2.0f)
        - FocusDist *Camera->W;
    
    Camera->LensRadius = Aperture / 2.0f;
}

#endif //ENGINE_CAMERA_CAMERA_H
