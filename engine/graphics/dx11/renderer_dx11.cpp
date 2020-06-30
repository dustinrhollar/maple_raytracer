
void RendererInit(renderer_t *Renderer, free_allocator *Allocator, resource_registry *Registry,
                  window_t Window, u32 Width, u32 Height, u32 RefreshRate)
{
    renderer_t pRenderer = (renderer_t)FreeListAllocatorAlloc(Allocator, sizeof(renderer));
    
    device_create_info DeviceInfo = {};
    DeviceInfo.Width       = 0;
    DeviceInfo.Height      = 0;
    DeviceInfo.RefreshRate = RefreshRate;
    DeviceInfo.Window      = Window;
    DeviceInfo.SampleCount = 1;
    pRenderer->Device = CreateResource(Registry, Resource_Device, &DeviceInfo);
    
    render_target_create_info RenderTargetInfo = {};
    RenderTargetInfo.Device = pRenderer->Device;
    pRenderer->RenderTarget = CreateResource(Registry, Resource_RenderTarget, &RenderTargetInfo);
    
    file_t VertFile = PlatformLoadFile("data/shaders/quad_vert.cso");
    file_t FragFile = PlatformLoadFile("data/shaders/quad_frag.cso");
    
    pipeline_create_info PipelineInfo = {};
    PipelineInfo.Device              = pRenderer->Device;
    PipelineInfo.VertexData          = GetFileBuffer(VertFile);
    PipelineInfo.PixelData           = GetFileBuffer(FragFile);
    PipelineInfo.VertexDataSize      = PlatformGetFileSize(VertFile);
    PipelineInfo.PixelDataSize       = PlatformGetFileSize(FragFile);
    PipelineInfo.PipelineLayout      = NULL;
    PipelineInfo.PipelineLayoutCount = 0;
    pRenderer->Pipeline = CreateResource(Registry, Resource_Pipeline, &PipelineInfo);
    
    //~ Create the sample texture that the ray traced image is copied into, but do not upload the data
    
    //
    
    pRenderer->TextureWidth  = 1920;
    pRenderer->TextureHeight = 1080;
    
    texture2d_create_info TextureInfo = {};
    TextureInfo.Device              = pRenderer->Device;
    TextureInfo.Width               = pRenderer->TextureWidth;
    TextureInfo.Height              = pRenderer->TextureHeight;
    TextureInfo.Usage               = BufferUsage_Dynamic;
    TextureInfo.CpuAccessFlags      = BufferCpuAccess_Write;
    TextureInfo.BindFlags           = BufferBind_ShaderResource;
    TextureInfo.MiscFlags           = BufferMisc_None;
    TextureInfo.StructureByteStride = 0;
    TextureInfo.Format              = PipelineFormat_R32G32B32_FLOAT;
    pRenderer->RaytracedTexture = CreateResource(Registry, Resource_Texture2D, &TextureInfo);
    
    // Uplload default texture
    u32 BytesPerPixel = sizeof(r32) * 3;
    pRenderer->TextureSize = pRenderer->TextureWidth * pRenderer->TextureHeight * BytesPerPixel;
    pRenderer->TextureBackbuffer = FreeListAllocatorAlloc(Allocator, pRenderer->TextureSize);
    memset(pRenderer->TextureBackbuffer, 0, pRenderer->TextureSize);
    
    *Renderer = pRenderer;
}

void RendererShutdown(renderer_t *Renderer, free_allocator *Allocator)
{
    FreeListAllocatorAllocFree(Allocator, (*Renderer)->TextureBackbuffer);
    (*Renderer)->TextureBackbuffer = NULL;
    
    FreeListAllocatorAllocFree(Allocator, *Renderer);
    *Renderer = NULL;
}

