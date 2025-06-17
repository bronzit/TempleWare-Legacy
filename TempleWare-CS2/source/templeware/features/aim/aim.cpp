#include "../../../cs2/entity/C_CSPlayerPawn/C_CSPlayerPawn.h"
#include "../../../templeware/interfaces/CGameEntitySystem/CGameEntitySystem.h"
#include "../../../templeware/interfaces/interfaces.h"
#include "../../../templeware/hooks/hooks.h"
#include "../../../templeware/config/config.h"

#include <chrono>
#include <Windows.h>

// Literally the most autistic code ive ever written in my life
// Please dont ever make me do this again

Vector_t GetEntityEyePos(const C_CSPlayerPawn* Entity) {
    if (!Entity)
        return {};

    uintptr_t game_scene_node = *reinterpret_cast<uintptr_t*>((uintptr_t)Entity + SchemaFinder::Get(hash_32_fnv1a_const("C_BaseEntity->m_pGameSceneNode")));

    uintptr_t model = *reinterpret_cast<uintptr_t*>(game_scene_node + SchemaFinder::Get(hash_32_fnv1a_const("CSkeletonInstance->m_modelState")) + 0x80);
    auto head = *reinterpret_cast<Vector_t*>(model + static_cast<unsigned long long>(6) * 32);

    Vector_t result = head;
    if (!std::isfinite(Result.x) || !std::isfinite(Result.y) || !std::isfinite(Result.z))
        return {};

    return Result;
}

inline QAngle_t CalcAngles(Vector_t viewPos, Vector_t aimPos)
{
    QAngle_t angle = { 0, 0, 0 };

    Vector_t delta = aimPos - viewPos;

    angle.x = -asin(delta.z / delta.Length()) * (180.0f / 3.141592654f);
    angle.y = atan2(delta.y, delta.x) * (180.0f / 3.141592654f);

    return angle;
}

inline float GetFov(const QAngle_t& viewAngle, const QAngle_t& aimAngle)
{
    QAngle_t delta = (aimAngle - viewAngle).Normalize();

    return sqrtf(powf(delta.x, 2.0f) + powf(delta.y, 2.0f));
}

void Aimbot() {
    if (!Config::aimbot)
        return;

    int nMaxHighestEntity = I::GameEntity->Instance->GetHighestEntityIndex();

    C_CSPlayerPawn* lp = H::oGetLocalPlayer(0);
    Vector_t lep = GetEntityEyePos(lp);

    QAngle_t* viewangles = (QAngle_t*)(modules.getModule("client") + 0x1A75620); // If aimbot stops working, then you should update this offset

    for (int i = 1; i <= nMaxHighestEntity; i++) {
        auto Entity = I::GameEntity->Instance->Get(i);
        if (!Entity)
            continue;

        if (!Entity->handle().valid())
            continue;

        SchemaClassInfoData_t* _class = nullptr;
        Entity->dump_class_info(&_class);
        if (!_class)
            continue;

        const uint32_t hash = HASH(_class->szName);

        if (hash == HASH("C_CSPlayerPawn")) {
            C_CSPlayerPawn* pawn = (C_CSPlayerPawn*)Entity;

            if (pawn->get_entity_by_handle() == lp->get_entity_by_handle())
                continue;

            if (pawn->getHealth() < 1)
                continue;

            if (!Config::team_check && pawn->getTeam() == lp->getTeam())
                continue;

            Vector_t eye_pos = GetEntityEyePos(pawn);
            QAngle_t angle = CalcAngles(eye_pos, lep);

            angle.x *= -1.f;
            angle.y += 180.f;

            const float fov = GetFov(*viewangles, angle);
            if (!std::isfinite(fov) || fov > Config::aimbot_fov)
                continue;

            QAngle_t ang_punch_angle = *(QAngle_t*)((uintptr_t)lp + SchemaFinder::Get(hash_32_fnv1a_const("C_CSPlayerPawn->m_aimPunchAngle")));

            if (Config::rcs)
                angle -= ang_punch_angle * 2.f;

            angle.z = 0.f;
            angle = angle.Normalize();
            *viewangles = angle;
            break;
        }
    }
}
