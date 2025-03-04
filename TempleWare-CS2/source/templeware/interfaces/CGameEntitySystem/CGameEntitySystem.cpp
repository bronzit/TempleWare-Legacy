#include "CGameEntitySystem.h"
#include "..\..\hooks\hooks.h"

void* CGameEntitySystem::GetEntityByIndex(int nIndex) {
	return H::ogGetBaseEntity(this, nIndex);
}
