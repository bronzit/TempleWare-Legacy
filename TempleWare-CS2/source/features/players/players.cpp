#include "players.h"
#include "../../game/datatypes/schema/ISchemaClass/ISchemaClass.h"

void onAddEntityHook(__int64 CGameEntitySystem, void* entityPointer, int entityHandle) {
	if (!entityPointer || !CGameEntitySystem || !entityHandle) {
		oOnAddEntity(CGameEntitySystem, entityPointer, entityHandle);
		return;
	}

	uintptr_t uEntityPointer = reinterpret_cast<uintptr_t>(entityPointer);

	SchemaClassInfoData_t* entityInfo = nullptr;
	GetSchemaClassInfo(uEntityPointer, &entityInfo);

	if (entityInfo == nullptr) {
		oOnAddEntity(CGameEntitySystem, entityPointer, entityHandle);
		return;
	}

	if (strcmp(entityInfo->szName, "C_CSPlayerPawn") == 0) {

		// Add pawn to respective player in player list

		oOnAddEntity(CGameEntitySystem, entityPointer, entityHandle);
		return;
	}
	if (strcmp(entityInfo->szName, "CCSPlayerController") == 0) {

		// Add controller to respective player in player list

		oOnAddEntity(CGameEntitySystem, entityPointer, entityHandle);
		return;
	}

	oOnAddEntity(CGameEntitySystem, entityPointer, entityHandle);
	return;
}

void onRemoveEntityHook(__int64 CGameEntitySystem, void* entityPointer, int entityHandle) {
	if (!entityPointer || !CGameEntitySystem || !entityHandle) {
		oOnRemoveEntity(CGameEntitySystem, entityPointer, entityHandle);
		return;
	}

	uintptr_t uEntityPointer = reinterpret_cast<uintptr_t>(entityPointer);

	// Check player's entity pointers and if matches then remove player from player list

	oOnRemoveEntity(CGameEntitySystem, entityPointer, entityHandle);
	return;
}

onAddEntity oOnAddEntity = nullptr;
onRemoveEntity oOnRemoveEntity = nullptr;
