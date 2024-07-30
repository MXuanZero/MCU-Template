#ifndef MATH_FILTER_H
#define MATH_FILTER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "math_base.h"
#include "ctrl_defs.h"
/* Define --------------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
typedef struct math_lpf_1st {
	float alpha;
	float prev;
} math_lpf_1st_t;

typedef struct math_lpf_1st_int32 {
	float alpha;
	int32_t prev;
} math_lpf_1st_int32_t;
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * 低通滤波初始化
 */
void math_lpf_1st_init(math_lpf_1st_t *f, float sample_freq, float cutoff_freq);

/**
 * 低通滤波器使用
 */
float math_lpf_1st_apply(math_lpf_1st_t *f, float input);

/**
 * 低通滤波初始化
 */
void math_lpf_1st_init_int32(math_lpf_1st_int32_t *f, float sample_freq, float cutoff_freq);

/**
 * 低通滤波器使用
 */
int32_t math_lpf_1st_apply_int32(math_lpf_1st_int32_t *f, int32_t input);

#ifdef __cplusplus
}
#endif

#endif // MATH_FILTER_H
