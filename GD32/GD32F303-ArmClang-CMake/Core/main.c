#include "gd32f30x.h"
#include "systick.h"
#include "main.h"

int main(void)
{
	systick_config();

	rcu_periph_clock_enable(RCU_GPIOC);
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
	gpio_bit_reset(GPIOC, GPIO_PIN_13);

	while (1) {
	}
}
