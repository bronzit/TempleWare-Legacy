#pragma once
#include "../../../external/imgui/imgui.h"

// CBA to make proper atm, it's 03:42 right now.
// For now just stores config values don't mind it too much
//
// (FYI THIS IS A HORRID SOLUTION BUT FUNCTIONS) 

namespace Config {
	extern bool esp;
	extern bool glow;
	extern bool showHealth;
	extern bool showNametags;
	extern bool showTracers;
	extern float espThickness;
	extern ImVec4 espColor;
	extern ImVec4 glowColor;
	extern ImVec4 tracersColor;
}