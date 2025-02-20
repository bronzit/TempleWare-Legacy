#include "entity.h"

#include "../sdk.h"
#include "..\..\utils\hook\hooks.h"

CCSPlayerController* CCSPlayerController::Get() {

	const int index = sdk::m_engine->get_local_player();
	return sdk::m_game->pGameEntitySystem->Get<CCSPlayerController>(index);

}

CCSWeaponBase* CCSPlayerPawn::GetActiveWeapon()
{
	if (!this)
		return nullptr;
	// get weapon services
	CCSPlayer_WeaponServices* weapon_services = this->m_pWeaponServices();

	// sanity
	if (weapon_services == nullptr)
		return nullptr;

	CCSWeaponBase* active_weapon = sdk::m_game->pGameEntitySystem->Get<CCSWeaponBase>(weapon_services->m_hActiveWeapon());
	if (!active_weapon)
		return nullptr;
	// return base weapon pointer from active weapon handle
	return active_weapon;
}

Vector_t CCSPlayerPawn::GetEyePosition() {
	Vector_t EyePosition;
	utils::CallVFunc<void, 169>(this, &EyePosition);
	return EyePosition;
}
bool CCSPlayerPawn::IsAlive()
{
	if (!this)
		return false;

	return m_iHealth() > 0;//m_lifeState() == LIFE_ALIVE;
}
bool CCSPlayerPawn::enemy(CCSPlayerPawn* pOther)
{
	if (!pOther || !this)
		return false;

	if (this->get_entity_by_handle() == pOther->get_entity_by_handle())
		return false;

	if (g::deathmatch)
		return true;

	return this->m_iTeamNum() != pOther->m_iTeamNum();
}
CCSWeaponBaseVData* CCSWeaponBase::GetWeaponData()
{
	// return pointer to weapon data
	return *reinterpret_cast<CCSWeaponBaseVData**>((uintptr_t)this + hooks::m_get_weapon_data);
}
bool CBaseEntity::get_bounding_box(ImVec4& out, bool item) {

	if (!this)
		return false;

	CCollisionProperty* pCollision = this->m_pCollision();
	if (!pCollision)
		return false;

	Vector_t min{}, max{};

	CTransform nodeToWorldTransform = this->m_pGameSceneNode()->m_nodeToWorld();
	const Matrix3x4_t matTransform = nodeToWorldTransform.quatOrientation.ToMatrix(nodeToWorldTransform.vecPosition);

	min = pCollision->m_vecMins();
	max = pCollision->m_vecMaxs();

	if (!item) {
		out.x = out.y = std::numeric_limits<float>::max();
		out.z = out.w = -std::numeric_limits<float>::max();
		for (int i = 0; i < 8; ++i) {
			const Vector_t vecPoint{ i & 1 ? max.x : min.x, i & 2 ? max.y : min.y, i & 4 ? max.z : min.z };
			ImVec2 vecScreen;
			if (!math::w2s(vecPoint.Transform(matTransform), vecScreen))
				return false;

			out.x = math::Min(out.x, vecScreen.x); out.y = math::Min(out.y, vecScreen.y); out.z = math::Max(out.z, vecScreen.x); out.w = math::Max(out.w, vecScreen.y);
		}

	}
	else {
		out.x = out.y = std::numeric_limits<float>::max();
		out.z = out.w = -std::numeric_limits<float>::max();
		for (int i = 0; i < 8; ++i) {
			const Vector_t vecPoint{ i & 1 ? max.x : min.x, i & 2 ? max.y : min.y, i & 4 ? max.z : min.z };
			ImVec2 vecScreen;
			if (!math::w2s(vecPoint.Transform(matTransform), vecScreen))
				return false;

			out.x = math::Min(out.x, vecScreen.x); out.y = math::Min(out.y, vecScreen.y); out.z = math::Max(out.z, vecScreen.x); out.w = math::Max(out.w, vecScreen.y);
		}

		if (item) {
			const float offset = 2.0f;
			out.x -= offset;  // Left
			out.y -= offset;  // Top
			out.z += offset;  // Right
			out.w += offset;  // Bottom
		}

	}

	return true;
}



