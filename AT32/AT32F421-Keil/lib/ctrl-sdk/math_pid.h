#ifndef MATH_PID_H
#define MATH_PID_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "math_base.h"
#include "ctrl_defs.h"
typedef struct math_pid_t {
	float kp; // P
	float ki; // I
	float kd; // D

	float op; // KP输出
	float oi; // KI输出
	float od; // KD输出
	float out; // pInt32_t
	float out_min;
	float out_max;

	float integrator;
	float Integrator_max; // 积分限幅
	float last_error; // 上次误差
	float last_derivative; // 上次误差与上上次误差之差
	unsigned long last_time; // 上次时间
} math_pid_t;

void math_pid_init(math_pid_t *pid, float kp, float ki, float kd);
void math_pid_clear(math_pid_t *pid, _Bool mod);
float math_pid_inc(math_pid_t *pid, float error);
float math_pid_loc(math_pid_t *pid, float error);
void math_pid_set_output_restrict(math_pid_t *pid, float max, float min);

#ifdef __cplusplus
}
#endif

#endif //MATH_PID_H
