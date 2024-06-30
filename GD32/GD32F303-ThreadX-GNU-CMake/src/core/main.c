#include "gd32f30x_libopt.h"
#include "sys.h"
#include "tx_api.h"

#define TASK_STACK_SIZE 1024
TX_THREAD thread_0;
uint8_t thread_0_stack[TASK_STACK_SIZE] = { 0 };
void swd_init(void)
{
	rcu_periph_clock_enable(RCU_AF);
	gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
}

int main(void)
{
	swd_init();
	systick_config();
	tx_kernel_enter();
	while (1) {
	}
}

/* 任务入口函数 */
void thread_0_entry(ULONG thread_input)
{
	(void)thread_input;
	volatile float f = 0.0f, f1 = 3.14f, f2 = 6.28f;
	rcu_periph_clock_enable(RCU_GPIOC);
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_14);
	gpio_bit_reset(GPIOC, GPIO_PIN_13);

	while (1){
		gpio_bit_set(GPIOC, GPIO_PIN_14);
		f = f1 * f2;
		gpio_bit_reset(GPIOC, GPIO_PIN_14);
		tx_thread_sleep((int)(f - 17.0f));
	}
}

void tx_application_define(void *first_unused_memory)
{
	(void)first_unused_memory;
	/* 创建任务0 */
	tx_thread_create(&thread_0, "thread_0", thread_0_entry, 0,
			 thread_0_stack, TASK_STACK_SIZE, 16, 16,
			 TX_NO_TIME_SLICE, TX_AUTO_START);
}

