/* Includes ------------------------------------------------------------------*/
#include "lib_timer.h"
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static lib_list_t lib_timer_groups = { 0 };
/* Private function prototypes -----------------------------------------------*/
static void lib_timer_fsm(lib_timer_t *timer);
/* Private functions ---------------------------------------------------------*/
void lib_timer_init(lib_timer_t *timer, time_us_t period, time_us_t timeout,
		    lib_timer_cb_fn cb_fn)
{
	if (timer == NULL) {
		return;
	}
	timer->period = period;
	timer->timeout = timeout;
	timer->fn = cb_fn;
	timer->node.next = NULL;
	timer->node.prev = NULL;
}

void lib_timer_group_reg(lib_timer_group_t *group)
{
	if (group == NULL) {
		return;
	}
	if (group->id != 0) {
		return;
	}
	lib_node_t *node = &group->node;
	lib_list_inc_tail(&lib_timer_groups, node);
}

void lib_timer_reg(lib_timer_group_t *group, lib_timer_t *timer)
{
	if (group == NULL || timer == NULL) {
		return;
	}
	if (timer->fn == NULL) {
		return;
	}
	lib_list_t *list = &group->list;
	lib_node_t *node = &timer->node;
	lib_list_inc_tail(list, node);
}

static inline uint32_t lib_timer_get_micros(void)
{
	return lib_get_us();
}

static void lib_timer_fsm(lib_timer_t *timer)
{
	uint32_t start_time;
	lib_timer_event event;
	time_us_t temp;
	time_us_t us = lib_timer_get_micros();
	switch (timer->state) {
	case LIB_TIMER_READY:
		temp = us - timer->last_run;
		if (temp < timer->period) {
			return;
		}
		if (temp > timer->timeout) {
			event = LIB_TIMER_TIMEOUT;
		} else {
			event = LIB_TIMER_NORMAL;
		}
		timer->last_run = us;
		start_time = lib_timer_get_micros();
		if (timer->fn != NULL) {
#if LIB_USE_SELF_DATA == 1
			timer->fn(event, timer->user_data);
#else
			timer->fn(event, NULL);
#endif
			timer->run_time = lib_timer_get_micros() - start_time;
		} else {
			/* error */
			timer->run_time = 0;
		}
		break;
	case LIB_TIMER_DELAY:
		temp = us - timer->last_run;
		if (temp < LIB_TIMER_PERIOD_MAX) {
			timer->last_run = us;
			timer->state = LIB_TIMER_READY;
		}
		break;
	case LIB_TIMER_STOP:
		break;
	default:
		/* error */
		break;
	}
}

void lib_timer_handler(lib_timer_group_t *group)
{
	lib_node_t *temp, *node;
	lib_timer_t *timer;
	lib_list_t *list = &group->list;
	lib_list_for_each_safe(list, node, temp) {
		timer = container_addr(node, lib_timer_t, node);
		lib_timer_fsm(timer);
	}
}

void lib_timer_group_handler(void)
{
	lib_node_t *temp, *node;
	lib_timer_group_t *group;
	lib_list_t *list = &lib_timer_groups;
	lib_list_for_each_safe(list, node, temp) {
		group = container_addr(node, lib_timer_group_t, node);
		lib_timer_handler(group);
	}
}

lib_timer_error lib_timer_delay(lib_timer_t *timer, time_us_t us)
{
	if (timer == NULL || us == 0 || us > LIB_TIMER_PERIOD_MAX) {
		return LIB_TIEMR_ERROR;
	}
	if (timer->state == LIB_TIMER_STOP) {
		return LIB_TIMER_OK;
	}
	time_us_t tick = lib_timer_get_micros();
	timer->state = LIB_TIMER_DELAY;
	timer->last_run = tick - us;
	return LIB_TIMER_OK;
}

lib_timer_error lib_timer_group_delay(lib_timer_group_t *group, time_us_t us)
{
	if (group == NULL || us == 0 || us > LIB_TIMER_PERIOD_MAX) {
		return LIB_TIEMR_ERROR;
	}
	lib_timer_t *timer;
	lib_node_t *node;
	lib_list_for_each(&group->list, node) {
		timer = container_addr(node, lib_timer_t, node);
		if (timer->state == LIB_TIMER_STOP) {
			continue;
		}
		time_us_t tick = lib_timer_get_micros();
		timer->state = LIB_TIMER_DELAY;
		timer->last_run = tick - us;
	}
	return LIB_TIMER_OK;
}

void lib_timer_stop(lib_timer_t *timer)
{
	timer->state = LIB_TIMER_STOP;
	timer->last_run = 0;
}

void lib_timer_restart(lib_timer_t *timer)
{
	timer->state = LIB_TIMER_READY;
	timer->last_run = lib_timer_get_micros();
}
