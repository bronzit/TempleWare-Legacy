#include "CCSPlayerController.h"

CCSPlayerController::CCSPlayerController(uintptr_t address) : address(address) {}

uintptr_t CCSPlayerController::getAddress() const {
	return address;
}
