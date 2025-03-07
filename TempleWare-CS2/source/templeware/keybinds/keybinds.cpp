#include "keybinds.h"
#include <Windows.h>

#include "../../../external/imgui/imgui.h"
#include "../config/config.h"

Keybind::Keybind(bool& v, int k)
    : var(v), key(k), isListening(false), skipFrame(false) {}

Keybinds::Keybinds() {
	keybinds.emplace_back(Keybind(Config::aimbot, VK_INSERT));
}

void Keybinds::pollInputs() {
	for (Keybind& k : keybinds) {
		if (GetAsyncKeyState(k.key) & 1) {
			k.var = !k.var;
		}
	}
}

void Keybinds::menuButton(bool& var) {
    for (auto& kb : keybinds) {
        if (&kb.var != &var) continue;

        if (!kb.isListening) {
            if (ImGui::Button("Bind")) {
                kb.isListening = true;
                kb.skipFrame = true;
            }
        }
        else {
            ImGui::Text("Press any key (ESC to cancel)...");
            ImGui::SameLine();

            if (ImGui::Button("Cancel") || (GetAsyncKeyState(VK_ESCAPE) & 1)) {
                kb.isListening = false;
                return;
            }

            if (!kb.skipFrame) {
                for (int keyCode = 7; keyCode < 256; ++keyCode) {
                    if (GetAsyncKeyState(keyCode)) {
                        kb.key = keyCode;
                        kb.isListening = false;
                        return;
                    }
                }
            }
            else {
                kb.skipFrame = false;
            }
        }
    }
}


Keybinds keybind;