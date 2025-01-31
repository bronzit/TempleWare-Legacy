#pragma once

#include "../menu/menu.h"
#include "../visuals/visuals.h"
#include "../menu/hud.h"

class Renderer {
public:
	Menu menu;
	Visuals visuals;
	Hud hud;
};