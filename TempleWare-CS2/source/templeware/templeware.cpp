#include "templeware.h"

void TempleWare::init(HWND& window, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ID3D11RenderTargetView* mainRenderTargetView) {
	renderer.menu.init(window, pDevice, pContext, mainRenderTargetView);
}

