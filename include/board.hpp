#pragma once

#include "hydrolib_bus_application_master.hpp"
#include "hydrolib_bus_datalink_stream.hpp"
#include "hydrolib_command_map.hpp"
#include "hydrolib_device_manager.hpp"
#include "hydrolib_fixed_point.hpp"
#include "hydrolib_return_codes.hpp"
#include "hydrolib_shell.hpp"
#include "hydrolib_stream_device.hpp"
#include "hydrolib_thrust_generator.hpp"
#include "hydrv_clock.hpp"
#include "hydrv_rs_485.hpp"
#include "hydrv_shell_uart.hpp"
#include "hydrv_thruster.hpp"
#include "hydrv_tim_low.hpp"

#include "memory_map.hpp"
#include <chrono>
#include <cstdint>
#include <cstring>

extern "C"
{
    void SysTick_Handler(void);
    void USART3_IRQHandler(void);
    void USART1_IRQHandler(void);
    void HardFault_Handler(void);
}

namespace pioneer
{
class Board
{
    friend void ::SysTick_Handler(void);
    friend void ::USART3_IRQHandler(void);
    friend void ::USART1_IRQHandler(void);

public:
    Board();

    void RunExample();

private:
    static constexpr void *kLoggerStab = nullptr;

    static inline constinit hydrv::GPIO::GPIOLow rx_pin1_{
        hydrv::GPIO::GPIOLow::GPIOB_port, 7,
        hydrv::GPIO::GPIOLow::GPIO_UART_RX};
    static inline constinit hydrv::GPIO::GPIOLow tx_pin1_{
        hydrv::GPIO::GPIOLow::GPIOB_port, 6,
        hydrv::GPIO::GPIOLow::GPIO_UART_TX};
    static inline constinit hydrv::GPIO::GPIOLow direction_pin1_{
        hydrv::GPIO::GPIOLow::GPIOB_port, 5, hydrv::GPIO::GPIOLow::GPIO_Output};
    static inline constinit hydrv::RS485::RS485<255, 255> rs485_1_{
        hydrv::UART::UARTLow::USART1_115200_LOW, rx_pin1_, tx_pin1_,
        direction_pin1_, 7};

    static inline hydrolib::device::StreamDevice<decltype(rs485_1_)>
        rs485_1_device_{"rs485", rs485_1_};

    static inline constinit hydrolib::bus::datalink::StreamManager
        stream_manager_{2, rs485_1_, kLoggerStab};
    static inline hydrolib::bus::datalink::Stream shore_stream_{stream_manager_,
                                                                3};

    static inline hydrolib::device::StreamDevice shore_stream_device_{
        "shore_stream", shore_stream_};
    static inline hydrolib::bus::application::Master master_{shore_stream_,
                                                             kLoggerStab};

    static inline constinit hydrv::GPIO::GPIOLow rx_pin3_{
        hydrv::GPIO::GPIOLow::GPIOB_port, 11,
        hydrv::GPIO::GPIOLow::GPIO_UART_RX};
    static inline constinit hydrv::GPIO::GPIOLow tx_pin3_{
        hydrv::GPIO::GPIOLow::GPIOB_port, 10,
        hydrv::GPIO::GPIOLow::GPIO_UART_TX};
    static inline constinit hydrv::UART::ShellUART<255, 255> uart3_{
        hydrv::UART::UARTLow::USART3_115200_LOW, rx_pin3_, tx_pin3_, 7};

    MemoryMap::SystemData system_data_for_write = {
        .vma_statuses = {1, 1, 1, 1, 0, 0, 0, 1, 1, 0},
        .light_status = 0,
        .current_mission = 0,
        .batL_voltage = 1000,
        .batR_voltage = 1250,
        .mission_names = {"mission 1", "mission 2", "mission 3", "mission 4"},
        .error_logs = {"error 1", "error 2", "error 3", ""}};

    MemoryMap::SystemData system_data_for_read = {
        .vma_statuses = {0, 0, 1, -1, -1, -1, -1, -1, -1, -1},
        .light_status = 2,
        .current_mission = 1,
        .batL_voltage = 0,
        .batR_voltage = 0,
        .mission_names = {"", "", "", ""},
        .error_logs = {"", "", "", ""}};

    static inline hydrolib::device::DeviceManager device_manager_{
        &shore_stream_device_, &rs485_1_device_};

    static inline hydrolib::shell::Shell<
        decltype(uart3_), hydrolib::shell::CommandMap::CommandType,
        hydrolib::shell::CommandMap>
        shell_{uart3_, hydrolib::shell::command_map};
};

inline Board::Board()
{
    hydrv::clock::Clock::Init(hydrv::clock::Clock::HSI_DEFAULT);
    NVIC_SetPriorityGrouping(0);
    rs485_1_.Init();
    uart3_.Init();
}

using namespace std::literals::chrono_literals;
inline void Board::RunExample()
{
    shell_.Process();
    static constexpr auto kRequestTimeout = 5s;
    std::chrono::steady_clock::time_point last_request_time_;
    while (1)
    {
        shell_.Process();
        if (std::chrono::steady_clock::now() - last_request_time_ >
            kRequestTimeout)
        {
            // ПРИМЕР ДЛЯ ЗАПИСИ
            /*master_.Write(&system_data_for_write, 0,
                          sizeof(MemoryMap::SystemData));
            stream_manager_.Process();
            master_.Process();*/

            // ПРИМЕР ДЛЯ ЧТЕНИЯ
            master_.Read(&system_data_for_read, 0,
                         sizeof(MemoryMap::SystemData));
            for (int i = 0; i < 10; i++)
            {
                stream_manager_.Process();
                hydrolib::ReturnCode result = master_.Process();
                if (result == hydrolib::ReturnCode::NO_DATA ||
                    result == hydrolib::ReturnCode::TIMEOUT)
                {
                    std::chrono::steady_clock::time_point last_request_time_1 =
                        std::chrono::steady_clock::now();
                    while (std::chrono::steady_clock::now() -
                               last_request_time_1 <
                           10ms)
                    {
                    }
                    continue;
                }
            }
            last_request_time_ = std::chrono::steady_clock::now();
        }
    }
}
} // namespace pioneer
