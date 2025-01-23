#include "hooks.h"
#include <iostream>

#include "../../../external/kiero/minhook/include/MinHook.h"
#include "../utils/memory/patternscan/patternscan.h"

#include "../players/hook/playerHook.h"

template <typename T>
void registerHook(const std::string& moduleName, const std::string& pattern, void* hookFunction, T originalFunction) {
	uintptr_t functionAddress = M::patternScan(moduleName, pattern);
	if (!functionAddress) {
		std::cout << "\nHook failed.\nHook Pattern: " << pattern << "\n\n";
		return;
	}

	LPVOID fA = reinterpret_cast<LPVOID>(functionAddress);
	LPVOID hF = reinterpret_cast<LPVOID>(hookFunction);
	LPVOID oF = reinterpret_cast<LPVOID*>(originalFunction);

	MH_CreateHook(fA, hF, &oF);
}

void hOnAddEntity() {
	auto pOnAddEntity = M::patternScan("client", "48 89 74 24 ? 57 48 83 EC ? 48 8B F9 41 8B C0 B9");
	if (!pOnAddEntity) {
		std::cout << "onAddEntity hook failed.\n";
		return;
	}
	MH_CreateHook((LPVOID)pOnAddEntity, (LPVOID)onAddEntityHook, (LPVOID*)&oOnAddEntity);
}

void hOnRemoveEntity() {
	auto pOnRemoveEntity = M::patternScan("client", "48 89 74 24 ? 57 48 83 EC ? 48 8B F9 41 8B C0 25");
	if (!pOnRemoveEntity) {
		std::cout << "onRemoveEntity hook failed.\n";
		return;
	}
	MH_CreateHook((LPVOID)pOnRemoveEntity, (LPVOID)onRemoveEntityHook, (LPVOID*)&oOnRemoveEntity);
}

void Hooks::init() {
	hOnAddEntity();
	hOnRemoveEntity();

	// Enable literally every hook, might be a bad idea and could conflict with other things.
	// Most likely won't cause any but if issues ever arise just track state/store hooks and manually enable all of them I guess.
	MH_EnableHook(MH_ALL_HOOKS);
	std::cout << "Enabled hooks\n";
}
