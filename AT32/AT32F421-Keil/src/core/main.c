/* Includes --------------------------------------------------------------------------------------*/
#include "at32f421_clock.h"
#include "lib_timer.h"
#include "dbg.h"
#include "bsp.h"

#include "string.h"
#include "lib_utils.h"
/* Private define --------------------------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------------------------*/
lib_timer_group_t timer = { 0 };
/* Private functions -----------------------------------------------------------------------------*/
void soft_timer_init(void)
{
	/* debug */
	lib_timer_reg(&timer, &debug_task_handle);
	
	/* vofa */
	lib_timer_reg(&timer, &curve_task_handle);

	/* timer */
	lib_timer_group_reg(&timer);
}

int main(void)
{
	/* sys init */
	system_clock_config();
	nvic_config();
	dbg_init();

	/* bsp init */
	delay_init();
	bsp_uart_init();

	/* driver init */
	
	/* soft timer register */
	soft_timer_init();
	
	/* init ok */
	bsp_uart_dma_send_data((uint8_t *)"build time: ", 12);
	bsp_uart_dma_send_data((uint8_t *)lib_utils_build_time, strlen(lib_utils_build_time));
	bsp_uart_dma_send_data((uint8_t *)"\r\n", 2);
	
	while (1) {
		lib_timer_group_handler();
	}
}

void SysTick_Handler(void)
{
	lib_update_clock(1);
}
