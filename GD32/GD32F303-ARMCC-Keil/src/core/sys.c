/* Includes */
#include "gd32f30x.h"
#include "systick.h"

static volatile uint32_t sys_delay_ms = 0;
void systick_config(void)
{
	/* setup systick timer for 1000Hz interrupts */
	if (SysTick_Config(SystemCoreClock / 1000U)){
		/* capture error */
		while (1){
		}
	}
	/* configure the systick handler priority */
	NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

void delay(uint32_t count)
{
	sys_delay_ms = count;
	while(0U != sys_delay_ms){
	}
}

void delay_decrement(void)
{
	if (0U != sys_delay_ms){
		sys_delay_ms--;
	}
}
