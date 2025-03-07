#include "hud.h"
#include "../../../external/imgui/imgui.h"
#include "../config/config.h"
#include "../hooks/hooks.h"
#include <ctime>
#include <string>
#include <sstream>
#include <DirectXMath.h>

Hud::Hud() {

}

float CalculateFovRadius(float fovDegrees, float screenWidth, float screenHeight, float gameVerticalFOV) {
    // Horizontal FOV conversion to radians
    float aspectRatio = screenWidth / screenHeight;
    float fovRadians = fovDegrees * (DirectX::XM_PI / 180.0f);

    // Calculate radius based on in-game vertical FOV and screen height
    float screenRadius = std::tan(fovRadians / 2.0f) * (screenHeight / 2.0f) / std::tan(gameVerticalFOV * (DirectX::XM_PI / 180.0f) / 2.0f);

    static float flScalingMultiplier = 2.5f;

    return screenRadius * flScalingMultiplier;
}

void RenderFovCircle(ImDrawList* drawList, float fov, ImVec2 screenCenter, float screenWidth, float screenHeight, uint32_t color, float thickness) {
    float radius = CalculateFovRadius(fov, screenWidth, screenHeight, H::g_flActiveFov);
    drawList->AddCircle(screenCenter, radius, color, 100, thickness);  // 100 segments for smoothness
}

void Hud::render() {

    // Time
    std::time_t now = std::time(nullptr);
    std::tm localTime;
    localtime_s(&localTime, &now);
    char timeBuffer[9];
    std::strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &localTime);

    // FPS
    float fps = ImGui::GetIO().Framerate;
    std::ostringstream fpsStream;
    fpsStream << static_cast<int>(fps) << " FPS";

    // WaterMark
    std::string watermarkText = "TempleWare | " + fpsStream.str() + " | " + timeBuffer;

    ImVec2 textSize = ImGui::CalcTextSize(watermarkText.c_str());
    float padding = 5.0f;
    ImVec2 pos = ImVec2(10, 10);
    ImVec2 rectSize = ImVec2(textSize.x + padding * 2, textSize.y + padding * 2);

    ImU32 bgColor = IM_COL32(50, 50, 50, 200);
    ImU32 borderColor = IM_COL32(153, 76, 204, 255);
    ImU32 textColor = IM_COL32(255, 255, 255, 255);

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    drawList->AddRectFilled(pos, ImVec2(pos.x + rectSize.x, pos.y + rectSize.y), bgColor);

    float lineThickness = 2.0f;
    drawList->AddLine(pos, ImVec2(pos.x, pos.y + rectSize.y), borderColor, lineThickness);
    drawList->AddLine(ImVec2(pos.x + rectSize.x, pos.y), ImVec2(pos.x + rectSize.x, pos.y + rectSize.y), borderColor, lineThickness);

    ImVec2 textPos = ImVec2(pos.x + padding, pos.y + padding);
    drawList->AddText(textPos, textColor, watermarkText.c_str());

    if (Config::fov_circle) {
        ImVec2 Center = ImVec2(ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f);

        RenderFovCircle(drawList, Config::aimbot_fov, Center, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y, ImColor(255, 255, 255, 255), 1.f);
    }
}