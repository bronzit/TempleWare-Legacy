#include "patternscan.h"

#include "../../module/module.h"

#include <Windows.h>
#include <vector>
#include <string>
#include <iostream>
#include <Psapi.h>

std::vector<std::pair<uint8_t, bool>> PatternToBytes(const std::string& pattern) {
    std::vector<std::pair<uint8_t, bool>> patternBytes;
    const char* start = pattern.c_str();
    const char* end = start + pattern.size();

    for (const char* current = start; current < end; ++current) {
        if (*current == ' ') continue;
        if (*current == '?') {
            patternBytes.emplace_back(0, false);
            if (*(current + 1) == '?') ++current;
        }
        else {
            patternBytes.emplace_back(strtoul(current, nullptr, 16), true);
            if (*(current + 1) != ' ') ++current;
        }
    }

    return patternBytes;
}

uintptr_t M::patternScan(const std::string& module, const std::string& pattern) {
    uintptr_t baseAddress = modules.getModule(module);
    HMODULE hModule = reinterpret_cast<HMODULE>(baseAddress);

    if (!hModule) return 0;

    MODULEINFO moduleInfo;
    GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(MODULEINFO));

    size_t moduleSize = moduleInfo.SizeOfImage;

    std::vector<std::pair<uint8_t, bool>> patternBytes = PatternToBytes(pattern);
    size_t patternLength = patternBytes.size();

    for (size_t i = 0; i < moduleSize - patternLength; ++i) {
        bool found = true;
        for (size_t j = 0; j < patternLength; ++j) {
            if (patternBytes[j].second && patternBytes[j].first != *reinterpret_cast<uint8_t*>(baseAddress + i + j)) {
                found = false;
                break;
            }
        }
        if (found) {
            return baseAddress + i;
        }
    }

    return 0;
}