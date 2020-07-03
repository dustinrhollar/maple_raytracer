
file_internal vec3 RayColor(ray *Ray, asset *Assets, u32 AssetsCount, u32 Depth)
{
    vec3 Result = {0, 0, 0};
    
    hit_record Record = {};
    if (Depth <= 0)
    {
        Result = {0, 0, 0};
    }
    else if (RayIntersection(&Record, Ray, Assets, AssetsCount, 0.001, MAX_FLOAT))
    {
        ray ScatteredRay = {0};
        vec3 Attentuation;
        
        if (Scatter(&Record, Ray, &ScatteredRay, &Attentuation))
        {
            Result = Attentuation * RayColor(&ScatteredRay, Assets, AssetsCount, Depth - 1);
        }
    }
    else
    {
        vec3 UnitDirection = norm(Ray->Dir);
        r32 t = 0.5f * (UnitDirection.y + 1.0f);
        
        //vec3 White = { 1.0f, 1.0f, 1.0f };
        //vec3 Blue  = {  0.0f, 0.0f, 0.0f };
        
        vec3 White = { 0.5f, 0.5f, 0.5f };
        vec3 Blue  = {  0.5f, 0.7f, 1.0f };
        
        Result = (1.0f - t) * White + t * Blue;
    }
    
    return Result;
}

extern "C" void GameStageEntry(frame_params* FrameParams)
{
    u32 ImageWidth  = FrameParams->TextureWidth;
    u32 ImageHeight = FrameParams->TextureHeight;
    
    u32 SamplesPerPixel = 10;
    u32 MaxDepth = 5;
    
    u32 MaxXIdx = FrameParams->PixelXOffset + FrameParams->ScanWidth - 1;
    
    vec3 *Image = (vec3*)FrameParams->TextureBackbuffer;
    u32 Idx = 0;
    for (i32 j = 0; j < FrameParams->ScanHeight; ++j)
    {
        i32 RemapY = (FrameParams->ScanHeight - 1) - j;
        //vec3 *RowOffset = Image + (RemapY * ImageWidth);
        vec3 *RowOffset = Image + (j * FrameParams->ScanWidth);
        
        for (i32 i = 0; i < FrameParams->ScanWidth; ++i)
        {
            u32 RealPixelX = MaxXIdx - i;
            u32 RealPixelY = FrameParams->PixelYOffset + j;
            
            vec3 Color = {0.0f, 0.0f, 0.0f};
            for (u32 s = 0; s < SamplesPerPixel; ++s)
            {
                
                r32 U = float(RealPixelX + Random()) / float(ImageWidth - 1);
                r32 V = float(RealPixelY + Random()) / float(ImageHeight - 1);
                
                ray EyeRay = CameraGetRay(FrameParams->Camera, U, V);
                Color += RayColor(&EyeRay, FrameParams->Assets, FrameParams->AssetsCount, MaxDepth);
            }
            
            r32 Scale = 1.0f / (r32)SamplesPerPixel;
            Color *= Scale;
            
            Color.r = sqrt(Color.r);
            Color.g = sqrt(Color.b);
            Color.b = sqrt(Color.g);
            
            Color.r = Clamp(Color.r, 0.0f, 0.9999f);
            Color.g = Clamp(Color.g, 0.0f, 0.9999f);
            Color.b = Clamp(Color.b, 0.0f, 0.9999f);
            
            Image[Idx++] = Color;
        }
    }
}
