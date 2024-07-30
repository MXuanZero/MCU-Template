/* Includes ------------------------------------------------------------------*/
#include "vofa.h"
#include <stdarg.h>
/* Private define ------------------------------------------------------------*/
#define VOFA_SEND_DATA_FUNC(data, len) usart2_dma_send_data(data, len);
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
lib_timer_t curve_task_handle = { 0 };
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

	va_end(args);
}

void vofa_send_data(void)
{
}

void curve_task(lib_timer_event event, void *user_data)
{
	vofa_send_data();
}

void vofa_init(void)
{
	lib_timer_init(&curve_task_handle, 1000, 2000, curve_task);
}
