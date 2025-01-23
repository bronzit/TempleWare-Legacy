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
