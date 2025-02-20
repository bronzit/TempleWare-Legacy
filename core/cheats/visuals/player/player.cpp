#include "player.h"
#include "..\preview\preview.h"
#include "..\..\..\utils\render\render.h"
#include "../../../utils/hook/hooks.h"
#include "../../../utils/hook/hooks.h"
#include "..\proccess.h"
using namespace visuals::preview;

void skeleton(CCSPlayerPawn* player) {

	if (!cfg_get(bool, g_cfg.bSkeleton))
		return;

	if (!g::m_local_player_pawn)
		return;

	auto game_scene_node = player->m_pGameSceneNode();
	if (!game_scene_node)
		return;

	auto skeleton = game_scene_node->m_pSkeleton();
	if (!skeleton)
		return;
	auto model_state = &skeleton->GetModel();
	stronghandle<CModel> model = model_state->m_hModel();
	auto model_skelet = &model->m_modelSkeleton();

	if (!model_skelet)
		return;


//	skeleton->calc_world_space_bones(0, bone_flags::FLAG_HITBOX);

	const auto num_bones = model->hitboxes_num();
	auto bones = model_state->hitbox_data();
	auto delta_penisov = bones[4].pos - bones[5].pos;
	auto bone_penis_fix = bones[5].pos + delta_penisov * 0.5f;


	for (uint32_t i = 0; i < num_bones; i++) {
		if (!(model->hitbox_flags(i) & bone_flags::FLAG_HITBOX)) {
			continue;
		}

		auto parent_index = model->hitbox_parent(i);
		if (parent_index == -1)
			continue;

		auto child = bones[i].pos;
		auto parent = bones[parent_index].pos;

		if (parent_index == 7 || parent_index == 12)
			parent = bone_penis_fix;

		ImVec2 start_scr, end_scr;
		if (!math::WorldToScreen(child, start_scr) || !math::WorldToScreen(parent, end_scr))
			continue;

		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(start_scr.x, start_scr.y), ImVec2(end_scr.x, end_scr.y), cfg_get(ColorPickerVar_t, g_cfg.colSkeletonOutline).colValue.GetU32(), 2.f);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(start_scr.x, start_scr.y), ImVec2(end_scr.x, end_scr.y), cfg_get(ColorPickerVar_t, g_cfg.colSkeleton).colValue.GetU32(), 1.f);

	}
}
std::mutex m_mutex;

std::string extractWeaponName(const std::string& fullName) {
	// Remove the "weapon_" prefix
	const std::string prefix = "weapon_";
	std::string truncatedName = fullName;

	// Check if the prefix exists in the name
	if (truncatedName.find(prefix) == 0) {
		truncatedName = truncatedName.substr(prefix.length());
	}

	// Find the first underscore after the prefix removal
	size_t underscorePos = truncatedName.find('_');
	if (underscorePos != std::string::npos) {
		truncatedName = truncatedName.substr(0, underscorePos);
	}

	return truncatedName;
}

const char* convertToUpperr(const char* str) {
	static char buffer[256]; // Adjust the size as needed
	int i = 0;

	// Convert each character to uppercase
	while (str[i] != '\0' && i < sizeof(buffer) - 1) {
		buffer[i] = toupper(str[i]);
		i++;
	}
	buffer[i] = '\0'; // Null-terminate the string

	return buffer;
}

