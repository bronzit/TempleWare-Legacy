#include "players.h"
#include "../offsets/offsets.h"

std::vector<CCSPlayerController> Players::controllers;
std::vector<C_CSPlayerPawn> Players::pawns;

C_CSPlayerPawn Players::getLocalPawn() {
    return C_CSPlayerPawn(modules.getModule("client") + Offset::dwLocalPlayerPawn);
}