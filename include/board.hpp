#pragma once

#include "hydrolib_bus_application_slave.hpp"
#include "hydrolib_bus_datalink_stream.hpp"
#include "hydrolib_command_map.hpp"
#include "hydrolib_device_manager.hpp"
#include "hydrolib_return_codes.hpp"
#include "hydrolib_shell.hpp"
#include "hydrolib_stream_device.hpp"
#include "hydrv_clock.hpp"
#include "hydrv_rs_485.hpp"
#include "hydrv_shell_uart.hpp"
#include "hydrv_thruster.hpp"
#include "hydrv_tim_low.hpp"

#include "memory_map.hpp"
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
    class Memory
    {
    public:
        hydrolib::ReturnCode Read(void *read_buffer, int address, int length);
        hydrolib::ReturnCode Write(const void *write_buffer, int address,
                                   int length);
    };

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
    static inline Memory memory_{};

    static inline hydrolib::device::StreamDevice shore_stream_device_{
        "shore_stream", shore_stream_};
    static inline hydrolib::bus::application::Slave slave_{
        shore_stream_, memory_, kLoggerStab};

    static inline constinit hydrv::GPIO::GPIOLow rx_pin3_{
        hydrv::GPIO::GPIOLow::GPIOB_port, 11,
        hydrv::GPIO::GPIOLow::GPIO_UART_RX};
    static inline constinit hydrv::GPIO::GPIOLow tx_pin3_{
        hydrv::GPIO::GPIOLow::GPIOB_port, 10,
        hydrv::GPIO::GPIOLow::GPIO_UART_TX};
    static inline constinit hydrv::UART::ShellUART<255, 255> uart3_{
        hydrv::UART::UARTLow::USART3_115200_LOW, rx_pin3_, tx_pin3_, 7};

    static inline constinit hydrv::timer::TimerLow tim_{
        hydrv::timer::TimerLow::TIM5_low,
        hydrv::thruster::Thruster::tim_prescaler,
        hydrv::thruster::Thruster::tim_counter_period};

    static inline constinit hydrv::timer::TimerLow tim3_{
        hydrv::timer::TimerLow::TIM3_low,
        hydrv::thruster::Thruster::tim_prescaler,
        hydrv::thruster::Thruster::tim_counter_period};

    static inline constinit hydrv::GPIO::GPIOLow tim_pin_0_{
        hydrv::GPIO::GPIOLow::GPIOA_port, 0, hydrv::GPIO::GPIOLow::GPIO_Timer};
    static inline constinit hydrv::GPIO::GPIOLow tim_pin_1_{
        hydrv::GPIO::GPIOLow::GPIOA_port, 1, hydrv::GPIO::GPIOLow::GPIO_Timer};
    static inline constinit hydrv::GPIO::GPIOLow tim_pin_2_{
        hydrv::GPIO::GPIOLow::GPIOA_port, 2, hydrv::GPIO::GPIOLow::GPIO_Timer};
    static inline constinit hydrv::GPIO::GPIOLow tim_pin_3_{
        hydrv::GPIO::GPIOLow::GPIOA_port, 3, hydrv::GPIO::GPIOLow::GPIO_Timer};
    static inline constinit hydrv::GPIO::GPIOLow tim3_pin_0_{
        hydrv::GPIO::GPIOLow::GPIOB_port, 0, hydrv::GPIO::GPIOLow::GPIO_Timer};
    static inline constinit hydrv::GPIO::GPIOLow tim3_pin_1_{
        hydrv::GPIO::GPIOLow::GPIOB_port, 1, hydrv::GPIO::GPIOLow::GPIO_Timer};

    static inline constinit hydrv::thruster::Thruster thruster_0_{0, tim_,
                                                                  tim_pin_0_};
    static inline constinit hydrv::thruster::Thruster thruster_1_{1, tim_,
                                                                  tim_pin_1_};
    static inline constinit hydrv::thruster::Thruster thruster_2_{2, tim_,
                                                                  tim_pin_2_};
    static inline constinit hydrv::thruster::Thruster thruster_3_{3, tim_,
                                                                  tim_pin_3_};
    static inline constinit hydrv::thruster::Thruster thruster_4_{2, tim3_,
                                                                  tim3_pin_0_};
    static inline constinit hydrv::thruster::Thruster thruster_5_{3, tim3_,
                                                                  tim3_pin_1_};

    static inline hydrolib::device::ThrusterDevice thruster_device_0_{
        "thr0", thruster_0_};
    static inline hydrolib::device::ThrusterDevice thruster_device_1_{
        "thr1", thruster_1_};
    static inline hydrolib::device::ThrusterDevice thruster_device_2_{
        "thr2", thruster_2_};
    static inline hydrolib::device::ThrusterDevice thruster_device_3_{
        "thr3", thruster_3_};
    static inline hydrolib::device::ThrusterDevice thruster_device_4_{
        "thr4", thruster_4_};
    static inline hydrolib::device::ThrusterDevice thruster_device_5_{
        "thr5", thruster_5_};

    static inline hydrolib::device::DeviceManager device_manager_{
        &shore_stream_device_, &rs485_1_device_,    &thruster_device_0_,
        &thruster_device_1_,   &thruster_device_2_, &thruster_device_3_,
        &thruster_device_4_,   &thruster_device_5_};

    static inline hydrolib::shell::Shell<
        decltype(uart3_), hydrolib::shell::CommandMap::CommandType,
        hydrolib::shell::CommandMap>
        shell_{uart3_, hydrolib::shell::command_map};

    static inline int connection_test = 0xABABABAB;
    static inline int thruster_speed_0 = 0;
    static inline int thruster_speed_1 = 0;
    static inline int thruster_speed_2 = 0;
    static inline int thruster_speed_3 = 0;
    static inline int thruster_speed_4 = 0;
    static inline int thruster_speed_5 = 0;
};

