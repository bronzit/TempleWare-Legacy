#include "menu.h"
#include "..\render.h"

#include "../../../deps/xorstr/xorstr.h"

#include "menu.h"
#include "../../../deps/imgui/imgui_internal.h"
#include "..\..\..\helpers\crt\crt.h"
#include "../../configs/config.h"

#include "../../../cheats/visuals/preview/preview.h"
#include <imgui.h>
#include <examples/example_win32_directx11/font.h>
#include <stdio.h>
#include <stdint.h>
#include <d3d11.h>
#include <stdlib.h>
#include <assert.h>
#include "..\..\..\helpers\tfm\tinyformat.h"
#include <format>
#include <examples/example_win32_directx11/font.h>
#include <examples/example_win32_directx11/textures.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include <comdef.h>
#include <examples/example_win32_directx11/font.h>

#include <tchar.h>
#include <examples/example_win32_directx11/image.h>

#include <D3DX11tex.h>
#pragma comment(lib, "D3DX11.lib")

using namespace visuals::preview;
#define CS_CONFIGURATION_FILE_EXTENSION ".bin"
#include "..\..\sdk\addresses\entity.h"

using namespace ImGui;

std::string description_test = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.";

void apply_theme() {
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.0f);
	colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.0f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.0f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.0f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.0f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.09f, 0.09f, 0.09f, 1.0f);

	colors[ImGuiCol_Button] = ImVec4(0.60f, 0.30f, 0.80f, 1.0f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.70f, 0.40f, 0.90f, 1.0f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.50f, 0.20f, 0.70f, 1.0f);

	colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.50f, 1.00f, 1.0f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.60f, 0.30f, 0.80f, 1.0f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.70f, 0.40f, 0.90f, 1.0f);

	colors[ImGuiCol_Header] = ImVec4(0.60f, 0.30f, 0.80f, 1.0f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.40f, 0.90f, 1.0f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.50f, 0.20f, 0.70f, 1.0f);

	colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.20f, 0.70f, 1.0f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.30f, 0.80f, 1.0f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.40f, 0.90f, 1.0f);

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);

	style.WindowRounding = 0.0f;
	style.FrameRounding = 0.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabRounding = 0.0f;

	style.ItemSpacing = ImVec2(10, 5);
	style.FramePadding = ImVec2(5, 5);
}

bool render::Init(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* device_context)
{
	// check if we loaded already once
	if (m_initialized)
		return true;

	// create imgui context
	ImGui::CreateContext();

	// setup platform
	if (!ImGui_ImplWin32_Init(hwnd))
		return false;

	// setup renderer bindings
	if (!ImGui_ImplDX11_Init(device, device_context))
		return false;
	//post_request_with_hwid(1);
	// get imgui style
	ImGuiStyle& style = ImGui::GetStyle();

	style.AntiAliasedLines = true;
	style.AntiAliasedFill = true;

	apply_theme();

	// get imgui io
	ImGuiIO& io = ImGui::GetIO();

	// tell imgui to keep the default font for GUI
	io.Fonts->AddFontDefault();

	// create font config
	ImFontConfig font_config;
	font_config.PixelSnapH = false;
	font_config.FontDataOwnedByAtlas = false;
	font_config.OversampleH = 5;
	font_config.OversampleV = 5;
	font_config.RasterizerMultiply = 1.2f;

	static const ImWchar ranges[] = {

		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
		0x2DE0, 0x2DFF, // Cyrillic Extended-A
		0xA640, 0xA69F, // Cyrillic Extended-B
		0xE000, 0xE226, // icons
		0,
	};

	font_config.GlyphRanges = ranges;

	ImFontConfig cfg;

	m_default_font = io.Fonts->AddFontFromFileTTF(xorstr_("C:\\Windows\\Fonts\\Tahoma.ttf"), 14.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
	render::weapon_icons = io.Fonts->AddFontFromFileTTF(("C:\\Windows\\Fonts\\segoeui.ttf"), 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	m_esp_name = io.Fonts->AddFontFromFileTTF(xorstr_("C:\\Windows\\Fonts\\Verdana.ttf"), 10.f, &cfg, io.Fonts->GetGlyphRangesDefault());
	m_esp_weapons = io.Fonts->AddFontFromFileTTF(xorstr_("C:\\Windows\\Fonts\\Verdana.ttf"), 9.f, &cfg, io.Fonts->GetGlyphRangesDefault());
	m_esp_flags = io.Fonts->AddFontFromFileTTF(xorstr_("C:\\Windows\\Fonts\\Verdana.ttf"), 9.f, &cfg, io.Fonts->GetGlyphRangesDefault());

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	m_indicator_font = io.Fonts->AddFontFromFileTTF(xorstr_("C:\\Windows\\Fonts\\Tahoma.ttf"), 14.f, &font_config, io.Fonts->GetGlyphRangesCyrillic());

	// build fonts
	m_initialized = io.Fonts->Build();

	// return status
	return m_initialized;
}
void render::Destroy()
{
	// check if we initialized
	if (!m_initialized)
		return;

	// destroy imgui
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// mark as uninitialized
	m_initialized = false;
}

void render::CreateRenderTarget()
{
	// attempt to get the device from the swap chain
	if (FAILED(sdk::m_swap_chain->m_swap_chain->GetDevice(__uuidof(ID3D11Device), (void**)&sdk::m_device)))
	{
		// log that we failed to get device from swap chain
		CS2_LOG(xorstr_("failed to get device from swapchain while creating render target\n"));
	}

	else
	{
		// successfully got the device, so get the device context
		sdk::m_device->GetImmediateContext(&sdk::m_device_context);
	}

	// lambda function to get the correct DXGI format
	static const auto get_correct_dxgi_format = [](DXGI_FORMAT current_frame)
	{
		switch (current_frame)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		}

		return current_frame;
	};

	DXGI_SWAP_CHAIN_DESC sd{ };

	// retrieve the description of the swap chain
	sdk::m_swap_chain->m_swap_chain->GetDesc(&sd);

	ID3D11Texture2D* back_buffer = nullptr;

	// attempt to get the back buffer from the swap chain
	if (SUCCEEDED(sdk::m_swap_chain->m_swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer))))
	{
		if (back_buffer)
		{
			D3D11_RENDER_TARGET_VIEW_DESC desc{ };

			// set gxgi format
			desc.Format = static_cast<DXGI_FORMAT>(get_correct_dxgi_format(sd.BufferDesc.Format));
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

			// attempt to create a render target view from the back buffer
			if (FAILED(sdk::m_device->CreateRenderTargetView(back_buffer, &desc, &sdk::m_render_target_view)))
			{
				// try with multisampled texture if failed
				desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
				if (FAILED(sdk::m_device->CreateRenderTargetView(back_buffer, &desc, &sdk::m_render_target_view)))
				{
					// if still failed, create render target view with default parameters
					if (FAILED(sdk::m_device->CreateRenderTargetView(back_buffer, NULL, &sdk::m_render_target_view)))
					{
						// at this point kill yourself
						CS2_LOG(xorstr_("failed to get render target view after multiple attempts.\n"));
					}
				}
			}

			// release back buffer
			back_buffer->Release();

			// invalidate back buffer
			back_buffer = nullptr;
		}
	}
}

