/* Includes ------------------------------------------------------------------*/
#include "lib_tick.h"
#include "mcu.h"
#include "delay.h"
/* Private define ------------------------------------------------------------*/
#define STEP_DELAY_MS 50
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void lib_delay_ms(const time_ms_t nms)
{
	delay_ms(nms);
}

time_us_t lib_get_us(void)
{
	static uint64_t ot; // old time val
	uint64_t nt; // new time val
	nt = ((uint16_t)(lib_get_ms()) * 1000) + (SysTick->LOAD - SysTick->VAL) * 1000 / SysTick->LOAD;

	// 当计算出来的时间值小于上一个时间值时
	// 说明在函数计算的期间发生了SysTick中断，此时应该补正时间值
	if (nt < ot) {
		nt += 1000;
	}
	ot = nt;

	// 返回正确的时间值
	return nt;
}
