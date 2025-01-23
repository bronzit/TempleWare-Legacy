#pragma once
#include <cstdint>
#include <vector>
#include <string>

namespace M {
	uintptr_t patternScan(const std::string& module, const std::string& pattern);
}