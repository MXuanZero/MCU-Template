/* Includes --------------------------------------------------------------------------------------*/
#include "nvic.h"
#include "gd32f30x_libopt.h"
/* Private define --------------------------------------------------------------------------------*/
#define ENCODER_SAMPLING_IRQ TIMER5_IRQn// 编码器采样中断
#define USART_TX_DMA_IRQ DMA0_Channel1_IRQn // 串口DMA发送中断
#define USART_IRQ USART2_IRQn // 串口中断
/* Private macro ---------------------------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------------*/
/* Private functions -----------------------------------------------------------------------------*/
void nvic_config(void)
{
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	
	nvic_irq_enable(USART_IRQ, 1, 0);
	nvic_irq_enable(USART_TX_DMA_IRQ, 1, 0);
	nvic_irq_enable(ENCODER_SAMPLING_IRQ, 0, 1);
	
	nvic_irq_enable(SDIO_IRQn, 0, 1);
	nvic_irq_enable(TIMER6_IRQn, 0, 0);
}
