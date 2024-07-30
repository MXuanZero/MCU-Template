/* Includes --------------------------------------------------------------------------------------*/
#include "uart.h"
#include "mcu.h"
#include <string.h>
/* Private define --------------------------------------------------------------------------------*/
#define USART USART1
#define USART_DMA DMA1_CHANNEL2
/* Private macro ---------------------------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------------*/
/* Private functions -----------------------------------------------------------------------------*/
void bsp_uart_init(void)
{
	/* uart gpio */
	gpio_init_type gpio_init_struct;

	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
	gpio_default_para_init(&gpio_init_struct);
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	gpio_init_struct.gpio_pins = GPIO_PINS_6 | GPIO_PINS_7;
	gpio_init(GPIOB, &gpio_init_struct);

	gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE6, GPIO_MUX_0);
	gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE7, GPIO_MUX_0);
	
	/* usart1 param */
	crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, TRUE);
	usart_init(USART1, 115200, USART_DATA_8BITS, USART_STOP_1_BIT);

	/* usart tx dma */
	dma_init_type dma_init_struct;
	crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
	dma_reset(USART_DMA);
	dma_default_para_init(&dma_init_struct);
	dma_init_struct.buffer_size = 0;
	dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
	dma_init_struct.memory_base_addr = NULL;
	dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
	dma_init_struct.memory_inc_enable = TRUE;
	dma_init_struct.peripheral_base_addr = (uint32_t)&USART->dt;
	dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
	dma_init_struct.peripheral_inc_enable = FALSE;
	dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
	dma_init_struct.loop_mode_enable = FALSE;
	dma_init(USART_DMA, &dma_init_struct);

	/* usart rx dma */
	// dma_init_type dma_init_struct;
	// dma_reset(DMA1_CHANNEL3);
	// dma_default_para_init(&dma_init_struct);
	// dma_init_struct.buffer_size = USART2_TX_BUFFER_SIZE;
	// dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
	// dma_init_struct.memory_base_addr = (uint32_t)usart1_rx_buffer;
	// dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
	// dma_init_struct.memory_inc_enable = TRUE;
	// dma_init_struct.peripheral_base_addr = (uint32_t)&USART1->dt;
	// dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
	// dma_init_struct.peripheral_inc_enable = FALSE;
	// dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
	// dma_init_struct.loop_mode_enable = FALSE;
	// dma_init(DMA1_CHANNEL3, &dma_init_struct);

	/* enable */
	usart_transmitter_enable(USART, TRUE);
	usart_dma_transmitter_enable(USART, TRUE);
	usart_receiver_enable(USART, FALSE);
	usart_dma_receiver_enable(USART, FALSE);
	usart_enable(USART, TRUE);
	// dma_channel_enable(DMA1_CHANNEL3, TRUE); /* usart1 rx begin dma receiving */
	// dma_channel_enable(DMA1_CHANNEL2, TRUE); /* usart1 tx begin dma transmitting */
}

inline uint16_t bsp_uart_dma_get_dtcnt(void)
{
	return USART_DMA->dtcnt;
}

void bsp_uart_dma_send_data(const uint8_t *data, uint16_t len)
{
	while (bsp_uart_dma_get_dtcnt()) {
		__NOP();
	}

	/* check */
	dma_channel_enable(USART_DMA, FALSE);
	USART_DMA->paddr = (uint32_t)&USART->dt;
	USART_DMA->maddr = (uint32_t)data;
	USART_DMA->dtcnt = len;
	dma_channel_enable(USART_DMA, TRUE);
}
