#include "gd32f30x_libopt.h"
#include "systick.h"
#include "dbg.h"

int main(void)
{
	/* sys init */
	dbg_init();
	systick_config();

	/* bsp init */

	
	/* drivers init */
	
	
	/* task init */
	
	rcu_periph_clock_enable(RCU_GPIOC);
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);


	while (1) {
		delay(1000);
		gpio_bit_set(GPIOC, GPIO_PIN_13);
		delay(1000);
		gpio_bit_reset(GPIOC, GPIO_PIN_13);
	}
}
