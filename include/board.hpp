#pragma once

#include "hydrolib_bus_datalink_stream.hpp"
#include "hydrolib_command_map.hpp"
#include "hydrolib_device_manager.hpp"
#include "hydrolib_shell.hpp"
#include "hydrolib_stream_device.hpp"
#include "hydrv_clock.hpp"
#include "hydrv_rs_485.hpp"
#include "hydrv_shell_uart.hpp"
#include "hydrv_thruster.hpp"
#include "hydrv_tim_low.hpp"

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

    void RunShell();

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
        stream_manager_{0x01, rs485_1_, kLoggerStab};
    static inline hydrolib::bus::datalink::Stream shore_stream_{stream_manager_,
                                                                0x02};

    static inline hydrolib::device::StreamDevice shore_stream_device_{
        "shore_stream", shore_stream_};

    static inline constinit hydrv::GPIO::GPIOLow rx_pin3_{
        hydrv::GPIO::GPIOLow::GPIOB_port, 11,
        hydrv::GPIO::GPIOLow::GPIO_UART_RX};
    static inline constinit hydrv::GPIO::GPIOLow tx_pin3_{
        hydrv::GPIO::GPIOLow::GPIOB_port, 10,
        hydrv::GPIO::GPIOLow::GPIO_UART_TX};
    static inline constinit hydrv::UART::ShellUART<255, 255> uart3_{
        hydrv::UART::UARTLow::USART3_115200_LOW, rx_pin3_, tx_pin3_, 7};

    static inline constinit hydrv::GPIO::GPIOLow tim_pin_{
        hydrv::GPIO::GPIOLow::GPIOA_port, 0, hydrv::GPIO::GPIOLow::GPIO_Timer};
    static inline constinit hydrv::timer::TimerLow tim_{
        hydrv::timer::TimerLow::TIM5_low,
        hydrv::thruster::Thruster::tim_prescaler,
        hydrv::thruster::Thruster::tim_counter_period};
    static inline constinit hydrv::thruster::Thruster thruster_{0, tim_,
                                                                tim_pin_};
    static inline hydrolib::device::ThrusterDevice thruster_device{"thruster",
                                                                   thruster_};

    static inline hydrolib::device::DeviceManager device_manager_{
        &shore_stream_device_, &rs485_1_device_, &thruster_device};

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
    thruster_.Init();
}

inline void Board::RunShell()
{
    while (1)
    {
        shell_.Process();
    }
}

} // namespace pioneer
