#include "interfaces.h"
#include "CGameEntitySystem/CGameEntitySystem.h"

// @used: I::Get<template>
#include "..\..\templeware\utils\memory\Interface\Interface.h"
bool I::Interfaces::init()
{
    bool success = true;

    EngineClient = I::Get<IEngineClient>(("engine2.dll"), "Source2EngineToClient00");
    success &= (EngineClient != nullptr);

    GameEntity = I::Get<IGameResourceService>(("engine2.dll"), "GameResourceServiceClientV00");
    success &= (GameEntity != nullptr);

    printf("Source2EngineToClient00: 0x%p\n", reinterpret_cast<void*>(EngineClient));
    printf("GameResourceServiceClientV00: 0x%p\n", reinterpret_cast<void*>(GameEntity));

    // return status
    return success;
}
