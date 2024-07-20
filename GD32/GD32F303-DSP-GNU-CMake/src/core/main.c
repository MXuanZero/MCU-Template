#include "gd32f30x_libopt.h"
#include "systick.h"

void swd_init(void)
{
	rcu_periph_clock_enable(RCU_AF);
	gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
}

int main(void)
{
	swd_init();
	systick_config();

	rcu_periph_clock_enable(RCU_GPIOC);
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);

	while (1) {
		delay(1000);
		gpio_bit_set(GPIOC, GPIO_PIN_13);
		delay(1000);
		gpio_bit_reset(GPIOC, GPIO_PIN_13);
	}
}
