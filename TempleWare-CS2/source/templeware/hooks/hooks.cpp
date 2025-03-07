#include "hooks.h"
#include <iostream>

#include "../../../external/kiero/minhook/include/MinHook.h"

#include "../../templeware/utils/memory/Interface/Interface.h"
#include "../utils/memory/patternscan/patternscan.h"
#include "../utils/memory/gaa/gaa.h"

#include "../players/hook/playerHook.h"
#include "../features/visuals/visuals.h"
#include "../features/chams/chams.h"

#include "../../cs2/datatypes/cutlbuffer/cutlbuffer.h"
#include "../../cs2/datatypes/keyvalues/keyvalues.h"
#include "../../cs2/entity/C_Material/C_Material.h"

#include "../config/config.h"
#include "../interfaces/interfaces.h"
#include "../features/aim/aim.h"

void __fastcall H::hkFrameStageNotify(void* a1, int stage)
{
	FrameStageNotify.GetOriginal()(a1, stage);

	// frame_render_stage | 9
	if (stage == 9 && oGetLocalPlayer(0)) {
		Esp::cache();

		Aimbot();
	}
}

void* __fastcall H::hkLevelInit(void* pClientModeShared, const char* szNewMap) {
	static void* g_pPVS = (void*)M::getAbsoluteAddress(M::patternScan("engine2", "48 8D 0D ? ? ? ? 33 D2 FF 50"), 0x3);

	M::vfunc<void*, 6U, void>(g_pPVS, false);

	return LevelInit.GetOriginal()(pClientModeShared, szNewMap);
}

void H::Hooks::init() {

	oGetWeaponData = *reinterpret_cast<int*>(M::patternScan("client", ("48 8B 81 ? ? ? ? 85 D2 78 ? 48 83 FA ? 73 ? F3 0F 10 84 90 ? ? ? ? C3 F3 0F 10 80 ? ? ? ? C3 CC CC CC CC")) + 0x3);
	ogGetBaseEntity = reinterpret_cast<decltype(ogGetBaseEntity)>(M::patternScan("client", ("81 FA ? ? ? ? 77 ? 8B C2 C1 F8 ? 83 F8 ? 77 ? 48 98 48 8B 4C C1 ? 48 85 C9 74 ? 8B C2 25 ? ? ? ? 48 6B C0 ? 48 03 C8 74 ? 8B 41 ? 25 ? ? ? ? 3B C2 75 ? 48 8B 01")));
	oGetLocalPlayer = reinterpret_cast<decltype(oGetLocalPlayer)>(M::patternScan("client", ("48 83 EC 28 83 F9 FF 75 17 48 8B 0D ?? ?? ?? ?? 48 8D 54 24 30 48 8B 01 FF 90 ?? ?? ?? ?? 8B 08 48 63 C1 48 8D 0D ?? ?? ?? ?? 48 8B 0C")));

	UpdateWallsObject.Add((void*)M::patternScan("scenesystem", ("48 89 5C 24 10 48 89 6C 24 18 56 57 41 54 41 56 41 57 48 83 EC 40")), &hkUpdateSceneObject);
	FrameStageNotify.Add((void*)M::patternScan("client", ("48 89 5C 24 ? 56 48 83 EC 30 8B 05 ? ? ? ?")), &hkFrameStageNotify);
	DrawArray.Add((void*)M::patternScan("scenesystem", ("48 8B C4 48 89 50 10 53 41 55 41 56 48 81 EC ? ? ? ? 4D 63 F1")), &chams::hook);
	GetRenderFov.Add((void*)M::getAbsoluteAddress(M::patternScan("client", "E8 ? ? ? ? F3 0F 11 45 00 48 8B 5C 24 40"), 1), &hkGetRenderFov);
	LevelInit.Add((void*)M::getAbsoluteAddress(M::patternScan("client", "E8 ? ? ? ? C6 83 ? ? ? ? ? C6 83"), 1), &hkLevelInit);
	RenderFlashBangOverlay.Add((void*)M::patternScan("client", ("85 D2 0F 88 ? ? ? ? 55 56 41 55")), &hkRenderFlashbangOverlay);

	MH_EnableHook(MH_ALL_HOOKS);
}
