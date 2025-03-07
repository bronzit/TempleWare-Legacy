#pragma once
#include "includeHooks.h"
#include "../../cs2/entity/C_AggregateSceneObject/C_AggregateSceneObject.h"
#include "../../cs2/entity/C_CSPlayerPawn/C_CSPlayerPawn.h"
#include "../../cs2/datatypes/cutlbuffer/cutlbuffer.h"
#include "../../cs2/datatypes/keyvalues/keyvalues.h"
#include "../../cs2/entity/C_Material/C_Material.h"

// Forward declaration
class CMeshData;
class CEntityIdentity;

namespace H {
	void* __fastcall hkUpdateSceneObject(C_AggregateSceneObject* object, void* unk);
	void __fastcall hkFrameStageNotify(void* a1, int stage);
	void* __fastcall hkLevelInit(void* pClientModeShared, const char* szNewMap);
	void __fastcall hkChamsObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2);
	void __fastcall hkRenderFlashbangOverlay(void* a1, void* a2, void* a3, void* a4, void* a5);
	inline float g_flActiveFov;
	float hkGetRenderFov(void* rcx);

	inline CInlineHookObj<decltype(&hkChamsObject)> DrawArray = { };
	inline CInlineHookObj<decltype(&hkFrameStageNotify)> FrameStageNotify = { };
	inline CInlineHookObj<decltype(&hkUpdateSceneObject)> UpdateWallsObject = { };
	inline CInlineHookObj<decltype(&hkGetRenderFov)> GetRenderFov = { };
	inline CInlineHookObj<decltype(&hkLevelInit)> LevelInit = { };
	inline CInlineHookObj<decltype(&hkRenderFlashbangOverlay)> RenderFlashBangOverlay = { };

	// inline hooks
	inline int  oGetWeaponData;
	inline void* (__fastcall* ogGetBaseEntity)(void*, int);
	inline  C_CSPlayerPawn* (__fastcall* oGetLocalPlayer)(int);

	class Hooks {
	public:
		void init();
	};
}