inline Board::Board()
{
    hydrv::clock::Clock::Init(hydrv::clock::Clock::HSI_DEFAULT);
    NVIC_SetPriorityGrouping(0);
    rs485_1_.Init();
    uart3_.Init();
    thruster_0_.Init();
    thruster_1_.Init();
    thruster_2_.Init();
    thruster_3_.Init();
    thruster_4_.Init();
    thruster_5_.Init();
}

inline void Board::RunShell()
{
    while (1)
    {
        shell_.Process();
    }
}

inline hydrolib::ReturnCode Board::Memory::Read(void *read_buffer, int address,
                                                int length)
{
    switch (address)
    {
    case offsetof(MemoryMap, connection_test):
    {
        uint32_t status = 0xABABABAB;
        memcpy(read_buffer, &status, sizeof(status));
        break;
    }

    case offsetof(MemoryMap, thruster_speed_0):
    {
        int speed;
        speed = thruster_0_.GetSpeed();
        memcpy(read_buffer, &speed, sizeof(speed));
        break;
    }
    case offsetof(MemoryMap, thruster_speed_1):
    {
        int speed;
        speed = thruster_1_.GetSpeed();
        memcpy(read_buffer, &speed, sizeof(speed));
        break;
    }
    case offsetof(MemoryMap, thruster_speed_2):
    {
        int speed;
        speed = thruster_2_.GetSpeed();
        memcpy(read_buffer, &speed, sizeof(speed));
        break;
    }
    case offsetof(MemoryMap, thruster_speed_3):
    {
        int speed;
        speed = thruster_3_.GetSpeed();
        memcpy(read_buffer, &speed, sizeof(speed));
        break;
    }
    case offsetof(MemoryMap, thruster_speed_4):
    {
        int speed;
        speed = thruster_4_.GetSpeed();
        memcpy(read_buffer, &speed, sizeof(speed));
        break;
    }
    case offsetof(MemoryMap, thruster_speed_5):
    {
        int speed;
        speed = thruster_5_.GetSpeed();
        memcpy(read_buffer, &speed, sizeof(speed));
        break;
    }
    default:
        return hydrolib::ReturnCode::FAIL;
    }
    length -= sizeof(int);
    if (length > 0)
    {

        void *next_read_buffer =
            static_cast<uint8_t *>(read_buffer) + sizeof(int);
        return Read(next_read_buffer, address + sizeof(int), length);
    }
    return hydrolib::ReturnCode::OK;
}

inline hydrolib::ReturnCode Board::Memory::Write(const void *write_buffer,
                                                 int address, int length)
{
    switch (address)
    {
    case offsetof(MemoryMap, connection_test):
    {
        uint32_t status;
        memcpy(&status, write_buffer, sizeof(status));
        if (status != 0xABABABAB)
        {
            return hydrolib::ReturnCode::FAIL;
        }
        break;
    }

    case offsetof(MemoryMap, thruster_speed_0):
    {
        int speed;
        memcpy(&speed, write_buffer, sizeof(speed));
        thruster_0_.SetSpeed(speed);
        thruster_speed_0 = speed;
        break;
    }
    case offsetof(MemoryMap, thruster_speed_1):
    {
        int speed;
        memcpy(&speed, write_buffer, sizeof(speed));
        thruster_1_.SetSpeed(speed);
        thruster_speed_1 = speed;
        break;
    }
    case offsetof(MemoryMap, thruster_speed_2):
    {
        int speed;
        memcpy(&speed, write_buffer, sizeof(speed));
        thruster_2_.SetSpeed(speed);
        thruster_speed_2 = speed;
        break;
    }
    case offsetof(MemoryMap, thruster_speed_3):
    {
        int speed;
        memcpy(&speed, write_buffer, sizeof(speed));
        thruster_3_.SetSpeed(speed);
        thruster_speed_3 = speed;
        break;
    }
    case offsetof(MemoryMap, thruster_speed_4):
    {
        int speed;
        memcpy(&speed, write_buffer, sizeof(speed));
        thruster_4_.SetSpeed(speed);
        thruster_speed_4 = speed;
        break;
    }
    case offsetof(MemoryMap, thruster_speed_5):
    {
        int speed;
        memcpy(&speed, write_buffer, sizeof(speed));
        thruster_5_.SetSpeed(speed);
        thruster_speed_5 = speed;
        break;
    }
    default:
        return hydrolib::ReturnCode::FAIL;
    }
    length -= sizeof(int);
    if (length > 0)
    {
        const void *next_write_buffer =
            static_cast<const uint8_t *>(write_buffer) + sizeof(int);
        return Write(next_write_buffer, address + sizeof(int), length);
    }
    return hydrolib::ReturnCode::OK;
}
} // namespace pioneer
