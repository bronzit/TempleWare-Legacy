#include "visuals.h"
#include "../players/players.h"
#include "../utils/memory/patternscan/patternscan.h"
#include "../utils/memory/gaa/gaa.h"
#include "../../../external/imgui/imgui.h"
#include "../utils/math/math.h"

#include <algorithm>
#include <iostream>
#include "../config/config.h"

void Visuals::init() {
    viewMatrix.viewMatrix = (viewmatrix_t*)M::getAbsoluteAddress(M::patternScan("client", "48 8D 0D ? ? ? ? 48 C1 E0 06"), 3, 0);
}

void Visuals::esp() {
    C_CSPlayerPawn localPawn = Players::getLocalPawn();
    if (!localPawn.getAddress()) { 
        return;
    }

    for (const auto& pawn : Players::pawns) {
        if (pawn.getHealth() <= 0)
            continue;

        if (pawn.getAddress() == localPawn.getAddress())
            continue;

        if (Config::teamCheck && (pawn.getTeam() == localPawn.getTeam()))
            continue;

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();

        Vector_t feetPos = pawn.getPosition();
        Vector_t headPos = pawn.getPosition() + pawn.getViewOffset();

        Vector_t feetScreen, headScreen;
        if (!viewMatrix.WorldToScreen(feetPos, feetScreen) ||
            !viewMatrix.WorldToScreen(headPos, headScreen))
            continue;

        float boxHeight = feetScreen.y - headScreen.y;
        float boxWidth = boxHeight / 2.0f;

        float centerX = (feetScreen.x + headScreen.x) / 2.0f;
        float boxX = centerX - (boxWidth / 2.0f);
        float boxY = headScreen.y;

        ImVec4 espColorWithAlpha = Config::espColor;
        espColorWithAlpha.w = Config::espFillOpacity;
        ImU32 boxColor = ImGui::ColorConvertFloat4ToU32(Config::espColor);
        ImU32 fillColor = ImGui::ColorConvertFloat4ToU32(espColorWithAlpha);

        if (Config::espFill) {
            drawList->AddRectFilled(
                ImVec2(boxX, boxY),
                ImVec2(boxX + boxWidth, boxY + boxHeight),
                fillColor
            );
        }

        drawList->AddRect(
            ImVec2(boxX, boxY),
            ImVec2(boxX + boxWidth, boxY + boxHeight),
            boxColor,
            0.0f,
            0,
            Config::espThickness
        );

        if (Config::showHealth) {
            int health = pawn.getHealth();
            float healthHeight = boxHeight * (static_cast<float>(health) / 100.0f);
            float barWidth = 4.0f;
            float barX = boxX - (barWidth + 2);
            float barY = boxY + (boxHeight - healthHeight);

            drawList->AddRectFilled(
                ImVec2(barX, boxY),
                ImVec2(barX + barWidth, boxY + boxHeight),
                IM_COL32(70, 70, 70, 255)
            );

            ImU32 healthColor = IM_COL32(
                static_cast<int>((100 - health) * 2.55f),
                static_cast<int>(health * 2.55f),
                0,
                255
            );
            drawList->AddRectFilled(
                ImVec2(barX, barY),
                ImVec2(barX + barWidth, barY + healthHeight),
                healthColor
            );

            std::string displayText = "[" + std::to_string(health) + "HP]";
            ImVec2 textSize = ImGui::CalcTextSize(displayText.c_str());
            float textX = boxX + (boxWidth - textSize.x) / 2;
            float textY = boxY + boxHeight + 2;
            drawList->AddText(
                ImVec2(textX + 1, textY + 1),
                IM_COL32(0, 0, 0, 255),
                displayText.c_str()
            );
            drawList->AddText(
                ImVec2(textX, textY),
                IM_COL32(255, 255, 255, 255),
                displayText.c_str()
            );
        }
    }
}
