#include "visuals.h"
#include "../players/players.h"
#include "../utils/memory/patternscan/patternscan.h"
#include "../utils/memory/gaa/gaa.h"
#include "../../../external/imgui/imgui.h"
#include "../utils/math/math.h"

#include <algorithm>
#include <iostream>

void Visuals::init() {
	viewMatrix.viewMatrix = (viewmatrix_t*)M::getAbsoluteAddress(M::patternScan("client", "48 8D 0D ? ? ? ? 48 C1 E0 06"), 3, 0);
}

void Visuals::esp() {
	for (const auto &pawn : Players::pawns) {
		ImDrawList* drawList = ImGui::GetBackgroundDrawList();
		

		Vector_t bottomPos = pawn.getPosition();
		Vector_t topPos = bottomPos + Vector_t(0, 250.0f, 0);

		Vector_t bPosScreen;
		Vector_t tPosScreen;
		if (viewMatrix.WorldToScreen(bottomPos, bPosScreen) && viewMatrix.WorldToScreen(topPos, tPosScreen)) {
			float circleSize = 10.0f;
			drawList->AddCircleFilled(ImVec2(bPosScreen.x, bPosScreen.y), circleSize, IM_COL32(255, 255, 255, 255));
		}
	}
}