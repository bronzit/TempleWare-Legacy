#pragma once

#include <memory>
#include <array>
#include <deque>

#include "..\..\utils\render\render.h"

#include "..\..\utils\utils.h"
#include "..\..\sdk\sdk.h"
#include "..\..\math\elements\qangle.h"
#include "..\..\math\elements\vector.h"
#include "..\..\math\elements\transform.h"
#include "..\..\utils\configs\config.h"
#include <imgui.h>
enum object_type_t : int
{
	object_none = -1,
	object_player = 0,
	object_weapons = 1,
	object_molotovs = 2,
	object_planted_c4 = 3,
};

struct object_t
{
	object_t(CBaseEntity* a1, CCSPlayerPawn* plyr, CBaseHandle a2, ImVec4 a3, object_type_t a4) :
		entity(a1), p_entity(plyr), handle(a2), box(a3), type(a4) { }

	CBaseEntity* entity;
	CCSPlayerPawn* p_entity;

	CBaseHandle handle;
	ImVec4 box;
	object_type_t type;
};

namespace visuals
{
	class c_visuals {
	public:
		void cache();
		void proccess();
		bool processing_directx() {
			if (!ImGui::GetBackgroundDrawList())
				return false;

			return true;
		}
	};
	const auto visuals = std::make_unique<c_visuals>();
}
extern std::vector<object_t> m_entity_object;

