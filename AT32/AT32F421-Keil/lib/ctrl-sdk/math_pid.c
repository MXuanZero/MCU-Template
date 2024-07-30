#include <string.h>
#include <stdbool.h>
#include "math_pid.h"

void math_pid_init(math_pid_t *pid, float kp, float ki, float kd)
{
	if (pid == NULL) {
		return;
	}
	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;
}

void math_pid_clear(math_pid_t *pid, bool mod)
{
	if (pid == NULL) {
		return;
	}

	if (mod == 1) {
		memset(pid, 0, sizeof(math_pid_t));
	} else {
		pid->op = 0; // KP输出
		pid->oi = 0; // KI输出
		pid->od = 0; // KD输出
		pid->out = 0; // pInt32_t
		pid->integrator = 0; // 积分值
		pid->last_error = 0; // 上次误差
		pid->last_derivative = 0; // 上次误差与上上次误差之差
		pid->last_time = 0; // 上次时间
	}
}

/**
 * @brief  pid位置式控制器输出
 * @param  pid     pid参数
 * @param  error   pid输入误差
 * @retval PID输出结果
 */
float math_pid_loc(math_pid_t *pid, float error)
{
	if (pid == NULL) {
		return 0;
	}
	/* 累积误差 */
	pid->integrator += error;
	if (pid->Integrator_max != 0) {
		pid->integrator = DEF_CONSTRAIN(pid->integrator, -pid->Integrator_max, pid->Integrator_max);
	}

	pid->op = (float)(pid->kp * (float)error);
	pid->oi = (float)(pid->ki * (float)pid->integrator);
	pid->od = (float)(pid->kd * (float)(error - pid->last_error));

	pid->last_error = error;

	pid->out = pid->op + pid->oi + pid->od;
	if (pid->out_min != 0 || pid->out_max != 0) {
		pid->out = DEF_CONSTRAIN(pid->out, pid->out_min, pid->out_max);
	}
	return pid->out;
}

/**
 * @brief  pid增量式控制器输出
 * @param  pid     pid参数
 * @param  error   pid输入误差
 * @note   注意输出结果已经包涵了上次结果
 * @retval PID输出结果
 */
float math_pid_inc(math_pid_t *pid, float error)
{
	if (pid == NULL) {
		return 0;
	}

	pid->op = (float)(pid->kp * (float)(error - pid->last_error));
	pid->oi = (float)(pid->ki * (float)error);
	pid->od = (float)(pid->kd * (float)((error - pid->last_error) - pid->last_derivative));

	pid->last_derivative = error - pid->last_error;
	pid->last_error = error;

	pid->out += pid->op + pid->oi + pid->od;
	if (pid->out_min != 0 || pid->out_max != 0) {
		pid->out = DEF_CONSTRAIN(pid->out, pid->out_min, pid->out_max);
	}
	return pid->out;
}

void math_pid_set_output_restrict(math_pid_t *pid, float max, float min)
{
	if (pid == NULL) {
		return;
	}
	pid->out_max = max;
	pid->out_min = min;
}
