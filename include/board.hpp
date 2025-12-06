#pragma once

#include "hydrolib_command_map.hpp"
#include "hydrolib_device_manager.hpp"
#include "hydrolib_shell.hpp"
#include "hydrolib_stream_device.hpp"
#include "hydrv_clock.hpp"
#include "hydrv_uart.hpp"

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

public:
    void RunShell();

private:
    static inline constinit hydrv::clock::Clock clock_{
        hydrv::clock::Clock::HSI_DEFAULT};

    static inline constinit hydrv::GPIO::GPIOLow rx_pin1_{
        hydrv::GPIO::GPIOLow::GPIOB_port, 7,
        hydrv::GPIO::GPIOLow::GPIO_UART_RX};
    static inline constinit hydrv::GPIO::GPIOLow tx_pin1_{
        hydrv::GPIO::GPIOLow::GPIOB_port, 6,
        hydrv::GPIO::GPIOLow::GPIO_UART_TX};
    static inline constinit hydrv::UART::UART<255, 255> uart1_{
        hydrv::UART::UARTLow::USART1_115200_LOW, rx_pin1_, tx_pin1_, 7};

    static inline hydrolib::device::StreamDevice uart1_device_{"uart1", uart1_};

    static inline constinit hydrv::GPIO::GPIOLow rx_pin3_{
        hydrv::GPIO::GPIOLow::GPIOB_port, 11,
        hydrv::GPIO::GPIOLow::GPIO_UART_RX};
    static inline constinit hydrv::GPIO::GPIOLow tx_pin3_{
        hydrv::GPIO::GPIOLow::GPIOB_port, 10,
        hydrv::GPIO::GPIOLow::GPIO_UART_TX};
    static inline constinit hydrv::UART::UART<255, 255> uart3_{
        hydrv::UART::UARTLow::USART3_115200_LOW, rx_pin3_, tx_pin3_, 7};

    static inline hydrolib::device::DeviceManager device_manager_{
        &uart1_device_};

    static inline hydrolib::shell::Shell<
        decltype(uart3_), hydrolib::shell::CommandMap::CommandType,
        hydrolib::shell::CommandMap>
        shell_{uart3_, hydrolib::shell::command_map};
};

inline Board::Board()
{
    clock_.Init();
    NVIC_SetPriorityGrouping(0);
    uart1_.Init();
    uart3_.Init();
}

inline void Board::RunShell()
{
    while (1)
    {
        shell_.Process();
    }
}

} // namespace pioneer
