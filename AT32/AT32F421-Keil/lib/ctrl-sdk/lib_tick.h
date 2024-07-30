#ifndef LIB_TICK_H
#define LIB_TICK_H
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "lib_cfg.h"
#include "lib_tick.h"
#include "ctrl_defs.h"
/* Define --------------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define LIB_TIME_MS(ms) ((ms) * 1000)
#define LIB_TIME_US(us) (us)
#define LIB_TIME_HZ(hz) (1000000 / (hz))
/* Exported typedef ----------------------------------------------------------*/
typedef uint32_t time_us_t;
typedef uint32_t time_ms_t;
typedef int32_t time_delta_t;
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void lib_update_clock(time_us_t us);
time_ms_t lib_get_ms(void);
time_us_t lib_get_us(void);
void lib_delay_ms(time_ms_t ms);
#endif // LIB_TICK_H
