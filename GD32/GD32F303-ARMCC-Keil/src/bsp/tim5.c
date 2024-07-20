/* Includes --------------------------------------------------------------------------------------*/
#include "tim5.h"
#include "gd32f30x_libopt.h"
#include "control.h"
/* Private define --------------------------------------------------------------------------------*/
#define TIMER TIMER5
#define RCU_TIMER RCU_TIMER5
/* Private macro ---------------------------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------------*/
/* Private functions -----------------------------------------------------------------------------*/
void tim5_init(void)
{
	timer_parameter_struct timer_initpara;
	timer_deinit(TIMER);
	rcu_periph_clock_enable(RCU_TIMER);

	/* TIMER6 configuration */
	timer_initpara.prescaler = 59;
	timer_initpara.alignedmode = TIMER_COUNTER_CENTER_BOTH;
	timer_initpara.counterdirection = TIMER_COUNTER_UP;
	timer_initpara.period = 499;
	timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
	timer_initpara.repetitioncounter = 0;
	timer_init(TIMER, &timer_initpara);

	timer_interrupt_flag_clear(TIMER, TIMER_INT_FLAG_UP);
	timer_interrupt_enable(TIMER, TIMER_INT_UP);
	
	// timer_enable(TIMER);
}

void tim5_callback_2khz(void)
{
	if (SET == timer_interrupt_flag_get(TIMER, TIMER_INT_FLAG_UP)) {
		timer_interrupt_flag_clear(TIMER, TIMER_INT_FLAG_UP);
		encoder_handler();
	}
}