void RendererResize(renderer_t Renderer, resource_registry *Registry)
{
    ID3D11Device *Device = Registry->Resources[Renderer->Device.Index]->Device.Handle;
    ID3D11DeviceContext *DeviceContext = Registry->Resources[Renderer->Device.Index]->Device.Context;
    IDXGISwapChain *Swapchain = Registry->Resources[Renderer->Device.Index]->Device.Swapchain;
    ID3D11RenderTargetView **RenderTarget = &Registry->Resources[Renderer->RenderTarget.Index]->RenderTarget.Handle;
    
    DeviceContext->OMSetRenderTargets(0, 0, 0);
    (*RenderTarget)->Release();
    
    HRESULT hr = Swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr))
    {
        mprinte("Failed to preserve swapchain format during resize!\n");
        return;
    }
    
    ID3D11Texture2D* pBuffer;
    hr = Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &pBuffer );
    if (FAILED(hr))
    {
        mprinte("Failed to retrieve the texture for the RenderTarget View!\n");
        return;
    }
    
    hr = Device->CreateRenderTargetView(pBuffer, NULL, RenderTarget);
    if (FAILED(hr))
    {
        mprinte("Failed to create the RenderTarget View!\n");
        return;
    }
    pBuffer->Release();
}

void RendererUi()
{
    
}

file_internal void UpdateTextureResource(renderer_t Renderer, frame_params *FrameParams)
{
    ID3D11DeviceContext *DeviceContext = FrameParams->Resources[Renderer->Device.Index].Device.Context;
    ID3D11Texture2D     *Texture = FrameParams->Resources[Renderer->RaytracedTexture.Index].Texture.Handle;
    
    D3D11_MAPPED_SUBRESOURCE TextureResource;
    HRESULT hr = DeviceContext->Map(Texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &TextureResource);
    
    if (FAILED(hr))
    {
        mprinte("Failed to map the texture resource at setup!\n");
    }
    else
    {
        void* Backbuffer = TextureResource.pData;
        memcpy(Backbuffer, Renderer->TextureBackbuffer, TextureResource.DepthPitch);
        
        DeviceContext->Unmap(Texture, 0);
    }
}

void RendererEntry(renderer_t Renderer, frame_params *FrameParams)
{
    UpdateTextureResource(Renderer, FrameParams);
    
    ID3D11DeviceContext *DeviceContext =
        FrameParams->Resources[Renderer->Device.Index].Device.Context;
    IDXGISwapChain *Swapchain =
        FrameParams->Resources[Renderer->Device.Index].Device.Swapchain;
    ID3D11RenderTargetView *RenderTarget =
        FrameParams->Resources[Renderer->RenderTarget.Index].RenderTarget.Handle;
    
    ID3D11InputLayout  *Layout       = FrameParams->Resources[Renderer->Pipeline.Index].Pipeline.Layout;
    ID3D11VertexShader *VertexShader = FrameParams->Resources[Renderer->Pipeline.Index].Pipeline.VertexShader;
    ID3D11PixelShader  *PixelShader  = FrameParams->Resources[Renderer->Pipeline.Index].Pipeline.PixelShader;
    
    DeviceContext->OMSetRenderTargets(1, &RenderTarget, NULL);
    
    // Set the viewport
    D3D11_VIEWPORT Viewport;
    ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));
    
    u32 Width, Height;
    PlatformGetClientWindowDimensions(&Width, &Height);
    Viewport.TopLeftX = 0;
    Viewport.TopLeftY = 0;
    Viewport.Width    = Width;
    Viewport.Height   = Height;
    DeviceContext->RSSetViewports(1, &Viewport);
    
    vec4 ClearColor = { 0.0f, 0.2f, 0.4f, 1.0f };
    DeviceContext->ClearRenderTargetView(RenderTarget, ClearColor.data);
    
    DeviceContext->IASetInputLayout(Layout);
    DeviceContext->VSSetShader(VertexShader, 0, 0);
    DeviceContext->PSSetShader(PixelShader, 0, 0);
    
    resource_texture RayTracedTexture = FrameParams->Resources[Renderer->RaytracedTexture.Index].Texture;
    DeviceContext->PSSetShaderResources(0, 1, &RayTracedTexture.View);
    DeviceContext->PSSetSamplers(0, 1, &RayTracedTexture.Sampler);
    
    // select which primtive type we are using
    DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    
    //DeviceContext->DrawIndexed(Renderer->IndexCount, 0, 0);
    DeviceContext->Draw(4, 0);
    
    // Defer updates to the next frame
    for (u32 i = 0; i < FrameParams->RenderCommandsCount; ++i)
    {
        render_command RenderCmd = FrameParams->RenderCommands[i];
        
        switch (RenderCmd.Type)
        {
            case RenderCmd_DrawDevUi:
            {
                RenderCmd.DrawDevUi.Callback(FrameParams);
            } break;
        }
    }
    
    Swapchain->Present(0, 0);
}
