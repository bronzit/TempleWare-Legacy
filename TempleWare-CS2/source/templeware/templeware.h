#pragma once
#include "renderer/renderer.h"
#include "config/config.h"

class TempleWare {
public:
	void init(HWND& window, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ID3D11RenderTargetView* mainRenderTargetView);

	Renderer renderer;
};