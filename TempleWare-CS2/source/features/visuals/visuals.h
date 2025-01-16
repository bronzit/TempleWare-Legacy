#pragma once
#include "math.h"
#include "../../../external/imgui/imgui.h"
#include "../../offsets/offsets.h"

class Visuals {
public:
    void PlayerESP();
    void PlayerNameTags();
    void PlayerTracers();
    void PlayerGlow();

private:
    Offset offsets_;
};

