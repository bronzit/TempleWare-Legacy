#include "../../../hooks/hooks.h"
#include "../../../config/config.h"

void __fastcall H::hkRenderFlashbangOverlay(void* a1, void* a2, void* a3, void* a4, void* a5) {
	if (Config::antiflash) return;
	return RenderFlashBangOverlay.GetOriginal()(a1, a2, a3, a4, a5);
}