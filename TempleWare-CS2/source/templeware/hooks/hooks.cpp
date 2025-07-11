#include "hooks.h"
#include <iostream>

#include "../../../external/kiero/minhook/include/MinHook.h"

#include "../../templeware/utils/memory/Interface/Interface.h"
#include "../utils/memory/patternscan/patternscan.h"
#include "../utils/memory/gaa/gaa.h"

#include "../features/visuals/visuals.h"
#include "../features/chams/chams.h"

#include "../../cs2/datatypes/cutlbuffer/cutlbuffer.h"
#include "../../cs2/datatypes/keyvalues/keyvalues.h"
#include "../../cs2/entity/C_Material/C_Material.h"

#include "../config/config.h"
#include "../interfaces/interfaces.h"
#include "../features/aim/aim.h"


void* __fastcall H::hkUpdateSkybox(c_env_sky* sky) {
    static auto org = UpdateSkybox.GetOriginal();
    
    static uint8_t nOldColor[3] = {
        sky->m_tint_color()[0],
        sky->m_tint_color()[1],
        sky->m_tint_color()[2]
    };
    
    sky->m_tint_color() = 
    Config::skybox ? Color_t(
        static_cast<uint8_t>(Config::skyboxcolor.x * 255.0f), 
        static_cast<uint8_t>(Config::skyboxcolor.y * 255.0f), 
        static_cast<uint8_t>(Config::skyboxcolor.z * 255.0f)
    ) : 
        Color_t(
        nOldColor[0], 
        nOldColor[1], 
        nOldColor[2]
    );

    sky->m_brightness_scale() = Config::fog ? 0.f : 1.f;

    sky->PostDataUpdate(1);

    return org(sky);
}


void* __fastcall H::hkDrawLegs(void* a1, void* a2, void* a3, void* a4, void* a5) {
    if (Config::removelegs) return nullptr;
    return DrawLegs.GetOriginal()(a1, a2, a3, a4, a5);
}


void __fastcall H::hkDrawScopeOverlay(void* a1, void* a2) {
    if (Config::ScopeRemove) return;
    return DrawScopeOverlay.GetOriginal()(a1, a2);
}


enum c_stages : std::uint32_t
{
    frame_handler_start = 0,
    frame_handler_end = 1,
    frame_prediction = 3,
    frame_net_full_frame_update_on_remove = 2,
    frame_net_update_start = 4,
    frame_net_update_postdataupdate_start = 5,
    frame_net_update_postdataupdate_end = 6,
    frame_net_update_end = 7,
    frame_start = 8,
    frame_render = 9
};

void __fastcall H::hkFrameStageNotify(void* a1, int stage)
{
    FrameStageNotify.GetOriginal()(a1, stage);

    if (oGetLocalPlayer(0)) {

        switch (stage) {

        case frame_handler_start:
            FakeSpamm();
            break;
        

        case frame_prediction:
            Triggerbot();
            Aimbot();
            break;
        

        case frame_render:
            Esp::cache();
            break;
        

        }

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
    DrawSmokeArray.Add((void*)M::patternScan("client", ("48 89 54 24 ? 55 41 55 48 8D AC 24")), &hkDrawSmokeArray);
    DrawScopeOverlay.Add((void*)M::patternScan("client", ("4C 8B DC 53 56 57 48 83 EC")), &hkDrawScopeOverlay);
    DrawLegs.Add((void*)M::patternScan("client", ("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 01")), &hkDrawLegs);
    DrawViewModel.Add((void*)M::patternScan("client", ("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC ? 4D 8B E0")), &hkDrawViewModel);
    LightingModulate.Add((void*)M::patternScan("scenesystem", ("48 89 54 24 ? 53 41 56 41 57")), &hkLightingModulate);
    MouseInputEnabled.Add((void*)M::patternScan("client", ("40 53 48 83 EC 20 80 B9 ? ? ? ? ? 48 8B D9 75 78")), &hkMouseInputEnabled);
    IsRelativeMouseMode.Add((void*)M::getvfunc(I::InputSys, 76U), &hkIsRelativeMouseMode);
    UpdateSkybox.Add((void*)M::patternScan("client", ("48 8B C4 48 89 58 18 48 89 70 20 55 57 41 54 41 55")), &hkUpdateSkybox);

    MH_EnableHook(MH_ALL_HOOKS);
}   
