#include "debug.h"
#include <Windows.h>
#include <iostream>

void initDebug() {
    if (AllocConsole()) {
        FILE* pFile;
        freopen_s(&pFile, "CONOUT$", "w", stdout);
        freopen_s(&pFile, "CONOUT$", "w", stderr);
        freopen_s(&pFile, "CONIN$", "r", stdin);
        printf("Console allocated.\n\n");
    } else {
        MessageBoxA(NULL, "Failed to allocate console", "Error", MB_OK | MB_ICONERROR);
    }
}