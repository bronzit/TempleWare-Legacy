#include "hooks.h"

#include "../render/render.h"
#include "../render/ui/menu.h"

#include "..\..\sdk\interfaces\cvar.h"
#include "..\..\sdk\interfaces\network.h"
#include "..\..\sdk\datatypes\cutlbuffer\utlbuffer.h"
#include "..\..\cheats\visuals\proccess.h"
#include "..\..\sdk\addresses\entity.h"

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

// macro to release dxgi pointers
// @note: malloc may be better than method than nulling x

static bool initialized = false;

// macro to release dxgi pointers
// @note: malloc may be better than method than nulling x
#define RELEASE_DXGI( x )   \
if ( x != nullptr )         \
{                           \
    x->Release( );          \
    x = nullptr;            \
}                           \

// macro to hook (looks better lmao)
#define ATTACH_HOOK_IDX( vtable_ptr, index, target, basehookobj ) \
basehookobj.Create( utils::GetVFunc( vtable_ptr, index ), target ); \

#define ATTACH_HOOK_SIG( ptr, target, basehookobj ) \
basehookobj.Create( (void*)ptr, target ); \
// Declaration of m_decode_crc as void*
bool hooks::Init()
{
    bool success = true;

    // init minhook
    MH_Initialize();

    m_old_wndproc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(g::m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HkWndProc)));
    success &= (m_old_wndproc != nullptr);
    
    m_bones_num = reinterpret_cast<decltype(m_bones_num)>
        (utils::GetAbsoluteAddress(utils::FindPattern("client.dll", xorstr_("E8 ? ? ? ? 8B F8 89 44 24 ? EB ? 8B 7C 24")), 0x1, 0x5));

    m_bones_flags = reinterpret_cast<decltype(m_bones_flags)>
        (utils::FindPattern("client.dll", "85 D2 78 16 3B 91"));

    m_bones_name = reinterpret_cast<decltype(m_bones_name)>
        (utils::FindPattern("client.dll", "85 D2 78 25 3B 91"));

    m_bones_parent = reinterpret_cast<decltype(m_bones_parent)>
        (utils::FindPattern("client.dll", "85 D2 78 17 3B 91 78"));

    m_get_weapon_name = reinterpret_cast<decltype(m_get_weapon_name)>
        (utils::FindPattern("client.dll", "48 8B 81 ? ? ? ? 48 8B 88 ? ? ? ? 48 8D 05 ? ? ? ? 48 85 C9 48 0F 45 C1 C3 CC CC CC 44 0F B6 41 ?"));

    m_get_hash_function = reinterpret_cast<decltype(m_get_hash_function)>(utils::FindPattern("client.dll", "48 89 5C 24 08 44 0F"));

    m_get_handle_entity = reinterpret_cast<decltype(m_get_handle_entity)>(utils::FindPattern("client.dll", "48 85 C9 74 32 48 8B 41 10 48 85 C0 74 29 44"));

    m_get_base_entity = reinterpret_cast<decltype(m_get_base_entity)>
        (utils::FindPattern("client.dll", ("81 FA ? ? ? ? 77 ? 8B C2 C1 F8 ? 83 F8 ? 77 ? 48 98 48 8B 4C C1 ? 48 85 C9 74 ? 8B C2 25 ? ? ? ? 48 6B C0 ? 48 03 C8 74 ? 8B 41 ? 25 ? ? ? ? 3B C2 75 ? 48 8B 01")));

    m_get_weapon_data = *reinterpret_cast<int*>(utils::FindPattern(xorstr_("client.dll"), xorstr_("48 8B 81 ? ? ? ? 85 D2 78 ? 48 83 FA ? 73 ? F3 0F 10 84 90 ? ? ? ? C3 F3 0F 10 80 ? ? ? ? C3 CC CC CC CC")) + 0x3);

    m_get_local_player = reinterpret_cast<decltype(m_get_local_player)>(utils::FindPattern("client.dll", xorstr_("48 83 EC 28 83 F9 FF 75 17 48 8B 0D ?? ?? ?? ?? 48 8D 54 24 30 48 8B 01 FF 90 ?? ?? ?? ?? 8B 08 48 63 C1 48 8D 0D ?? ?? ?? ?? 48 8B 0C")));

    // hook vtable functions 
    ATTACH_HOOK_IDX(sdk::m_swap_chain->m_swap_chain, 8, &hkPresent, m_present);
    ATTACH_HOOK_IDX(sdk::m_swap_chain->m_swap_chain, 13, &hkResizeBuffers, m_resize_buffers);
    ATTACH_HOOK_IDX(sdk::m_input_system, 76, &hkRelativeMouse, m_relative_mouse);
    ATTACH_HOOK_IDX(sdk::m_csgo_input, 19, &hkMouseInputEnabled, m_mouse_input_enabled);

    ATTACH_HOOK_SIG(utils::FindPattern(xorstr_("client.dll"), xorstr_("40 53 48 81 EC ? ? ? ? 49 8B C1")), &hkViewMatrix, m_view_matrix);
    ATTACH_HOOK_SIG(utils::FindPattern(xorstr_("client.dll"), xorstr_("48 89 5C 24 ? 56 48 83 EC 30 8B 05 ? ? ? ?")), &hkFrameStageNotify, m_frame_stage_notify);    
    ATTACH_HOOK_SIG(utils::FindPattern(xorstr_("scenesystem.dll"), xorstr_("48 89 5C 24 10 48 89 6C 24 18 56 57 41 54 41 56 41 57 48 83 EC 40")), &hkUpdateSceneObject, m_update_scene_object);
   
    /* release DXGI hooks */
    {
        // create dxgi deviceB
        IDXGIDevice* dxgi_device = nullptr;

        sdk::m_device->QueryInterface(__uuidof(**(&dxgi_device)), IID_PPV_ARGS_Helper(&dxgi_device));
        success &= (dxgi_device != nullptr);

        // get dxgi adapter from dxgi device
        IDXGIAdapter* dxgi_adapter = nullptr;
        dxgi_device->GetAdapter(&dxgi_adapter);
        success &= (dxgi_adapter != nullptr);

        // get dxgi factory from dxgi adapter
        IDXGIFactory* dxgi_factory = nullptr;
        dxgi_adapter->GetParent(__uuidof(**(&dxgi_factory)), IID_PPV_ARGS_Helper(&dxgi_factory));
        success &= (dxgi_factory != nullptr);

        // hook create swap chain
        ATTACH_HOOK_IDX(dxgi_factory, 10, &hkCreateSwapChain, m_create_swap_chain);

        // free dxgi pointers
        dxgi_device->Release();
        dxgi_adapter->Release();
        dxgi_factory->Release();
    }

    initialized = true;

    // return status
    return success;
}

