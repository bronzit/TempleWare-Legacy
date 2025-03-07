#include "../../../hooks/hooks.h"
#include "../../../config/config.h"

float H::hkGetRenderFov(void* rcx) {
	if (Config::fovEnabled) {
		float flTargetFov = Config::fov;
		return flTargetFov;
	}
	
	return H::GetRenderFov.GetOriginal()(rcx);
}