bool CCSWeaponBase::can_primary_attack(const int nWeaponType, const float flServerTime)
{
	return (this->clip1() > 0) && (this->GetNextPrimaryAttackTick() <= flServerTime + 2);

}

bool CCSWeaponBase::can_seccondary_attack(const int nWeaponType, const float flServerTime)
{
	// check is weapon ready to attack
	if (this->GetNextSecondaryAttackTick() > TIME_TO_TICKS(flServerTime))
		return false;

	// we doesn't need additional checks for knives
	if (nWeaponType == WEAPONTYPE_KNIFE)
		return true;

	// check do weapon have ammo
	if (this->clip1() <= 0)
		return false;


	const auto nDefinitionIndex = this->m_AttributeManager().m_Item().m_iItemDefinitionIndex();

	// only revolver is allowed weapon for secondary attack
	if (nDefinitionIndex != WEAPON_R8_REVOLVER)
		return false;

	return true;
}
bool CCSPlayerPawn::can_shoot(int tick)
{
	if (!g::m_weapon_data || !sdk::m_global_vars)
		return false;

	if (const auto weapon_type{ g::m_weapon_data->m_WeaponType() }; weapon_type == WEAPONTYPE_KNIFE
		|| weapon_type == WEAPONTYPE_C4 || weapon_type == WEAPONTYPE_UNKNOWN
		|| weapon_type == WEAPONTYPE_GRENADE)
		return false;

	if (!g::m_active_weapon->clip1())
		return false;

	if (sdk::m_global_vars->m_curtime * 64 <= g::m_active_weapon->GetNextPrimaryAttackTick())
		return false;

	return true;
}

std::string CCSWeaponBase::GetWeaponName()
{
	debug(hooks::m_get_weapon_name != nullptr);
	return hooks::m_get_weapon_name(this);
}

uint32_t CModel::hitboxes_num()
{
	//	debug(hooks::m_bones_num != nullptr);
	return 27;//hooks::m_bones_num(this);
}

uint32_t CModel::hitbox_flags(uint32_t index)
{
	debug(hooks::m_bones_flags != nullptr);
	return hooks::m_bones_flags(this, index);
}

const char* CModel::hitbox_name(uint32_t index)
{
	debug(hooks::m_bones_name != nullptr);
	return hooks::m_bones_name(this, index);
}

uint32_t CModel::hitbox_parent(uint32_t index)
{
	debug(hooks::m_bones_parent != nullptr);
	return hooks::m_bones_parent(this, index);
}


void CEconItem::SetDynamicAttributeValueString(int index, const char* value)
{
}

CEconItem* CEconItem::CreateInstance()
{

	using fnCreateSharedObjectSubclassEconItem = CEconItem * (__cdecl*)();
	static fnCreateSharedObjectSubclassEconItem oCreateSharedObjectSubclassEconItem = reinterpret_cast<fnCreateSharedObjectSubclassEconItem>(utils::FindPattern(xorstr_("client.dll"), xorstr_("48 83 EC 28 B9 48 00 00 00 E8 ? ? ? ? 48 85")));
	debug(oCreateSharedObjectSubclassEconItem != nullptr);


	return oCreateSharedObjectSubclassEconItem();
}
int CCSPlayerPawn::get_index_by_hitbox(int idx) {

	switch (idx) {
	case HitBoxes::HEAD:
	case HitBoxes::NECK:
		return e_hitboxes::hitbox_head;
		break;
	case HitBoxes::LEFT_CHEST:
	case HitBoxes::RIGHT_CHEST:
	case HitBoxes::CHEST:
		return e_hitboxes::hitbox_chest;
		break;
	case HitBoxes::PELVIS:
	case HitBoxes::STOMACH:
		return e_hitboxes::hitbox_stomach;
		break;
	case HitBoxes::L_FEET:
		return e_hitboxes::hitbox_left_foot;
	case HitBoxes::R_FEET:
		return e_hitboxes::hitbox_right_foot;
		break;
	case HitBoxes::L_LEG:
		return e_hitboxes::hitbox_left_calf;
	case HitBoxes::R_LEG:
		return e_hitboxes::hitbox_right_calf;
		break;

	}

	return -999;
}
bool CEconItemDefinition::IsWeapon()
{
	return GetStickersSupportedCount() >= 4;
}

