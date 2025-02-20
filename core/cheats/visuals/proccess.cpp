#include "proccess.h"
#include "player/player.h"
#include "../../utils/hook/hooks.h"
std::mutex m_proccessing_mutex;
std::vector<object_t> m_entity_object;

void visuals::c_visuals::proccess()
{
	if (!sdk::m_engine->is_valid())
		return;

	if (!this->processing_directx())
		return;

	auto local_controller = CCSPlayerController::Get();
	if (!local_controller)
		return;

	auto local = sdk::m_game->pGameEntitySystem->Get<CCSPlayerPawn>(local_controller->PawnHandle());
	if (!local)
		return;

	player::esp->run();
}


void visuals::c_visuals::cache()
{
	if (!sdk::m_engine->is_valid())
		return;

	m_entity_object.clear();
	m_entity_object.reserve(32);

	auto local = hooks::m_get_local_player(0);
	if (!local)
		return;

	auto local_controller = CCSPlayerController::Get();
	if (!local_controller)
		return;

	/* start player caching etc */
	{
		/* make sure our vector holds max capacity */
		int max_ents = sdk::m_game->pGameEntitySystem->GetHighestEntityIndex();

		for (int i = 1; i <= max_ents; i++)
		{
			auto entity = sdk::m_game->pGameEntitySystem->Get(i);
			if (!entity)
				continue;

			if (!entity->handle().valid())
				continue;

			SchemaClassInfoData_t* _class = nullptr;
			entity->dump_class_info(&_class);
			if (!_class)
				continue;

			const uint32_t hash = hash_32_fnv1a_const(_class->szName);

			Vector_t m_origin = Vector_t();
			object_type_t m_type = object_none;

			/* perhaps i coudl use this logic instead for all entities and sort them based on class name ??? prob better*/
			if (hash == hash_32_fnv1a_const(xorstr_("CCSPlayerController")))
			{

				m_type = object_player;
				CCSPlayerController* p_controller = reinterpret_cast<CCSPlayerController*>(entity);
				/* make sure to avoid caching local */
				if (!p_controller || p_controller == local_controller)
					continue;

				if (!p_controller->PawnHandle().valid())
					continue;

				auto player = sdk::m_game->pGameEntitySystem->Get<CCSPlayerPawn>(p_controller->PawnHandle().index());
				if (!player)
					continue;

				if (player->m_iHealth() <= 0 || !player->GetActiveWeapon())
					continue;
				/* store data here */
				m_origin = Vector_t();

				ImVec4 box;
				player->get_bounding_box(box, false);


				m_entity_object.emplace_back(entity, player, p_controller->handle(), box, m_type);
			} 
		}
	}

}