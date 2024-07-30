#ifndef LIB_TIMER_H
#define LIB_TIMER_H
/* Includes ------------------------------------------------------------------*/
#include "lib_cfg.h"
#include "lib_list.h"
#include "lib_tick.h"
#include "ctrl_defs.h"
/* Define --------------------------------------------------------------------*/
#define LIB_TIMER_PERIOD_MAX (UINT32_MAX >> 1)

#ifndef LIB_TIMER_USE_SELF_DATA
#define LIB_TIMER_USE_SELF_DATA 0
#endif // LIB_TIMER_USE_SELF_DATA
/* Exported macro ------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
typedef enum lib_timer_event {
	LIB_TIMER_NORMAL = 0,
	LIB_TIMER_TIMEOUT = 1,
} lib_timer_event;

typedef enum lib_timer_status {
	LIB_TIMER_READY = 0,
	LIB_TIMER_DELAY = 1,
	LIB_TIMER_STOP = 2,
} lib_timer_status;

typedef enum lib_timer_error {
	LIB_TIMER_OK = 0,
	LIB_TIEMR_ERROR,
} lib_timer_error;

typedef void (*lib_timer_cb_fn)(lib_timer_event event, void *user_data);

/**
 * @next next lib_timer_t
 * @cb_fn callback function
 * @period between each execution
 * @timeout timeout
 * @repeat_count if the number becomes 0, the list is immediately disconnected
 */
typedef struct lib_timer_t {
	lib_node_t node;
	lib_timer_cb_fn fn;
#if LIB_TIMER_USE_SELF_DATA
	void *user_data;
#endif // LIB_TIMER_USE_SELF_DATA
	lib_timer_status state;
	time_ms_t run_time;
	time_us_t period;
	time_us_t timeout;
	time_us_t last_run;
	int32_t repeat_cnt;
} lib_timer_t;

typedef struct lib_timer_group_t {
	lib_list_t list;
	lib_node_t node;
	uint16_t id;
} lib_timer_group_t;
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void lib_timer_init(lib_timer_t *timer, time_us_t period, time_us_t timeout,
		    lib_timer_cb_fn cb_fn);
void lib_timer_group_reg(lib_timer_group_t *group);
void lib_timer_reg(lib_timer_group_t *group, lib_timer_t *timer);
void lib_timer_handler(lib_timer_group_t *group);
void lib_timer_group_handler(void);
lib_timer_error lib_timer_delay(lib_timer_t *timer, time_us_t us);
lib_timer_error lib_timer_group_delay(lib_timer_group_t *group, time_us_t us);
void lib_timer_stop(lib_timer_t *timer);
void lib_timer_restart(lib_timer_t *timer);
#endif //LIB_TIMER_H