void player::c_esp::run()
{
	/* stupid sanity checks */
	if (!cfg_get(bool, g_cfg.bVisualOverlay))
		return;

	if (!sdk::m_engine->is_valid())
		return;

	auto local_controller = CCSPlayerController::Get();
	if (!local_controller)
		return;

	auto local = sdk::m_game->pGameEntitySystem->Get<CCSPlayerPawn>(local_controller->PawnHandle());
	if (!local)
		return;

	if (m_entity_object.empty())
		return;

	/* loop thru cached entities and run esp on each them */
	/* im not sure about rendering delay on ms etc thats something that gotta be benchmarked with this "caching" method */
	for (auto& ent_object : m_entity_object)
	{
		// if the handle is invalid, skip this entity
		if (!ent_object.handle.valid() || ent_object.handle.index() == 0xFF)
			continue;

		if (ent_object.type != object_player)
			continue;

		auto _controller = sdk::m_game->pGameEntitySystem->Get<CCSPlayerController>(ent_object.handle.index());
		if (!_controller)
			continue;

		auto player = sdk::m_game->pGameEntitySystem->Get<CCSPlayerPawn>(_controller->PawnHandle().index());
		if (!player)
			continue;

		/* check if player's valid blabla */
		if (!player->IsAlive() || !player->enemy(local))
			continue;

		if (ent_object.handle.serial_number() != _controller->handle().serial_number() || !player->GetActiveWeapon())
			continue;

		if (player->m_iHealth() != ent_object.p_entity->m_iHealth())
			continue;

		skeleton(player);

		visuals::preview::Context_t context;

		ImVec4 box = ent_object.box;


		/* bounding box esp */
		if (const auto& frameOverlayConfig = cfg_get(FrameOverlayVar_t, g_cfg.overlayBox); frameOverlayConfig.bEnable) {

			context.AddBoxComponent(ImGui::GetBackgroundDrawList(), box, 1, frameOverlayConfig.flThickness, frameOverlayConfig.flRounding, frameOverlayConfig.colPrimary, frameOverlayConfig.colOutline, 1.f);
		}
		/* health bar esp */
		if (const auto& health_config = cfg_get(BarOverlayVar_t, g_cfg.overlayHealthBar); health_config.bEnable) {

			auto hp = player->m_iHealth() / 100.f;

			context.AddComponent(new CBarComponent(false, SIDE_LEFT, box, 100, hp, g_cfg.overlayHealthBar, 1.f));
		}

		/* name esp */
		if (const auto& name_config = cfg_get(TextOverlayVar_t, g_cfg.overlayName); name_config.bEnable) {
			const char* szName = ("");

			szName = _controller->GetPlayerName();

			context.AddComponent(new CTextComponent(true, false, SIDE_TOP, DIR_TOP, render::m_esp_name, szName, g_cfg.overlayName, 1.f));
		}

		if (const auto& weaponOverlayConfig = cfg_get(TextOverlayVar_t, g_cfg.Weaponesp); weaponOverlayConfig.bEnable) {
			auto wep = player->GetActiveWeapon();
			if (wep && ent_object.p_entity->GetActiveWeapon() && wep == ent_object.p_entity->GetActiveWeapon()) {

				if (wep->GetWeaponData()) {
					auto name = wep->GetWeaponData()->m_szName();

					// Extract the weapon name
					std::string cleanedWeaponName = extractWeaponName(name);
					const char* upperCaseWeaponName = convertToUpperr(cleanedWeaponName.c_str());

				
						context.AddComponent(new CTextComponent(true, false, SIDE_BOTTOM, DIR_BOTTOM, render::m_esp_weapons, upperCaseWeaponName, g_cfg.Weaponesp, 1.f));
					

				}

			}

		}

		if (const auto& ammo_config = cfg_get(BarOverlayVar_t, g_cfg.AmmoBar); ammo_config.bEnable) {
			auto wep = player->GetActiveWeapon();
			if (wep) {
				const auto ammo = wep->clip1();
				auto data = wep->GetWeaponData();
				if (data) {
					const auto max_ammo = data->m_iMaxClip1();;

					if (max_ammo > 0) {
						const float factor = static_cast<float>(ammo) / max_ammo;
						// here we use the SIDE_BOTTOM which overrided max_ammo as the var used for factor limitation
						context.AddComponent(new CBarComponent(true, SIDE_BOTTOM, box, max_ammo, factor, g_cfg.AmmoBar, 1.f));
					}
				}
			}
		}

		/* Start Player Flags*/
		{
			if (cfg_get(unsigned int, g_cfg.pEspFlags) & FLAGS_ARMOR) {
				if (player->GetArmorValue() > 0) {
					if (const auto& hk_cfg = cfg_get(TextOverlayVar_t, g_cfg.HKFlag); hk_cfg.bEnable) {
						const char* szName = ("K");

						context.AddComponent(new CTextComponent(true, false, SIDE_RIGHT, DIR_RIGHT, render::m_esp_flags, szName, g_cfg.HKFlag, 1.f));
					}
				}
			}

		}


		context.Render(ImGui::GetBackgroundDrawList(), box);

	}

}