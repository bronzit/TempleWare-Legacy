#pragma once
#include <vector>

struct Keybind {
    bool& var;
    int   key;
    bool  isListening;
    bool  skipFrame;

    Keybind(bool& v, int k = 0);
};


class Keybinds {
public:
	Keybinds();
	void pollInputs();

	void menuButton(bool& var);
private:
	std::vector<Keybind> keybinds;
};

extern Keybinds keybind;