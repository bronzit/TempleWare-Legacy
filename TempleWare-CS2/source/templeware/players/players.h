#pragma once

#include "../../cs2/entity/CCSPlayerController/CCSPlayerController.h"
#include "../../cs2/entity/C_CSPlayerPawn/C_CSPlayerPawn.h"

#include <cstdint>
#include <vector>

namespace Players {
	extern std::vector<CCSPlayerController> controllers;
	extern std::vector<C_CSPlayerPawn> pawns;

	C_CSPlayerPawn getLocalPawn();
}