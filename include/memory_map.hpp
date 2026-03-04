#pragma once

#include <cstdint>
namespace pioneer
{
struct MemoryMap
{
    int connection_test;
    int thruster_speed_0;
    int thruster_speed_1;
    int thruster_speed_2;
    int thruster_speed_3;
    int thruster_speed_4;
    int thruster_speed_5;
} __attribute__((__packed__));
} // namespace pioneer
