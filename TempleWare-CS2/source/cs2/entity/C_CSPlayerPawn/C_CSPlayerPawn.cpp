#include "C_CSPlayerPawn.h"

#include "../../../templeware/offsets/offsets.h"
#include "../../../templeware/interfaces/interfaces.h"

C_CSPlayerPawn::C_CSPlayerPawn(uintptr_t address) : address(address) {}

Vector_t C_CSPlayerPawn::getPosition() const {
	return *(Vector_t*)(address + Offset::C_BasePlayerPawn::m_vOldOrigin);
}

Vector_t C_CSPlayerPawn::getEyePosition() const {
	return Vector_t();
}

C_CSWeaponBase* C_CSPlayerPawn::GetActiveWeapon() const {
	if (!this)
		return nullptr;

	CCSPlayer_WeaponServices* weapon_services = this->GetWeaponServices();
	if (weapon_services == nullptr)
		return nullptr;

	C_CSWeaponBase* active_weapon = I::GameEntity->Instance->Get<C_CSWeaponBase>(weapon_services->m_hActiveWeapon());
	if (!active_weapon)
		return nullptr;

	return active_weapon;
}

CCSPlayer_WeaponServices* C_CSPlayerPawn::GetWeaponServices() const {
	if (!address) return nullptr;
	return reinterpret_cast<CCSPlayer_WeaponServices*>(address + 0x11A8);
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
