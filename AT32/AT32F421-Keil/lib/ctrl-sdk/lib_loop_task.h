#ifndef LIB_LOOP_TASK_H
#define LIB_LOOP_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
/* Define --------------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
typedef void (*lib_loop_task_action_fn)(void);

typedef struct lib_loop_task {
	lib_loop_task_action_fn enter_fn;
	lib_loop_task_action_fn loop_fn;
	lib_loop_task_action_fn exit_fn;
	uint8_t id;
} lib_loop_task_t;

typedef struct lib_loop_task_handle {
	lib_loop_task_t *tasks;
	uint8_t cur; // current task
	uint8_t next; // next task
	uint8_t num; // number of tasks
	bool enable;
} lib_loop_task_group_t;
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void lib_loop_task_init(lib_loop_task_t *handle, uint8_t len,
			lib_loop_task_action_fn enter_fn,
			lib_loop_task_action_fn loop_fn,
			lib_loop_task_action_fn exit_fn);
void lib_loop_task_group_init1(lib_loop_task_group_t *group);
void lib_loop_task_group_init2(lib_loop_task_t *tasks, uint8_t num);
void lib_loop_task_next(uint8_t next_id);
void lib_loop_task_handler(void);

#ifdef __cplusplus
}
#endif

#endif // LIB_LOOP_TASK_H
