/* Includes --------------------------------------------------------------------------------------*/
#include "tim6.h"
#include "gd32f30x_libopt.h"
#include "control.h"
/* Private define --------------------------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------------*/
/* Private functions -----------------------------------------------------------------------------*/
void tim6_init(void)
{
	timer_parameter_struct timer_initpara;
	timer_deinit(TIMER6);
	rcu_periph_clock_enable(RCU_TIMER6);
	
	/* TIMER6 configuration */
	timer_initpara.prescaler = 1;
	timer_initpara.alignedmode = TIMER_COUNTER_CENTER_BOTH;
	timer_initpara.counterdirection = TIMER_COUNTER_UP;
	timer_initpara.period = 3000;
	timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
	timer_initpara.repetitioncounter = 0;
	timer_init(TIMER6, &timer_initpara);

	timer_interrupt_flag_clear(TIMER6, TIMER_INT_FLAG_UP);
	timer_interrupt_enable(TIMER6, TIMER_INT_UP);
}

void tim6_callback_10khz(void)
{
	if (SET == timer_interrupt_flag_get(TIMER6, TIMER_INT_FLAG_UP)) {
		timer_interrupt_flag_clear(TIMER6, TIMER_INT_FLAG_UP);
		control_handler();
	}
}