const char* CMaterial2::GetName() {

	return utils::CallVFunc<const char*, 1>(this);
}


int CCSPlayerPawn::get_lerp_tick(int player_tick_count)
{
	if (!this)
		return 0;
	return 0;
}
bool CBaseEntity::IsBasePlayerController()
{
	SchemaClassInfoData_t* pClassInfo;
	dump_class_info(&pClassInfo);
	if (pClassInfo == nullptr)
		return false;

	return hash_32_fnv1a_const(pClassInfo->szName) == hash_32_fnv1a_const("C_CSPlayerController");
}
bool CBaseEntity::IsBasePlayer()
{
	SchemaClassInfoData_t* pClassInfo;
	dump_class_info(&pClassInfo);
	if (pClassInfo == nullptr)
		return false;

	return hash_32_fnv1a_const(pClassInfo->szName) == hash_32_fnv1a_const("C_CSPlayerPawn");
}

bool CBaseEntity::IsViewModel()
{
	SchemaClassInfoData_t* pClassInfo;
	dump_class_info(&pClassInfo);
	if (pClassInfo == nullptr)
		return false;

	return hash_32_fnv1a_const(pClassInfo->szName) == hash_32_fnv1a_const("C_CSGOViewModel");
}


bool CBaseEntity::IsInferno()
{
	SchemaClassInfoData_t* pClassInfo;
	dump_class_info(&pClassInfo);
	if (pClassInfo == nullptr)
		return false;

	return hash_32_fnv1a_const(pClassInfo->szName) == hash_32_fnv1a_const("C_Inferno");
}
bool CBaseEntity::IsPlantedC4()
{
	SchemaClassInfoData_t* pClassInfo;
	dump_class_info(&pClassInfo);
	if (pClassInfo == nullptr)
		return false;

	return hash_32_fnv1a_const(pClassInfo->szName) == hash_32_fnv1a_const("C_PlantedC4");
}




bool CBaseEntity::IsWeapon()
{
	static SchemaClassInfoData_t* pWeaponBaseClass = nullptr;
	if (pWeaponBaseClass == nullptr)
		sdk::m_schema_system->FindTypeScopeForModule(xorstr_("client.dll"))->FindDeclaredClass(&pWeaponBaseClass, xorstr_("C_CSWeaponBase"));


	SchemaClassInfoData_t* pClassInfo;
	dump_class_info(&pClassInfo);
	if (pClassInfo == nullptr)
		return false;

	return (pClassInfo->InheritsFrom(pWeaponBaseClass));
}

bool CEconItemDefinition::IsKnife(bool excludeDefault, const char* name)
{
	if (strcmp(m_pszItemTypeName, "#CSGO_Type_Knife"))
		return false;


	return excludeDefault ? m_nDefIndex >= 500 : true;

}
bool CEconItemDefinition::IsCustomPlayer()
{
	if (strcmp(m_pszItemTypeName, "#Type_CustomPlayer"))
		return false;

	if (m_nDefIndex >= 5036 && m_nDefIndex < 5106)
		return false;

	if (m_nDefIndex >= 5200 && m_nDefIndex <= 5205)
		return false;

	if (m_nDefIndex >= 5300 && m_nDefIndex <= 5305)
		return false;

	if (m_nDefIndex == 5600)
		return false;

	return true;
}
bool CEconItemDefinition::IsGlove(bool excludeDefault, const char* name)
{
	static constexpr auto Type_Hands = hash_32_fnv1a_const("#Type_Hands");

	if (hash_32_fnv1a_const(m_pszItemTypeName) != Type_Hands)
		return false;

	const bool defaultGlove = m_nDefIndex == 5028 || m_nDefIndex == 5029;

	return excludeDefault ? !defaultGlove : true;
}

bool CEconItemDefinition::IsAgent(bool bExcludeDefault)
{
	return false;
}
