/* Includes --------------------------------------------------------------------------------------*/
#include "dbg.h"
#include "mcu.h"

#include "uart.h"
#include "string.h"
#include "lib_utils.h"
/* Private define --------------------------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------------*/
lib_timer_t debug_task_handle = { 0 };
/* Private functions -----------------------------------------------------------------------------*/
inline void swd_init(void)
{
	crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
	gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE13, GPIO_MUX_0);
	gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE14, GPIO_MUX_0);
}

void debug_handle(lib_timer_event event, void *user_data)
{
	// bsp_uart_dma_send_data((uint8_t *)lib_utils_build_time, strlen(lib_utils_build_time));
}

void dbg_init(void)
{
	swd_init();
	lib_timer_init(&debug_task_handle, 1000 * 1000, 2000 * 1000, debug_handle);
}


