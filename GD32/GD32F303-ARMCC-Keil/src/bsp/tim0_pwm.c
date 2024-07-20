/* Includes --------------------------------------------------------------------------------------*/
#include "tim0_pwm.h"

#include "gd32f30x_libopt.h"
/* Private define --------------------------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------------*/
/* Private functions -----------------------------------------------------------------------------*/
static void timer0_gpio_config(void)
{
	rcu_periph_clock_enable(RCU_AF);
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);


	/* TIMER0 CH0 CH1 CH2 */
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_8);
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_9);
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_10);

	/* TIMER0 CH0N CH1N CH2N */
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_13);
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_14);
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_15);
}

static void timer0_config(void)
{
	timer_oc_parameter_struct timer_ocintpara;
	timer_parameter_struct timer_initpara;
	timer_break_parameter_struct timer_breakpara;

	rcu_periph_clock_enable(RCU_TIMER0);

	timer_deinit(TIMER0);

	/* TIMER0 configuration */
	timer_initpara.prescaler = 0;
	timer_initpara.alignedmode = TIMER_COUNTER_CENTER_BOTH;
	timer_initpara.counterdirection = TIMER_COUNTER_UP;
	timer_initpara.period = 3000;
	timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
	timer_initpara.repetitioncounter = 0;
	timer_init(TIMER0, &timer_initpara);

	/* CH1,CH2 and CH3 configuration in PWM mode */
	timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
	timer_ocintpara.outputnstate = TIMER_CCXN_ENABLE;
	timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
	timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
	timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
	timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

	timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocintpara);
	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 2000);
	timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
	timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

	timer_channel_output_config(TIMER0, TIMER_CH_1, &timer_ocintpara);
	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, 2000);
	timer_channel_output_mode_config(TIMER0, TIMER_CH_1, TIMER_OC_MODE_PWM0);
	timer_channel_output_shadow_config(TIMER0, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

	timer_channel_output_config(TIMER0, TIMER_CH_2, &timer_ocintpara);
	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_2, 3000);
	timer_channel_output_mode_config(TIMER0, TIMER_CH_2, TIMER_OC_MODE_PWM0);
	timer_channel_output_shadow_config(TIMER0, TIMER_CH_2, TIMER_OC_SHADOW_DISABLE);

	timer_primary_output_config(TIMER0, ENABLE);

	timer_breakpara.runoffstate = TIMER_ROS_STATE_ENABLE;
	timer_breakpara.ideloffstate = TIMER_IOS_STATE_ENABLE;
	timer_breakpara.deadtime = 2;
	timer_breakpara.breakpolarity = TIMER_BREAK_POLARITY_LOW;
	timer_breakpara.outputautostate = TIMER_OUTAUTO_ENABLE;
	timer_breakpara.protectmode = TIMER_CCHP_PROT_OFF;
	timer_breakpara.breakstate = TIMER_BREAK_DISABLE;
	timer_break_config(TIMER0, &timer_breakpara);

	/* auto-reload preload enable */
	timer_auto_reload_shadow_enable(TIMER0);
}

void tim0_init(void)
{
	timer0_gpio_config();
	timer0_config();
}
