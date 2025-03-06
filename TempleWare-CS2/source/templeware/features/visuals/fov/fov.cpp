#include "fov.h"
#include "../../../hooks/hooks.h"
#include "../../../config/config.h"

float H::hkGetRenderFov(void* rcx) {
	if (Config::fovEnabled)
		return Config::fov;
	
	return H::GetRenderFov.GetOriginal()(rcx);
}