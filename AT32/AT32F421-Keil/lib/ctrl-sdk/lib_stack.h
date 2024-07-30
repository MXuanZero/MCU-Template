#ifndef LIB_STACK_H
#define LIB_STACK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "ctrl_defs.h"

/* Define --------------------------------------------------------------------*/

#define LIB_STACK_MEMBER_SIZE sizeof(lib_stack_member_t)
#define LIB_STACK_IS_EMPTY 1
#define LIB_STACK_IS_NOT_EMPTY 0
#define LIB_STACK_IS_FULL 1
#define LIB_STACK_IS_NOT_FULL 0

/* Exported macro ------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/

typedef enum {
	LIB_STACK_OK = 0,
	LIB_STACK_ERROR,
	LIB_STACK_NOT_INIT,
	LIB_STACK_ALREADY_INIT,
	LIB_STACK_FULL,
	LIB_STACK_EMPTY,
} lib_stack_status;

typedef struct lib_stack_static {
	void *mem_addr;
	size_t mem_size;
	int32_t size_max;
	int32_t top;
} lib_stack_static_t;

typedef struct lib_stack {
	void *mem_addr;
	size_t mem_size;
	int32_t size_max;
	int32_t top;
} lib_stack_t;

typedef struct lib_stack_member {
	struct lib_stack_member *prev;
	void *val;
} lib_stack_member_t;

typedef struct lib_stack_list_t {
	lib_stack_member_t *top_member;
	size_t mem_size;
	int32_t top;
} Lib_StackList_t;

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/* static */
lib_stack_status lib_stack_static_init(lib_stack_static_t *stack, void *members,
				       size_t member_size, int32_t size);
_Bool lib_stack_static_empty(lib_stack_static_t *stack);
_Bool lib_stack_static_full(lib_stack_static_t *stack);
lib_stack_status lib_stack_static_push(lib_stack_static_t *stack, void *val);
lib_stack_status lib_stack_static_pop(lib_stack_static_t *stack, void *val);
lib_stack_status lib_stack_static_top(lib_stack_static_t *stack, void *val);
int32_t lib_stack_static_size(lib_stack_static_t *stack);
void lib_stack_static_clear(lib_stack_static_t *stack);

/* dynamic */
lib_stack_status lib_stack_init(lib_stack_t *stack, size_t member_size,
				int32_t size);
bool lib_stack_empty(lib_stack_t *stack);
bool lib_stack_full(lib_stack_t *stack);
lib_stack_status lib_stack_push(lib_stack_t *stack, void *val);
lib_stack_status lib_stack_pop(lib_stack_t *stack, void *val);
lib_stack_status lib_stack_top(lib_stack_t *stack, void *val);
int32_t lib_stack_size(lib_stack_t *stack);
void lib_stack_clear(lib_stack_t *stack);
void lib_stack_delete(lib_stack_t *stack);

/* list */
lib_stack_status Lib_StackList_Init(Lib_StackList_t *stack, size_t member_size);
bool Lib_StackList_Empty(Lib_StackList_t *stack, _Bool i);
bool Lib_StackList_Full(Lib_StackList_t *stack, _Bool i);
lib_stack_status Lib_StackList_Push(Lib_StackList_t *stack, void *val);
lib_stack_status Lib_StackList_Pop(Lib_StackList_t *stack, void *val);
lib_stack_status Lib_StackList_Top(Lib_StackList_t *stack, void *val);
int32_t Lib_StackList_Size(Lib_StackList_t *stack, _Bool mod);
void Lib_StackList_Clear(Lib_StackList_t *stack);
void Lib_StackList_Delete(Lib_StackList_t *stack);

#ifdef __cplusplus
}
#endif

#endif // LIB_STACK_H
