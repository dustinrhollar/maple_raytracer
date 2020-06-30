
MAPLE_DEV_UI_CALLBACK(RaytraceCallback)
{
    MapleDevGuiNewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    {
        ImGui::ShowDemoWindow();
    }
    ImGui::Render();
    ImDrawData* ImDrawData = ImGui::GetDrawData();
    MapleDevGuiRenderDrawData(ImDrawData);
    ImGui::EndFrame();
}
