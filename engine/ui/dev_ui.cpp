
MAPLE_DEV_UI_CALLBACK(RaytraceCallback)
{
    MapleDevGuiNewFrame();
    ImGui_ImplWin32_NewFrame();
    
    ImGui::NewFrame();
    {
        //ImGui::ShowDemoWindow();
        
        ImGui::SetNextWindowSize(ImVec2(100, 100), ImGuiCond_FirstUseEver);
        
        ImGuiWindowFlags window_flags = 0;
        if (!ImGui::Begin("Maple Editor UI", nullptr, window_flags)) // No bool flag to omit the close button
        {
            // Early out if the window is collapsed, as an optimization
            ImGui::End();
            return;
        }
        
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f); // right alifned, keep 180 pixels for the labels
        
        if (ImGui::CollapsingHeader("Render Settings"))
        {
            if (ImGui::InputInt("Scan Width", &RenderSettings->ScanWidth))
            {
            }
            
            if (ImGui::InputInt("Scan Height", &RenderSettings->ScanHeight))
            {
            }
            
            if (ImGui::InputInt("Sampler Per Pixel", &RenderSettings->SamplesPerPixel))
            {
            }
            
            if (ImGui::InputInt("Sample Depth", &RenderSettings->SampleDepth))
            {
            }
        }
        
        if (ImGui::CollapsingHeader("Camera Settings"))
        {
            if (ImGui::InputFloat4("Look From Vector", RenderSettings->LookFrom.data))
            {
                CameraInit(&RenderSettings->Camera,
                           RenderSettings->LookFrom, 
                           RenderSettings->LookAt, 
                           RenderSettings->Up,
                           RenderSettings->vFov, 
                           RenderSettings->AspectRatio, 
                           RenderSettings->Aperture, 
                           RenderSettings->FocusDist,
                           RenderSettings->Time0, 
                           RenderSettings->Time1);
            }
            
            if (ImGui::InputFloat4("Look At Vector", RenderSettings->LookAt.data))
            {
                CameraInit(&RenderSettings->Camera,
                           RenderSettings->LookFrom, 
                           RenderSettings->LookAt, 
                           RenderSettings->Up,
                           RenderSettings->vFov, 
                           RenderSettings->AspectRatio, 
                           RenderSettings->Aperture, 
                           RenderSettings->FocusDist,
                           RenderSettings->Time0, 
                           RenderSettings->Time1);
            }
            
            if (ImGui::InputFloat4("Up Vector", RenderSettings->Up.data))
            {
                CameraInit(&RenderSettings->Camera,
                           RenderSettings->LookFrom, 
                           RenderSettings->LookAt, 
                           RenderSettings->Up,
                           RenderSettings->vFov, 
                           RenderSettings->AspectRatio, 
                           RenderSettings->Aperture, 
                           RenderSettings->FocusDist,
                           RenderSettings->Time0, 
                           RenderSettings->Time1);
            }
            
            if (ImGui::InputFloat("Aspect Ratio", &RenderSettings->AspectRatio))
            {
                CameraInit(&RenderSettings->Camera,
                           RenderSettings->LookFrom, 
                           RenderSettings->LookAt, 
                           RenderSettings->Up,
                           RenderSettings->vFov, 
                           RenderSettings->AspectRatio, 
                           RenderSettings->Aperture, 
                           RenderSettings->FocusDist,
                           RenderSettings->Time0, 
                           RenderSettings->Time1);
            }
            
            if (ImGui::InputFloat("Vertical Field of View", &RenderSettings->vFov))
            {
                CameraInit(&RenderSettings->Camera,
                           RenderSettings->LookFrom, 
                           RenderSettings->LookAt, 
                           RenderSettings->Up,
                           RenderSettings->vFov, 
                           RenderSettings->AspectRatio, 
                           RenderSettings->Aperture, 
                           RenderSettings->FocusDist,
                           RenderSettings->Time0, 
                           RenderSettings->Time1);
            }
            
            if (ImGui::InputFloat("Aperture", &RenderSettings->Aperture))
            {
                CameraInit(&RenderSettings->Camera,
                           RenderSettings->LookFrom, 
                           RenderSettings->LookAt, 
                           RenderSettings->Up,
                           RenderSettings->vFov, 
                           RenderSettings->AspectRatio, 
                           RenderSettings->Aperture, 
                           RenderSettings->FocusDist,
                           RenderSettings->Time0, 
                           RenderSettings->Time1);
            }
            
            if (ImGui::InputFloat("Time 0", &RenderSettings->Time0))
            {
                CameraInit(&RenderSettings->Camera,
                           RenderSettings->LookFrom, 
                           RenderSettings->LookAt, 
                           RenderSettings->Up,
                           RenderSettings->vFov, 
                           RenderSettings->AspectRatio, 
                           RenderSettings->Aperture, 
                           RenderSettings->FocusDist,
                           RenderSettings->Time0, 
                           RenderSettings->Time1);
            }
            
            if (ImGui::InputFloat("Time 1", &RenderSettings->Time1))
            {
                CameraInit(&RenderSettings->Camera,
                           RenderSettings->LookFrom, 
                           RenderSettings->LookAt, 
                           RenderSettings->Up,
                           RenderSettings->vFov, 
                           RenderSettings->AspectRatio, 
                           RenderSettings->Aperture, 
                           RenderSettings->FocusDist,
                           RenderSettings->Time0, 
                           RenderSettings->Time1);
            }
        }
        
        if (ImGui::CollapsingHeader("Scene Settings"))
        {
        }
        
        ImGui::PopItemWidth();
        ImGui::End();
    }
    
    ImGui::Render();
    ImDrawData* ImDrawData = ImGui::GetDrawData();
    MapleDevGuiRenderDrawData(ImDrawData);
    ImGui::EndFrame();
}