void render::DestroyRenderTarget()
{
	// check if render view is valid
	if (sdk::m_render_target_view != nullptr)
	{
		// release render view
		sdk::m_render_target_view->Release();

		// invalidate render view
		sdk::m_render_target_view = nullptr;
	}
}

void render::NewFrame()
{
	// start a new frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	// create the fram
	ImGui::NewFrame();
}

void render::Render()
{
	// end frame and add to queue
	ImGui::EndFrame();

	// render the queued frame
	ImGui::Render();

	// render the queued draw data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

DWORD picker_flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview;
void colorpicker(const char* name, Color_t* color, ImGuiColorEditFlags flags) {
	float arrColor[4];
	color->BaseAlpha(arrColor);
	if (ImGui::ColorEdit4(name, arrColor, picker_flags));
	{
		*color = Color_t::FromBase4(arrColor);
	}
}
int  activeTab = 0;

void menu::Render()
{
	// update menu accent
	CEconItemSchema* pItemSchema = sdk::m_client->GetEconItemSystem()->GetEconItemSchema();

	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle* style = &ImGui::GetStyle();

	if (!m_opened)
		return;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar;

	ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_Once);

	ImGui::Begin("TempleWare | Internal", nullptr, window_flags);

	{
		float windowWidth = ImGui::GetWindowWidth();
		float leftTextWidth = ImGui::CalcTextSize("TempleWare - Internal").x;
		float rightTextWidth = ImGui::CalcTextSize("templecheats.xyz").x;

		ImGui::Text("TempleWare - Internal");

		ImGui::SameLine(windowWidth - rightTextWidth - 10);
		ImGui::Text("templecheats.xyz");
	}

	ImGui::Separator();
	ImGui::Spacing();

	{

		ImVec2 buttonSize(80, 0);

		if (ImGui::Button("Aim", buttonSize))       activeTab = 0; ImGui::SameLine();
		if (ImGui::Button("Visuals", buttonSize))   activeTab = 1; ImGui::SameLine();
		if (ImGui::Button("Misc", buttonSize))      activeTab = 2; ImGui::SameLine();
		if (ImGui::Button("Config", buttonSize))    activeTab = 3;
	}

	ImGui::Separator();
	ImGui::Spacing();

	ImGui::BeginChild("Main Content", ImVec2(0, 0), true);
	switch (activeTab) {
	case 0:
		ImGui::Text("Aim");
		ImGui::Separator();

		break;

	case 1:
		ImGui::Text("Visuals");
		ImGui::Separator();
		if (ImGui::CollapsingHeader("ESP")) {
			ImGui::Checkbox("Enabled", &cfg_get(bool, g_cfg.bVisualOverlay));
			ImGui::Checkbox("Name", &cfg_get(TextOverlayVar_t, g_cfg.overlayName).bEnable);
			ImGui::Checkbox("Skeleton", &cfg_get(bool, g_cfg.bSkeleton));
			ImGui::Checkbox("Box", &cfg_get(FrameOverlayVar_t, g_cfg.overlayBox).bEnable);
			ImGui::Checkbox("Health", &cfg_get(FrameOverlayVar_t, g_cfg.overlayHealthBar).bEnable);
			ImGui::Checkbox("Weapon", &cfg_get(TextOverlayVar_t, g_cfg.Weaponesp).bEnable);
		}
		ImGui::Separator();

		break;

	case 2:
		ImGui::Text("Misc");
		ImGui::Separator();
		if (ImGui::CollapsingHeader("World")) {
			ImGui::Checkbox("Night", &cfg_get(bool, g_cfg.night_mode));
			if (cfg_get(bool, g_cfg.night_mode))
				colorpicker("Night Color", &cfg_get(ColorPickerVar_t, g_cfg.night_mode_col).colValue, 0);
			
			ImGui::Checkbox("Props", &cfg_get(bool, g_cfg.props));
			if (cfg_get(bool, g_cfg.props)) 
				colorpicker("Props Color", &cfg_get(ColorPickerVar_t, g_cfg.props_col).colValue, 0);
		}
		break;

	case 3:
	
		break;
	}
	ImGui::EndChild();

	ImGui::End();

}

