#pragma once

#include "../../../../cs2/datatypes/viewmatrix/viewmatrix.h"
#include "../vector/vector.h"

class ViewMatrix {
public:
	//Vector_t WorldToScreen(const Vector_t& position) const;
	bool WorldToScreen(const Vector_t& position, Vector_t& out) const;
	
	viewmatrix_t* viewMatrix;

};