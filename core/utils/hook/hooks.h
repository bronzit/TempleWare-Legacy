#pragma once
#include "../../sdk/sdk.h"
#include "../../math/math.h"
#include "../../utils/utils.h"
#include "lib\basehookobj.h"
#include "..\..\sdk\datatypes\cutlbuffer\utlbuffer.h"
#include "../../math\elements\matrix.h"
#include "..\..\sdk\datatypes\cutlhandles\strong_handle.hpp"

#include "..\..\math\elements\vector.h"
class CRenderGameSystem;
class IViewRender;
class CCSGOInput;
class CViewSetup;
class CEconItem;
class c_grenade_trace;
class C_BaseCSGrenade;
class CGCClientSystem;
class CNetworkGameClient;
class CCSInventoryManager;
class IGameEvent;
class CUtlBuffer;
class trace_filter_t;
class trace_data_t;
class game_trace_t;

namespace hooks
{
	bool Init();

	HRESULT WINAPI hkPresent(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags);
	HRESULT __fastcall hkResizeBuffers(IDXGISwapChain* swap_chain, std::uint32_t count, std::uint32_t width, std::uint32_t height, DXGI_FORMAT new_format, std::uint32_t flags);
	HRESULT WINAPI hkCreateSwapChain(IDXGIFactory* factory, IUnknown* unknown_device, DXGI_SWAP_CHAIN_DESC* desc_swap_chain, IDXGISwapChain** swap_chain);
	long HkWndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
	bool __fastcall hkMouseInputEnabled(void* rcx);
	bool __fastcall hkUnlockInventory(void* rcx);
	void __fastcall hkFrameStageNotify(void* a1, int stage);
	void __fastcall AggreegateSceneObject(std::uintptr_t* a1, std::uintptr_t* a2, std::uintptr_t* a3, int a4, std::uintptr_t* a5, std::uintptr_t* a6, std::uintptr_t* a7, std::uintptr_t* a8);
	ViewMatrix_t* __fastcall hkViewMatrix(CRenderGameSystem* pRenderGameSystem, IViewRender* pViewRender, ViewMatrix_t* pOutWorldToView, ViewMatrix_t* pOutViewToProjection, ViewMatrix_t* pOutWorldToProjection, ViewMatrix_t* pOutWorldToPixels);
	inline WNDPROC m_old_wndproc = nullptr;
	void __fastcall hkRelativeMouse(void* a1, bool a);

	void __fastcall hkDrawSceneObject(void* ptr, void* a2, CBaseSceneData* scene_data, int count, void* a5, void* a6, void* a7, void* a8);
	void* __fastcall hkUpdateSceneObject(C_AggregateSceneObject* object, void* unk);
	void* __fastcall hkLightSceneObject(void* ptr, C_SceneLightObject* object, void* unk);
	

	inline CBaseHookObject<decltype(&hkLightSceneObject)> m_draw_light_modulation = { };
	inline CBaseHookObject<decltype(&hkUpdateSceneObject)> m_update_scene_object = { };
	inline CBaseHookObject<decltype(&hkDrawSceneObject)> m_draw_scene_object = { };
	inline CBaseHookObject<decltype(&AggreegateSceneObject)> m_aggregate_scene_object = { };
	inline CBaseHookObject<decltype(&hkPresent)> m_present = { };
	inline CBaseHookObject<decltype(&hkViewMatrix)> m_view_matrix = {};
	inline CBaseHookObject<decltype(&hkResizeBuffers)> m_resize_buffers = { };
	inline CBaseHookObject<decltype(&hkCreateSwapChain)> m_create_swap_chain = { };
	inline CBaseHookObject<decltype(&hkMouseInputEnabled)> m_mouse_input_enabled = { };
	inline CBaseHookObject<decltype(&hkFrameStageNotify)> m_frame_stage_notify = { };
	inline CBaseHookObject<decltype(&hkRelativeMouse)> m_relative_mouse = { };

	inline unsigned int(__fastcall* m_get_handle_entity)(void*);



	inline  CCSPlayerPawn* (__fastcall* m_get_local_player)(int);

	inline void* (__fastcall* m_get_base_entity)(void*, int);

	inline uint32_t(__fastcall* m_bones_num)(void*);
	inline int  m_get_weapon_data;
	inline uint32_t(__fastcall* m_bones_flags)(void*, uint32_t);
	inline const char* (__fastcall* m_bones_name)(void*, uint32_t);
	inline uint32_t(__fastcall* m_bones_parent)(void*, uint32_t);
	inline const char* (__fastcall* m_get_weapon_name)(CCSWeaponBase*);
	inline uint64_t(__fastcall* m_get_hash_function)(const char*, int32_t);

}
