#pragma once
#include <cstdint>

class CCSPlayerController {
public:
	CCSPlayerController(uintptr_t address);

	uintptr_t getAddress() const;
private:
	uintptr_t address;
};