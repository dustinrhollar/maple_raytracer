#ifndef ENGINE_CAMERA_CAMERA_H
#define ENGINE_CAMERA_CAMERA_H

struct camera
{
    vec3 Origin;
    vec3 LowerLeftCorner;
    vec3 Horizontal;
    vec3 Vertical;
};

void CameraInit(camera *Camera, u32 Width, u32 Height)
{
    r32 AspectRatio = Width / Height;
    
    r32 ViewportHeight = 2.0f;
    r32 ViewportWidth  = AspectRatio * ViewportHeight * 1.5f;
    r32 FocalLength = 1.0f;
    
    Camera->Origin = { 0.0f, 0.0f, 0.0f };
    Camera->Horizontal = { ViewportWidth,           0.0f, 0.0f };
    Camera->Vertical   = {          0.0f, ViewportHeight, 0.0f };
    
    vec3 FocalVec   = { 0.0f, 0.0f, FocalLength};
    Camera->LowerLeftCorner = Camera->Origin - (Camera->Horizontal / 2.0f) - (Camera->Vertical / 2.0f) - FocalVec;
}

#endif //ENGINE_CAMERA_CAMERA_H
