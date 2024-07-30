/* Includes ------------------------------------------------------------------*/
#include "lib_loop_task.h"
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static lib_loop_task_group_t *lib_loop_task_group = NULL;
/* Private functions ---------------------------------------------------------*/
inline void lib_loop_task_group_init1(lib_loop_task_group_t *group)
{
	if (group == NULL) {
		return;
	}
	lib_loop_task_group = group;
}

inline void lib_loop_task_group_init2(lib_loop_task_t *tasks, uint8_t num)
{
	if (tasks == NULL) {
		return;
	}

	lib_loop_task_group->tasks = tasks;
	lib_loop_task_group->num = num;
}

inline void lib_loop_task_init(lib_loop_task_t *handle, uint8_t id,
			       lib_loop_task_action_fn enter_fn,
			       lib_loop_task_action_fn loop_fn,
			       lib_loop_task_action_fn exit_fn)
{
	if (handle == NULL) {
		return;
	}
	handle->id = id;
	handle->enter_fn = enter_fn;
	handle->loop_fn = loop_fn;
	handle->exit_fn = exit_fn;
}

void lib_loop_task_next(uint8_t next_id)
{
	lib_loop_task_group->next = next_id;
}

void lib_loop_task_start(void)
{
	lib_loop_task_group->enable = true;
}

void lib_loop_task_stop(void)
{
	lib_loop_task_group->enable = false;
}

void lib_loop_task_handler(void)
{
	lib_loop_task_group_t *group = lib_loop_task_group;
	if (group == NULL) {
		return;
	}
	if (group->enable == true) {
		return;
	}
	if (group->cur != group->next) {
		if (group->tasks[group->cur].exit_fn) {
			group->tasks[group->cur].exit_fn();
		}
		if (group->tasks[group->next].enter_fn) {
			group->tasks[group->next].enter_fn();
		}
		group->cur = group->next;
	}
	if (group->tasks[group->cur].loop_fn) {
		group->tasks[group->cur].loop_fn();
	}
}
