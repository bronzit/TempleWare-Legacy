#pragma once
#include "includeHooks.h"
#include "..\..\cs2\entity\C_AggregateSceneObject\C_AggregateSceneObject.h"
#include "..\..\cs2\entity\C_CSPlayerPawn\C_CSPlayerPawn.h"

namespace H {
	void* __fastcall hkUpdateSceneObject(C_AggregateSceneObject* object, void* unk);
	void __fastcall hkFrameStageNotify(void* a1, int stage);

	inline CInlineHookObj<decltype(&hkFrameStageNotify)> FrameStageNotify = { };
	inline CInlineHookObj<decltype(&hkUpdateSceneObject)> UpdateWallsObject = { };
// inline hooks
	inline int  oGetWeaponData;
	inline void* (__fastcall* ogGetBaseEntity)(void*, int);
	inline  C_CSPlayerPawn* (__fastcall* oGetLocalPlayer)(int);

	class Hooks {
	public:
		void init();
	};
}