/* Includes --------------------------------------------------------------------------------------*/
#include <stddef.h>
#include "gd32f30x_libopt.h"
#include "spi0_dma.h"
/* Private define --------------------------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------------*/
/* Private functions -----------------------------------------------------------------------------*/
static void rcu_config(void)
{
	rcu_periph_clock_enable(RCU_SPI1);
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_DMA0);
	rcu_periph_clock_enable(RCU_DMA1);
}

static void gpio_config(void)
{
	/* I2S2 GPIO config: I2S2_WS/PA15, I2S2_CK/PB3, I2S2_SD/PB5 */
	gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3 | GPIO_PIN_5);
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
}

static void dma_config(void)
{
	dma_parameter_struct dma_init_struct;

	/* SPI1 transmit dma config: DMA0,DMA_CH4  */
	dma_deinit(DMA0, DMA_CH4);
	dma_init_struct.periph_addr = (uint32_t)&SPI_DATA(SPI1);
	dma_init_struct.memory_addr = NULL;
	dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
	dma_init_struct.priority = DMA_PRIORITY_LOW;
	dma_init_struct.number = 0;
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_init(DMA0, DMA_CH4, &dma_init_struct);
	/* configure DMA mode */
	dma_circulation_disable(DMA0, DMA_CH4);
	dma_memory_to_memory_disable(DMA0, DMA_CH4);

	/* SPI2 receive dma config: DMA1-DMA_CH0 */
	dma_deinit(DMA1, DMA_CH0);
	dma_init_struct.periph_addr = (uint32_t)&SPI_DATA(SPI2);
	dma_init_struct.memory_addr = NULL;
	dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
	dma_init_struct.priority = DMA_PRIORITY_HIGH;
	dma_init(DMA1, DMA_CH0, &dma_init_struct);
	/* configure DMA mode */
	dma_circulation_disable(DMA1, DMA_CH0);
	dma_memory_to_memory_disable(DMA1, DMA_CH0);
}

static void spi_config(void)
{
	spi_i2s_deinit(SPI1);
	spi_i2s_deinit(SPI2);

	i2s_init(SPI1, I2S_MODE_MASTERTX, I2S_STD_PHILLIPS, I2S_CKPL_LOW);
	i2s_psc_config(SPI1, I2S_AUDIOSAMPLE_11K, I2S_FRAMEFORMAT_DT16B_CH16B, I2S_MCKOUT_DISABLE);

	i2s_init(SPI2, I2S_MODE_SLAVERX, I2S_STD_PHILLIPS, I2S_CKPL_LOW);
	i2s_psc_config(SPI2, I2S_AUDIOSAMPLE_11K, I2S_FRAMEFORMAT_DT16B_CH16B, I2S_MCKOUT_DISABLE);
}

void spi0_dma_init(void)
{
	/* peripheral clock enable */
	rcu_config();
	/* GPIO config */
	gpio_config();
	/* DMA config */
	dma_config();
	/* SPI config */
	spi_config();

	/* SPI enable */
	i2s_enable(SPI2);
	i2s_enable(SPI1);

	/* DMA channel enable */
	dma_channel_enable(DMA0, DMA_CH4);
	dma_channel_enable(DMA1, DMA_CH0);

	/* SPI DMA enable */
	spi_dma_enable(SPI2, SPI_DMA_RECEIVE);
	spi_dma_enable(SPI1, SPI_DMA_TRANSMIT);

//	/* wait DMA transmit complete */
//	while (!dma_flag_get(DMA0, DMA_CH4, DMA_INTF_FTFIF)){
//	}
//	while (!dma_flag_get(DMA1, DMA_CH0, DMA_INTF_FTFIF)){
//	}
}
