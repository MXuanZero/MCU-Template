/* Includes ------------------------------------------------------------------*/
#include "vofa.h"
/* Private define ------------------------------------------------------------*/
#define VOFA_SEND_DATA_FUNC(data, len) usart2_dma_send_data(data, len);
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static volatile vofa_t vofa = {
	.tail = { 0x00, 0x00, 0x80, 0x7f },
};

void vofa_set_data(uint8_t ch, float data)
{
	if (ch > VOFA_CH_CNT - 1) {
		return;
	}
	vofa.fdata[ch] = data;
}

void vofa_set_data_va(int ch_num, ...)
{
	va_list args;
	va_start(args, ch_num);

	if (ch_num > VOFA_CH_CNT || ch_num <= 0) {
		return;
	}
	for (int i = 0; i < ch_num; i++) {
		vofa.fdata[i] = (float)va_arg(args, double);
	}

	va_end(args);
}

void vofa_send_data(void)
{
}
