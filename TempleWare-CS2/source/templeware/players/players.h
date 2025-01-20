#pragma once
#include "../hooks/includeHooks.h"

typedef void(__fastcall* onAddEntity)(__int64 CGameEntitySystem, void* entityPointer, int entityHandle);
extern onAddEntity oOnAddEntity;

// @Author: basmannetjeeee
// @IDA:
// __int64 __fastcall sub_651290(__int64 a1, __int64 a2, int a3)
void onAddEntityHook(__int64 CGameEntitySystem, void* entityPointer, int entityHandle);


typedef void(__fastcall* onRemoveEntity)(__int64 CGameEntitySystem, void* entityPointer, int entityHandle);
extern onRemoveEntity oOnRemoveEntity;

// @Author: basmannetjeeee
// @IDA:
// __int64 __fastcall sub_651890(__int64 a1, _QWORD *a2, int a3)
void onRemoveEntityHook(__int64 CGameEntitySystem, void* entityPointer, int entityHandle);