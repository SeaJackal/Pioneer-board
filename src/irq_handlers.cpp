#include "board.hpp"

extern "C"
{
void SysTick_Handler(void)
{
    pioneer::Board::clock_.SysTickHandler();
}

void USART3_IRQHandler(void)
{
    pioneer::Board::uart3_.IRQCallback();
}

void USART1_IRQHandler(void)
{
    pioneer::Board::uart1_.IRQCallback();
}

void HardFault_Handler(void)
{
    while (1)
    {
    }
}
}


