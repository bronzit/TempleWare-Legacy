#include "C_CSPlayerPawn.h"

#include "../../../templeware/offsets/offsets.h"

C_CSPlayerPawn::C_CSPlayerPawn(uintptr_t address) : address(address) {}

Vector_t C_CSPlayerPawn::getPosition() const {
	return *(Vector_t*)(address + Offset::C_BasePlayerPawn::m_vOldOrigin);
}

Vector_t C_CSPlayerPawn::getEyePosition() const {
	return Vector_t();
}

uintptr_t C_CSPlayerPawn::getAddress() const {
	return address;
}

int C_CSPlayerPawn::getHealth() const {
	if (!address) return 0;
	return *reinterpret_cast<int*>(address + Offset::C_BasePlayerPawn::m_iHealth);
}

int C_CSPlayerPawn::getTeam() const {
	if (!address) return 0;
	return *reinterpret_cast<int32_t*>(address + Offset::C_BasePlayerPawn::m_iTeamNum);
}

Vector_t C_CSPlayerPawn::getViewOffset() const {
	if (!address) return Vector_t();
	return *reinterpret_cast<Vector_t*>(address + Offset::C_BasePlayerPawn::m_vecViewOffset);
}
