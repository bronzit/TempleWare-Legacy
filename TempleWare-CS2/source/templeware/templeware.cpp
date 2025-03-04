#include "templeware.h"

#include "utils/module/module.h"

#include <iostream>

void TempleWare::init(HWND& window, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ID3D11RenderTargetView* mainRenderTargetView) {
	std::cout << "Initializing modules...\n";
	modules.init();

	std::cout << "Initializing menu...\n";
	renderer.menu.init(window, pDevice, pContext, mainRenderTargetView);
	std::cout << "Initializing visuals...\n";
	renderer.visuals.init();

	std::cout << "Initializing hooks...\n";
	hooks.init();
}