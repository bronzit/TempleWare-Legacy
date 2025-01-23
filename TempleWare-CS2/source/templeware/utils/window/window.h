#pragma once
#include "../math/vector/vector.h"

class WindowInfo {
public:
	void init();

	void updateWindowSize();

	Vector_t getWindowSize() const;
private:
	Vector_t windowSize;
};

extern WindowInfo windowInfo;