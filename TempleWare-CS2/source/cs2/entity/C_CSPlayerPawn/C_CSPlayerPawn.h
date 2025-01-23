#pragma once
#include "../../../templeware/utils/memory/memorycommon.h"
#include "../../../templeware/utils/math/vector/vector.h"
#include <cstdint>

class C_CSPlayerPawn {
public:
	C_CSPlayerPawn(uintptr_t address);

	Vector_t getPosition() const;
	Vector_t getEyePosition() const;

	uintptr_t getAddress() const;
private:
	uintptr_t address;
};