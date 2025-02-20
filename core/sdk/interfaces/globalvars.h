#pragma once
#include <cstdint>


class CGlobalVarsBase {
public:
    float m_realtime; //0x0000
    int m_framecount; //0x0004
    float m_absolute_frame_time; //0x0008
    float m_absolute_frame_start_time_std_dev; //0x000C
    int m_max_clients; //0x0010
    std::byte pad_0014[28]; //0x0014
    float m_frame_time; //0x0030
    float m_curtime; //0x0034
    float m_curtime2; //0x0038
    char pad_003C[20];
    std::int32_t m_tickcount; //0x0050
    char pad_0054[292]; //0x0054
    uint64_t m_current_map; //0x0178
    uint64_t m_current_mapname; //0x0180
};