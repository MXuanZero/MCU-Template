/* Includes --------------------------------------------------------------------------------------*/
#include "dbg.h"
#include "gd32f30x_libopt.h"
#include "SEGGER_RTT.h"
/* Private define --------------------------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------------*/
unsigned js_rtt_channel = 1;
uint8_t buffer[512] = { 0 };

/* Private functions -----------------------------------------------------------------------------*/
static inline void swd_init(void)
{
	rcu_periph_clock_enable(RCU_AF);
	gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
}

static inline void jscope_init(void)
{
	SEGGER_RTT_ConfigUpBuffer(js_rtt_channel, "JScope_I4I4", buffer, sizeof(buffer),
				  SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
}

void dbg_init(void)
{
	swd_init();
	jscope_init();
}

void jscope_send(void)
{
}
