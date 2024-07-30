/* Includes ------------------------------------------------------------------*/
#include "math_filter.h"
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void math_lpf_1st_init(math_lpf_1st_t *f, float sample_freq, float cutoff_freq)
{
	float rc = 1.0f / (2.0f * DEF_PI * cutoff_freq); // RC值
	float dt = 1.0f / sample_freq; // 采样时间间隔
	f->alpha = dt / (rc + dt); // 滤波器系数
	f->prev = 0.0f;
}

inline float math_lpf_1st_apply(math_lpf_1st_t *f, float input)
{
	float output = f->alpha * input + (1 - f->alpha) * f->prev;
	f->prev = output;
	return output;
}

void math_lpf_1st_init_int32(math_lpf_1st_int32_t *f, float sample_freq, float cutoff_freq)
{
	float rc = 1.0f / (2.0f * DEF_PI * cutoff_freq); // RC值
	float dt = 1.0f / sample_freq; // 采样时间间隔
	f->alpha = dt / (rc + dt); // 滤波器系数
	f->prev = 0;
}

inline int32_t math_lpf_1st_apply_int32(math_lpf_1st_int32_t *f, int32_t input)
{
	int32_t output = f->alpha * input + (1 - f->alpha) * f->prev;
	f->prev = output;
	return output;
}
