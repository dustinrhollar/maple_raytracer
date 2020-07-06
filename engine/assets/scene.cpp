
void SceneInit(scene *Scene, free_allocator *Allocator, u32 MaxAssets)
{
    Scene->Bvh.Type        = Primitive_None;
    Scene->PrimitivesMax   = MaxAssets;
    Scene->PrimitivesCount = 0;
    Scene->Primitives      = palloc<primitive>(Allocator, MaxAssets);
}

void SceneFree(scene *Scene, free_allocator *Allocator)
{
    pfree<primitive>(Allocator, Scene->Primitives);
    Scene->PrimitivesMax   = 0;
    Scene->PrimitivesCount = 0;
    Scene->Primitives      = NULL;
}

bool SceneAddPrimitive(scene *Scene, primitive_type Type, void *Data)
{
    if (Scene->PrimitivesCount + 1 > Scene->PrimitivesMax)
    {
        mprinte("Scene capacity has been reached!\n");
        return false;
    }
    
    primitive Primitive = {};
    Primitive.Type = Type;
    
    switch (Type)
    {
        case Primitive_Sphere:
        {
            Primitive.Sphere = *((sphere*)Data);
            
            vec3 Tmp = { Primitive.Sphere.Radius, Primitive.Sphere.Radius, Primitive.Sphere.Radius };
            Primitive.BoundingBox.Min = Primitive.Sphere.Origin - Tmp;
            Primitive.BoundingBox.Max = Primitive.Sphere.Origin + Tmp;
            
            Scene->Primitives[Scene->PrimitivesCount++] = Primitive;
        } break;
        
        case Primitive_DynamicSphere:
        {
            Primitive.DynamicSphere = *((dynamic_sphere*)Data);
            
            vec3 Tmp = { 
                Primitive.DynamicSphere.Radius, 
                Primitive.DynamicSphere.Radius, 
                Primitive.DynamicSphere.Radius 
            };
            
            Primitive.BoundingBox.Min
                = GetSphereCenter(&Primitive.DynamicSphere, Primitive.DynamicSphere.Time0) - Tmp;
            Primitive.BoundingBox.Max
                = GetSphereCenter(&Primitive.DynamicSphere, Primitive.DynamicSphere.Time0) + Tmp;
            
            Primitive.DynamicSphere.BoundingBox1.Min 
                = GetSphereCenter(&Primitive.DynamicSphere, Primitive.DynamicSphere.Time1) - Tmp;
            Primitive.DynamicSphere.BoundingBox1.Max 
                = GetSphereCenter(&Primitive.DynamicSphere, Primitive.DynamicSphere.Time1) + Tmp;
            
            Scene->Primitives[Scene->PrimitivesCount++] = Primitive;
        } break;
        
        case Primitive_Bvh:
        {
        } break;
    }
    
    return true;
}

int BoxXCompare(const void *Left, const void *Right)
{
    return BoxCompare(&((primitive*)Left)->BoundingBox, &((primitive*)Right)->BoundingBox, 0);
}

int BoxYCompare(const void *Left, const void *Right)
{
    return BoxCompare(&((primitive*)Left)->BoundingBox, &((primitive*)Right)->BoundingBox, 1);
}

int BoxZCompare(const void *Left, const void *Right)
{
    return BoxCompare(&((primitive*)Left)->BoundingBox, &((primitive*)Right)->BoundingBox, 2);
}


file_internal void ConvertSceneToBVh(scene *Scene, primitive *CurrentNode, u32 Start, u32 End)
{
    i32 Axis = RandomInt(0, 2);
    assert(Axis <= 2);
    
    auto Comparator = (Axis == 0) ? &BoxXCompare : (Axis == 1) ? &BoxYCompare : BoxZCompare;
    
    u32 Span = End - Start;
    if (Span == 1)
    {
        CurrentNode->BvhNode.Left = CurrentNode->BvhNode.Right = &Scene->Primitives[Start];
    }
    else if (Span == 2)
    {
        if (Comparator(&Scene->Primitives[Start], &Scene->Primitives[Start + 1])) 
        {
            CurrentNode->BvhNode.Left  = &Scene->Primitives[Start];
            CurrentNode->BvhNode.Right = &Scene->Primitives[Start + 1];
        }
        else
        {
            CurrentNode->BvhNode.Left  = &Scene->Primitives[Start + 1];
            CurrentNode->BvhNode.Right = &Scene->Primitives[Start];
        }
    }
    else
    {
        if (Scene->PrimitivesCount > Scene->PrimitivesMax)
        {
            mprinte("BVH too large! Exceeding allowed Scene Primitive count!");
        }
        else
        {
            primitive *pLeft  = &Scene->Primitives[Scene->PrimitivesCount++];
            primitive *pRight = &Scene->Primitives[Scene->PrimitivesCount++];
            
            pLeft->Type  = Primitive_Bvh;
            pRight->Type = Primitive_Bvh;
            
            bvh_node *Left  = &pLeft->BvhNode;
            bvh_node *Right = &pRight->BvhNode;
            
            *Left = {};
            *Right = {};
            
            CurrentNode->BvhNode.Left  = pLeft;
            CurrentNode->BvhNode.Right = pRight;
            
            qsort(Scene->Primitives + Start, Span, sizeof(primitive), Comparator);
            
            u32 Mid = Start + Span / 2;
            
            ConvertSceneToBVh(Scene, pLeft, Start, Mid);
            ConvertSceneToBVh(Scene, pRight, Mid, End);
        }
    }
    
    CurrentNode->BoundingBox = GetSurroundingAabb(CurrentNode->BvhNode.Left->BoundingBox, 
                                                  CurrentNode->BvhNode.Right->BoundingBox);
}

