#include "config.h"

// CBA to make proper atm, it's 03:42 right now.
// For now just stores config values don't mind it too much
//
// (FYI THIS IS A HORRID SOLUTION BUT FUNCTIONS) 

namespace Config {
	bool esp = false;
	bool glow = false;
	bool showHealth = false;
	bool showNametags = false;
	bool showTracers = false;
	float espThickness = 1.0f;
	ImVec4 espColor = ImVec4(1, 0, 0, 1);
	ImVec4 glowColor = ImVec4(1, 0, 0, 1);
	ImVec4 tracersColor = ImVec4(1, 0, 0, 1);
}