#pragma once
#include <cstdint>

namespace pioneer
{
struct MemoryMap
{
    struct int_control
    {
        int32_t x_force;
        int32_t y_force;
        int32_t z_force;
        int32_t x_torque;
        int32_t y_torque;
        int32_t z_torque;
    } __attribute__((__packed__)) int_control_;
    int32_t board_id;
    int32_t thruster_speed_0;
    int32_t thruster_speed_1;
    int32_t thruster_speed_2;
    int32_t thruster_speed_3;
    int32_t thruster_speed_4;
    int32_t thruster_speed_5;
} __attribute__((__packed__));
} // namespace pioneer