enum ModelType_t : int
{
    MODEL_SUN,
    MODEL_CLOUDS,
    MODEL_EFFECTS,
    MODEL_OTHER,
};

int GetModelType(const std::string_view& name)
{
    if (name.find(("sun")) != std::string::npos)
        return MODEL_SUN;

    if (name.find(("clouds")) != std::string::npos)
        return MODEL_CLOUDS;

    if (name.find(("effects")) != std::string::npos)
        return MODEL_EFFECTS;

    return MODEL_OTHER;
}


static bool set_night = false;
Color_t backup_light_color = Color_t(255, 255, 255, 255); // Store the original light color
void* __fastcall hooks::hkUpdateSceneObject(C_AggregateSceneObject* object, void* unk) {
    static auto original = m_update_scene_object.GetOriginal();
    auto result = original(object, unk);

    if (!initialized)
        return result;

    if (g::m_unloading)
        return result;

    if (!sdk::m_engine->is_valid())
        return result;

    if (object) {
        if (cfg_get(bool, g_cfg.night_mode)) {
            Color_t colors = cfg_get(ColorPickerVar_t, g_cfg.night_mode_col).colValue;

            for (int i = 0; i < object->m_nCount; i++) {
                object->m_pData[i].r = colors.r;
                object->m_pData[i].g = colors.g;
                object->m_pData[i].b = colors.b;
            }
        }
        else {
            // If night mode is disabled, apply the original colors
            for (int i = 0; i < object->m_nCount; i++) {
                object->m_pData[i].r = backup_light_color.r;
                object->m_pData[i].g = backup_light_color.g;
                object->m_pData[i].b = backup_light_color.b;
            }
        }
    }

    return result;
}
void hooks::hkRelativeMouse(void* input_system, bool enabled)
{
    const auto og = hooks::m_relative_mouse.GetOriginal();

    menu::m_wind_active = enabled;

    if (menu::m_opened)
        return og(input_system, false);

    return og(input_system, enabled);
}

ViewMatrix_t* __fastcall hooks::hkViewMatrix(CRenderGameSystem* pRenderGameSystem, IViewRender* pViewRender, ViewMatrix_t* pOutWorldToView, ViewMatrix_t* pOutViewToProjection, ViewMatrix_t* pOutWorldToProjection, ViewMatrix_t* pOutWorldToPixels)
{    // Call the original function
    const auto original = m_view_matrix.GetOriginal();
    if (!initialized)
        return  original(pRenderGameSystem, pViewRender, pOutWorldToView, pOutViewToProjection, pOutWorldToProjection, pOutWorldToPixels);

    // unload may fuck us up, dont run the hook
    if (g::m_unloading)
        return  original(pRenderGameSystem, pViewRender, pOutWorldToView, pOutViewToProjection, pOutWorldToProjection, pOutWorldToPixels);

    if (!sdk::m_engine->is_valid())
        return  original(pRenderGameSystem, pViewRender, pOutWorldToView, pOutViewToProjection, pOutWorldToProjection, pOutWorldToPixels);

    ViewMatrix_t* view_mat = original(pRenderGameSystem, pViewRender, pOutWorldToView, pOutViewToProjection, pOutWorldToProjection, pOutWorldToPixels);
    g::m_view_matrix = *pOutWorldToProjection;
    g::camera_position = pViewRender->view_setup.vecOrigin;


    return view_mat;
}


