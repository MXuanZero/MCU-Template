#ifndef LIB_FSM_H
#define LIB_FSM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
/* Define --------------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
typedef enum {
	LIB_FSM_STATE_CHANGED,
	LIB_FSM_NO_CHANGE,
	LIB_FSM_ERROR,
} lib_fsm_status;

typedef struct lib_fsm_state_t lib_fsm_state_t;
typedef struct lib_fsm_transition lib_fsm_transition_t;

typedef struct lib_fsm_event_t {
	void *data;
	int32_t type; // 状态
} lib_fsm_event_t;

typedef void (*lib_fsm_action_fn)(lib_fsm_event_t *event, void *param);

/**
 * @brief
 * @param param1 结构体Lib_Fsm_Transition_t的成员ConditionParam
 * @param param2 函数Lib_Fsm_PushEvent的入参condition_param
 * @return 返回1为判断通过，可进入下一状态
 */
typedef bool (*lib_fsm_guard_fn)(lib_fsm_event_t *e, void *p1, void *p2);

typedef struct lib_fsm_transition {
	void *param; // 见 lib_fsm_guard_fn 注释
	int32_t event; // 事件
	lib_fsm_guard_fn guard_fn; // 见 lib_fsm_guard_fn 注释
	lib_fsm_action_fn action_fn; // 状态过渡回调
	lib_fsm_state_t *state; // 下一状态
	lib_fsm_transition_t *next;
} lib_fsm_transition_t;

typedef struct lib_fsm_state_t {
	char *name;
	lib_fsm_action_fn entry_fn; // 进入函数
	lib_fsm_action_fn exit_fn; // 退出函数
	lib_fsm_transition_t *transitions;
	uint16_t num; // 有多少下一状态
} lib_fsm_state_t;

typedef struct lib_fsm_t {
	lib_fsm_state_t *current;
	lib_fsm_state_t *prev;
	lib_fsm_state_t *error;
} lib_fsm_t;
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief 初始化函数
 */
void lib_fsm_init(lib_fsm_t *fsm, lib_fsm_state_t *init_state,
		  lib_fsm_state_t *error_state);

void lib_fsm_state_init_static(lib_fsm_state_t *state, char *name,
			       lib_fsm_action_fn entry_fn,
			       lib_fsm_action_fn exit_fn);

void lib_fsm_reg_transition(lib_fsm_state_t *state, lib_fsm_state_t *next,
			    int32_t event, void *param,
			    lib_fsm_guard_fn guard_fn,
			    lib_fsm_action_fn action_fn);

void lib_fsm_reg_transition_static(lib_fsm_state_t *state,
				   lib_fsm_state_t *next,
				   lib_fsm_transition_t *transition);

/**
 * @brief 更新状态
 * @note  如果下一状态任然是当前状态，那么不执行进入状态和退出状态的回调函数，
 * 但状态过渡的回调函数依然会执行
 * @param condition_param 状态切换的条件参数
 * @param action_param    状态转换参数
 * @return
 */
lib_fsm_status lib_fsm_push_event(lib_fsm_t *fsm, lib_fsm_event_t *event,
				  void *condition_param, void *action_param);

/**
 * @brief 获取当前状态
 */
lib_fsm_state_t *lib_fsm_get_current_state(lib_fsm_t *fsm);

/**
 * @brief 获取上一个状态
 */
lib_fsm_state_t *lib_fsm_get_prev_state(lib_fsm_t *fsm);

#ifdef __cplusplus
}
#endif

#endif // LIB_FSM_H