file_internal void ConvertSceneToBvh(scene *Scene)
{
    primitive *Root = &Scene->Bvh;
    *Root = {};
    Root->Type = Primitive_Bvh;
    
    ConvertSceneToBVh(Scene, Root, 0, Scene->PrimitivesCount);
}


file_internal void MakeSphere(scene *Registry, vec3 Origin, r32 Radius,  material Material)
{
    sphere Result = {};
    
    Result.Origin   = Origin;
    Result.Radius   = Radius;
    Result.Material = Material;
    
    SceneAddPrimitive(Registry, Primitive_Sphere, &Result);
}

file_internal void MakeDynamicSphere(scene *Registry,
                                     vec3 Center0, vec3 Center1,
                                     r32 Time0, r32 Time1,
                                     r32 Radius,
                                     material Material)
{
    dynamic_sphere Result = {};
    
    Result.Center0  = Center0;
    Result.Center1  = Center1;
    Result.Time0    = Time0;
    Result.Time1    = Time1;
    Result.Radius   = Radius;
    Result.Material = Material;
    
    SceneAddPrimitive(Registry, Primitive_DynamicSphere, &Result);
}

file_internal material MakeLambertian(vec3 Albedo)
{
    material Result = {};
    
    Result.Type = Material_Lambertian;
    Result.Lambertian.Albedo = Albedo;
    
    return Result;
}

file_internal material MakeMetal(vec3 Albedo, r32 Fuzz)
{
    material Result = {};
    
    Result.Type = Material_Metal;
    Result.Metal.Albedo = Albedo;
    Result.Metal.Fuzz = Fuzz;
    
    return Result;
}

file_internal material MakeDielectric(r32 IoR)
{
    material Result = {};
    
    Result.Type = Material_Dielectric;
    Result.Dielectric.IndexOfRefraction = IoR;
    
    return Result;
}

file_internal void BuildRandomScene(scene *Scene)
{
    material GroundMaterial = MakeLambertian({ 0.5f, 0.5f, 0.5f });
    MakeSphere(Scene, { 0, -1000, 0 }, 1000, GroundMaterial);
    
    for (i32 a = -11; a < 11; ++a)
    {
        for (i32 b = -11; b < 11; ++b)
        {
            r32 ChooseMat = Random();
            vec3 Center = { a * 0.9f * Random(), 0.2f, b + 0.9f * Random() };
            
            vec3 Point = { 4, 0.2f, 0 };
            if (mag(Center - Point) > 0.9f)
            {
                if (ChooseMat < 0.8f)
                {
                    vec3 Albedo = RandomVec3() * RandomVec3();
                    material Mat = MakeLambertian(Albedo);
#if 1
                    MakeSphere(Scene, Center, 0.2f, Mat);
#else
                    // dynamic spheres for motion blur
                    vec3 RandY = { 0, Random(0, 0.5f), 0 };
                    vec3 Center2 = Center + RandY;
                    MakeDynamicSphere(Scene, Center, Center2, 0.0f, 1.0f, 0.2f, Mat);
#endif
                }
                else if (ChooseMat < 0.95f)
                {
                    vec3 Albedo = {
                        0.5f * (1 + Random()),
                        0.5f * (1 + Random()),
                        0.5f * (1 + Random())
                    };
                    
                    r32 Fuzz = Random(0.0f, 0.5f);
                    material Mat = MakeMetal(Albedo, Fuzz);
                    MakeSphere(Scene, Center, 0.2f, Mat);
                }
                else
                {
                    material Mat = MakeDielectric(1.5f);
                    MakeSphere(Scene, Center, 0.2f, Mat);
                }
            }
        }
    }
    
    material Mat1 = MakeDielectric(1.5f);
    MakeSphere(Scene, { 0, 1, 0 }, 1.0f, Mat1);
    
    material Mat2 = MakeLambertian({ 0.4, 0.2, 0.1 });
    MakeSphere(Scene, { -4, 1, 0 }, 1.0f, Mat2);
    
    material Mat3 = MakeMetal({ 0.7, 0.6, 0.5 }, 0.0f);
    MakeSphere(Scene, { 4, 1, 0 }, 1.0f, Mat3);
    
    //ConvertSceneToBvh(Scene);
}

void BuildSimpleScene(scene *Scene)
{
    r32 R = cosf(PI / 4.0f);
    
    material Mat1 = MakeLambertian({ 0.0f, 0.0f, 1.0f });
    MakeSphere(Scene, { -R, 0.0f, -1.0f }, R, Mat1);
    
    Mat1 = MakeLambertian({ 1.0f, 0.0f, 0.0f });
    MakeSphere(Scene, { R, 0.0f, -1.0f }, R, Mat1);
    
    //ConvertSceneToBvh(Scene);
}