HRESULT __stdcall hooks::hkPresent(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags)
{
    // get original function
    const auto og = m_present.GetOriginal();

    if (!sdk::m_swap_chain->m_swap_chain)
        return og(swap_chain, sync_interval, flags);

    // check if our swap chain and game swap chain diffear and log it
    if (swap_chain != sdk::m_swap_chain->m_swap_chain)
        CS2_LOG(xorstr_("swap chain pointers arent the same. they should be the same...\n"));

    // unload may fuck us up, dont run the hook
    if (g::m_unloading)
        return  og(swap_chain, sync_interval, flags);

    // recreate it if it's not valid
    if (sdk::m_render_target_view == nullptr)
        render::CreateRenderTarget();

    if (sdk::m_render_target_view)
        sdk::m_device_context->OMSetRenderTargets(1, &sdk::m_render_target_view, nullptr);

    // init render
    if (!render::m_initialized) {
        //blur::InitResources(sdk::m_device);
        if (!render::Init(g::m_hwnd, sdk::m_device, sdk::m_device_context))
        {
            // tell the user and the developer we failed.
            CS2_LOG(xorstr_("Failed to initialize render.\n"));
            return  og(swap_chain, sync_interval, flags);
        }
    }

    // check if we are initialized
    if (!render::m_initialized)
        return og(swap_chain, sync_interval, flags);

    // create a new frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();

    // create the fram
    ImGui::NewFrame();

    visuals::visuals->proccess();

    // render data
    menu::Render();
    // set our render target
    // 
    // render the queued frame
    ImGui::Render();


    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // let the game render now
    return og(swap_chain, sync_interval, flags);
}

HRESULT __fastcall hooks::hkResizeBuffers(IDXGISwapChain* swap_chain, std::uint32_t count, std::uint32_t width, std::uint32_t height, DXGI_FORMAT new_format, std::uint32_t flags)
{
    // get original function
    const auto og = m_resize_buffers.GetOriginal();

    // unload may fuck us up, dont run the hook
    if (g::m_unloading)
        return og(swap_chain, count, width, height, new_format, flags);

    L_PRINT(LOG_WARNING) << "[DirectX11 CRenderSystem] resized buffers, destroying render target";

    render::DestroyRenderTarget();

    // return og value
    return og(swap_chain, count, width, height, new_format, flags);
}

HRESULT __stdcall hooks::hkCreateSwapChain(IDXGIFactory* factory, IUnknown* unknown_device, DXGI_SWAP_CHAIN_DESC* desc_swap_chain, IDXGISwapChain** swap_chain)
{
    // get original function
    const auto og = m_create_swap_chain.GetOriginal();


    // unload may fuck us up, dont run the hook
    if (g::m_unloading)
        return  og(factory, unknown_device, desc_swap_chain, swap_chain);

    // destroy render target
    render::DestroyRenderTarget();

    // let the game create its new swap chain
    return og(factory, unknown_device, desc_swap_chain, swap_chain);

}

long hooks::HkWndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    // unload may fuck us up, dont run the hook
    if (g::m_unloading)
        return ::CallWindowProcW(m_old_wndproc, hwnd, umsg, wparam, lparam);

    // handle render
    if (render::OnWndProc(hwnd, umsg, wparam, lparam))
        return 1L;

    // run game's code
    return ::CallWindowProcW(m_old_wndproc, hwnd, umsg, wparam, lparam);
}

bool __fastcall hooks::hkMouseInputEnabled(void* rcx)
{
    // get original function
    const auto og = m_mouse_input_enabled.GetOriginal();

    // unload may fuck us up, dont run the hook
    if (g::m_unloading)
        return og(rcx);

    return menu::m_opened ? false : og(rcx);
}

enum c_stages : std::uint32_t
{
    frame_handler_start = 0,
    frame_handler_end = 1,
    frame_prediction = 3,
    frame_net_full_frame_update_on_remove = 2,
    frame_net_update_start = 4,
    frame_net_update_postdataupdate_start = 5,
    frame_net_update_postdataupdate_end = 6,
    frame_net_update_end = 7,
    frame_start = 8,
    frame_render = 9
};

void __fastcall hooks::hkFrameStageNotify(void* a1, int stage)
{
    // get original function
    const auto og = m_frame_stage_notify.GetOriginal();

    if (!initialized)
        return og(a1, stage);

    // unload may fuck us up, dont run the hook
    if (g::m_unloading)
        return og(a1, stage);

    auto local = hooks::m_get_local_player(0);
    if (!local)
        return og(a1, stage);
    // call og 


    g::interval = 0.015625f;

    switch (stage) {

    case frame_net_update_postdataupdate_end:
        /* run skin changer */
        visuals::visuals->cache();
        break;
    default:
        break;
    }

    return m_frame_stage_notify.GetOriginal()(a1, stage);
}

