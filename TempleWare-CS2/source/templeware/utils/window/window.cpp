#include "window.h"
#include "../memory/patternscan/patternscan.h"

#include <iostream>

void WindowInfo::updateWindowSize() {
	uintptr_t pX = M::patternScan("client", "8B 05 ? ? ? ? 89 07");
	uintptr_t pY = M::patternScan("client", "8B 05 ? ? ? ? 89 03");
	std::cout << pX << "\n";
	std::cout << pY << "\n";

	if (!pX || !pY) {
		windowSize.x = 1920;
		windowSize.y = 1080;
	}
	else {
		windowSize.x = *(int*)pX;
		windowSize.y = *(int*)pY;
	}


	windowSize.z = 0;
}

void WindowInfo::init() {
	updateWindowSize();
}

Vector_t WindowInfo::getWindowSize() const {
	return windowSize;
}

WindowInfo windowInfo;