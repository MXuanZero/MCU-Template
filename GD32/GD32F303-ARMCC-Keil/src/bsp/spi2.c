/* Includes --------------------------------------------------------------------------------------*/
#include "spi2.h"
#include "gd32f30x_libopt.h"
#include "systick.h"
/* Private define --------------------------------------------------------------------------------*/
#define SPI SPI2
#define SPI_CLK RCU_SPI2
/* Private macro ---------------------------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------------*/
/* Private functions -----------------------------------------------------------------------------*/
void spi2_nss_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	gpio_bit_set(GPIOA, GPIO_PIN_15);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
}

void spi2_gpio_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_bit_reset(GPIOB, GPIO_PIN_3);
	gpio_bit_reset(GPIOB, GPIO_PIN_4);
	gpio_bit_reset(GPIOB, GPIO_PIN_5);

	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
	gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
}

void spi2_init(void)
{
	/* SPI2 GPIO */
	rcu_periph_clock_enable(SPI_CLK);
	gpio_pin_remap_config(GPIO_SPI2_REMAP, DISABLE);

	spi2_nss_init();
	spi2_gpio_init();

	/* SPI2 parameter config */
	spi_parameter_struct spi_init_struct;

	spi_disable(SPI);
	spi_struct_para_init(&spi_init_struct);
	spi_init_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
	spi_init_struct.device_mode = SPI_MASTER;
	spi_init_struct.frame_size = SPI_FRAMESIZE_16BIT;
	spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE;
	spi_init_struct.nss = SPI_NSS_SOFT;
	spi_init_struct.prescale = SPI_PSC_16; // 120 / 16
	spi_init_struct.endian = SPI_ENDIAN_MSB;
	spi_init(SPI, &spi_init_struct);

	spi_enable(SPI);
}

inline void spi2_send_data_16bit(uint16_t *data, uint16_t len)
{
	/* RESET为发送缓冲区非空 */
	while (spi_i2s_flag_get(SPI, I2S_FLAG_TBE) == RESET) {
	}

	do {
		spi_i2s_data_transmit(SPI, *(data++));
		while (spi_i2s_flag_get(SPI, I2S_FLAG_TBE) == RESET) {
		}
	} while (--len);

	while (spi_i2s_flag_get(SPI, SPI_FLAG_TRANS) == SET) {
	}
}

inline void spi2_recv_data_16bit(uint16_t *data, uint16_t len)
{
	do {
		while (spi_i2s_flag_get(SPI, I2S_FLAG_RBNE) == RESET) {
		}
		*(data++) = spi_i2s_data_receive(SPI);
	} while (--len);
}
