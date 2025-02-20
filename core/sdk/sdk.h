#pragma once
#include <d3d11.h>


#include "interfaces/resourcesystem.h"
#include "interfaces/materialsystem.h"
#include "interfaces/swapchain.h"
#include "interfaces/inputsystem.h"
#include "interfaces/schemasystem.h"
#include "interfaces/entitysystem.h"
#include "interfaces/filesystem.h"
#include "interfaces/globalvars.h"
#include "interfaces/memory.h"

#include "addresses/entity.h"
#include <deque>
#include "..\math\elements\matrix.h"
#include "..\math\elements\qangle.h"
#include "..\math\elements\vector.h"
#include "..\math\elements\transform.h"

class CCSPlayerController;
class CPrediction;
class CCSPlayerPawn;
class CCSWeaponBase;
class CCSWeaponBaseVData;
class IEngineClient;
class ISource2Client;
class ISource2Client2;
class C_CSGameRules;
class IParticleSystemMgr;
class CGameParticleManager;
class IMemAlloc;
class IEngineCVar;
class IFileSystem;
class i_trace;
class CLocalize;
class IGameEventManager;
class INetworkClientService;
class IGameResourceService;
class IMaterialSystem2;
class IResourceSystem;
class CResourceHandleUtils;
class C_PlantedC4;
class IInputSystem;
class CLocalize {
public:
	auto FindSafe(const char* tokenName) {
		return utils::CallVFunc<const char*, 17U>(this, tokenName);
	}

};


namespace g
{
	// setup vars for local player
	inline bool m_unloading = false;
	inline float max_unlag = 0.f;
	inline float interval = 0.f;
	inline Vector_t eye_pos = Vector_t();
	inline Vector_t shoot_pos = Vector_t();
	inline float lerp = 0.f;
	inline Vector_t angle = Vector_t();
	inline Vector_t view_origin = Vector_t();

	inline QAngle_t command__number_angle = QAngle_t();
	inline bool trigger_enable{};
	inline HWND m_hwnd = nullptr;
	inline int last_trigger_shot_time{};
	inline int current_trigger_tick{};
	inline int trigger_delay{};
	inline void* m_module = nullptr;
	inline CCSPlayerController* m_local_player_controller = nullptr;
	inline CCSPlayerPawn* m_local_player_pawn = nullptr;
	inline CCSWeaponBase* m_active_weapon = nullptr;
	inline CCSWeaponBaseVData* m_weapon_data = nullptr;
	inline ViewMatrix_t m_view_matrix = ViewMatrix_t();
	inline Vector_t camera_position = Vector_t();
	inline	float sv_friction{};
	inline	float sv_accelerate{};
	inline	float sv_airaccelerate{};
	inline bool sv_autobunnyhopping{};
	inline float  m_yaw{};
	inline float m_pitch{};
	inline bool attack{};
	inline bool history_on_tick{};
	inline bool shot_fired{};
	inline bool client_performing_attack{};
	inline bool prev_button_attack{};
	inline bool client_attack{};
	inline bool client_trigger_attack{};
	inline int tick_base{};
	inline QAngle_t m_backup_angle = QAngle_t();
	inline bool attack2{};
	inline bool on_ground{};
	inline float sensitivity{};
	inline bool last_relative_mouse_node{};
	inline bool deathmatch{};
	static std::uint32_t cmd_random_seed{ };
	static int store_shot_input_history_index{ };
	static char effects_name[32];
	inline std::deque<ImVec2> lines;
	inline std::deque<Vector_t> lines_3d;
}

namespace command {
	inline CCSPlayerPawn* m_aim_target = nullptr;
	inline CCSPlayerPawn* m_rage_aim_target = nullptr;
	inline QAngle_t m_final_angle{};
	inline int m_tick_count{};
	inline float m_tick_fraction{};
	inline bool m_stop{};
}
class CCSGOInput;
namespace sdk
{
	bool Init();

	inline ISwapChainDx11* m_swap_chain = nullptr;
	inline ID3D11Device* m_device = nullptr;
	inline ID3D11DeviceContext* m_device_context = nullptr;
	inline ID3D11RenderTargetView* m_render_target_view = nullptr;
	inline IMemAlloc* m_mem_alloc = nullptr;
	inline CPrediction* m_prediction = nullptr;
	inline IInputSystem* m_input_system = nullptr;
	inline ISchemaSystem* m_schema_system = nullptr;
	inline IEngineClient* m_engine = nullptr;
	inline ISource2Client* m_client = nullptr;
	inline ISource2Client* m_source_client = nullptr;
	inline IGameResourceService* m_game = nullptr;
	inline IMaterialSystem2* m_material = nullptr;
	inline IResourceSystem* m_resource = nullptr;
	inline CResourceHandleUtils* m_resource_utils = nullptr;
	inline IEngineCVar* m_cvar = nullptr;
	inline INetworkClientService* m_network = nullptr;
	inline i_trace* m_trace = nullptr;
	inline CLocalize* m_localize = nullptr;
	inline IGameEventManager* m_game_event = nullptr;
	inline C_CSGameRules* m_game_rules = nullptr;
	inline CGlobalVarsBase* m_global_vars = nullptr;
	inline IFileSystem* m_file_system = nullptr;
	inline CCSGOInput* m_csgo_input = nullptr;

	inline C_PlantedC4* m_planted_c4 = nullptr;
	inline IParticleSystemMgr* m_particle_system = nullptr;
	inline CGameParticleManager* m_particle_game = nullptr;

}
