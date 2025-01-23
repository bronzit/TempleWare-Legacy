#pragma once
#include "../utils/math/viewmatrix/viewmatrix.h"

class Visuals {
public:
	void init();

	void esp();
private:
	ViewMatrix viewMatrix;
};