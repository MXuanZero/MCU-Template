/* Includes --------------------------------------------------------------------------------------*/
#include "usart2_dma.h"

#include "gd32f30x_libopt.h"
/* Private define --------------------------------------------------------------------------------*/
#define USART_GPIO_PORT GPIOB // GPIO
#define USART_GPIO_RX GPIO_PIN_11 // RX
#define USART_GPIO_TX GPIO_PIN_10 // TX
#define USART_GPIO_CLK RCU_GPIOB // GPIO RCU

#define USART USART2 // 串口号
#define USART_CLK RCU_USART2 //  串口

#define USART_DMA DMA0 // DMA
#define USART_TX_DMA_CH DMA_CH1 //  串口发送DMA通道
#define USART_RX_DMA_CH DMA_CH2 //  串口接收DMA通道
#define USART_DMA_CLK RCU_DMA0 //  DMA时钟
#define USART_DATA_ADDRESS ((uint32_t)&USART_DATA(USART)) // 串口发送数据地址
/* Private macro ---------------------------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------------*/
/* Private functions -----------------------------------------------------------------------------*/
void usart2_dma_init(void)
{
	/* USART Init */
	rcu_periph_clock_enable(USART_CLK);
	rcu_periph_clock_enable(USART_GPIO_CLK);

	gpio_init(USART_GPIO_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, USART_GPIO_TX);
	gpio_init(USART_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, USART_GPIO_RX);

	usart_deinit(USART);
	usart_baudrate_set(USART, 115200U); // 波特率
	usart_word_length_set(USART, USART_WL_8BIT); // 帧数据字长
	usart_stop_bit_set(USART, USART_STB_1BIT); // 停止位
	usart_parity_config(USART, USART_PM_NONE); // 奇偶校验位
	usart_hardware_flow_rts_config(USART, USART_RTS_DISABLE); // 硬件流RTS
	usart_hardware_flow_cts_config(USART, USART_CTS_DISABLE); // 硬件流CTS
	usart_receive_config(USART, USART_RECEIVE_ENABLE); // 使能接收
	usart_transmit_config(USART, USART_TRANSMIT_ENABLE); // 使能发送

	dma_parameter_struct dma_init_struct;

	/* DMA TX Init */
	rcu_periph_clock_enable(USART_DMA_CLK);

	dma_deinit(USART_DMA, USART_TX_DMA_CH);
	dma_struct_para_init(&dma_init_struct);
	dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
	dma_init_struct.memory_addr = NULL;
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
	dma_init_struct.number = 0;
	dma_init_struct.periph_addr = USART_DATA_ADDRESS;
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
	dma_init_struct.priority = DMA_PRIORITY_LOW;

	dma_init(USART_DMA, USART_TX_DMA_CH, &dma_init_struct);

	/* DMA RX Init */
	rcu_periph_clock_enable(USART_DMA_CLK);

	dma_deinit(USART_DMA, USART_RX_DMA_CH);
	dma_struct_para_init(&dma_init_struct);
	dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
	dma_init_struct.memory_addr = NULL;
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
	dma_init_struct.number = 0;
	dma_init_struct.periph_addr = USART_DATA_ADDRESS;
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
	dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
	dma_init(USART_DMA, USART_RX_DMA_CH, &dma_init_struct);

	/* enable */
	dma_circulation_disable(USART_DMA, USART_TX_DMA_CH);
	dma_circulation_disable(USART_DMA, USART_RX_DMA_CH);
	dma_interrupt_enable(USART_DMA, USART_TX_DMA_CH, DMA_INT_FTF);

	usart_dma_transmit_config(USART, USART_TRANSMIT_DMA_ENABLE);
	usart_dma_receive_config(USART, USART_RECEIVE_DMA_ENABLE);
	
	usart_interrupt_flag_clear(USART, USART_INT_FLAG_RBNE);
	usart_interrupt_flag_clear(USART, USART_INT_FLAG_IDLE);
	usart_interrupt_flag_clear(USART, USART_INT_FLAG_TC);
	usart_interrupt_enable(USART, USART_INT_IDLE);
	usart_interrupt_enable(USART, USART_INT_TC);
	
	usart_enable(USART);
}

void usart2_send_data(uint8_t *data, uint16_t len)
{
	/* 发送数据 */
	for (uint16_t i = 0; i < len; i++) {
		usart_data_transmit(USART, data[i]);
		/* 判断传输是否完成 */
		while (RESET == usart_flag_get(USART, USART_FLAG_TBE)) {
		}
	}
}

inline uint16_t usart2_dma_send_len(void)
{
	return dma_transfer_number_get(USART_DMA, USART_TX_DMA_CH);
}

inline uint16_t usart2_dma_recv_len(uint16_t rx_max)
{
	return rx_max - dma_transfer_number_get(USART_DMA, USART_RX_DMA_CH);
}

void usart2_dma_send_data(const uint8_t *data, uint16_t len)
{
	while (usart2_dma_send_len()) {
		__NOP();
	}
	/* check */
	dma_channel_disable(USART_DMA, USART_TX_DMA_CH);

	/* padding data */
	dma_periph_address_config(USART_DMA, USART_TX_DMA_CH, USART_DATA_ADDRESS);
	dma_memory_address_config(USART_DMA, USART_TX_DMA_CH, (uint32_t)data);
	dma_transfer_number_config(USART_DMA, USART_TX_DMA_CH, len);
	
	dma_channel_enable(USART_DMA, USART_TX_DMA_CH);
}

void usart2_dma_recv_data(const uint8_t *rx_data, uint16_t len, bool cmen)
{
	dma_channel_disable(USART_DMA, USART_RX_DMA_CH);
	if (cmen == true) {
		dma_circulation_enable(USART_DMA, USART_RX_DMA_CH);
	}

	dma_periph_address_config(USART_DMA, USART_RX_DMA_CH, USART_DATA_ADDRESS);
	dma_memory_address_config(USART_DMA, USART_RX_DMA_CH, (uint32_t)rx_data);
	dma_transfer_number_config(USART_DMA, USART_RX_DMA_CH, len);

	dma_channel_enable(USART_DMA, USART_RX_DMA_CH);
}

void usart2_iqr(void)
{
	if (RESET != usart_interrupt_flag_get(USART, USART_INT_FLAG_RBNE)) {
		usart_data_receive(USART);
	} else if (RESET != usart_interrupt_flag_get(USART, USART_INT_FLAG_IDLE)) {
		usart_data_receive(USART2); // gd32读取寄存器清除空闲中断
		usart2_interrupt_idle_cb();
	} else if (RESET != usart_interrupt_flag_get(USART, USART_INT_FLAG_TC)) {
		usart_interrupt_flag_clear(USART, USART_INT_FLAG_TC);
	}
}

void usart2_dma0_ch1_iqr(void)
{
	if (dma_interrupt_flag_get(USART_DMA, USART_TX_DMA_CH, DMA_INT_FLAG_FTF)) {
		dma_interrupt_flag_clear(USART_DMA, USART_TX_DMA_CH, DMA_INT_FLAG_G);
		/* dma传输完成中断 */
	}
}

void usart2_dma0_ch2_iqr(void)
{
	if (dma_interrupt_flag_get(USART_DMA, USART_RX_DMA_CH, DMA_INT_FLAG_FTF)) {
		dma_interrupt_flag_clear(USART_DMA, USART_RX_DMA_CH, DMA_INT_FLAG_G);
		/* dma传输完成中断 */
	}
}

__attribute__((weak)) void usart2_interrupt_idle_cb(void)
{
	(void)0;
}
