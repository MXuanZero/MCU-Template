/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "lib_cfg.h"
#include "lib_stack.h"
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

#if LIB_MALLOC
#define STACK_MALLOC(x) LIB_MALLOC(x)
#else
#define STACK_MALLOC(x) malloc(x)
#endif

#if LIB_FREE
#define STACK_FREE(x) LIB_FREE(x)
#else
#define STACK_FREE(x) free(x)
#endif

#if LIB_INIT_EMPTY
#define STACK_INIT_EMPTY LIB_INIT_EMPTY
#else
#define STACK_INIT_EMPTY 1
#endif
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

lib_stack_status lib_stack_static_init(lib_stack_static_t *stack, void *members,
				       size_t mem_size, int32_t size)
{
	if (stack == NULL || members == NULL) {
		return LIB_STACK_ERROR;
	}
	if (size > LIB_STACK_STATIC_SIZE_MAX || size <= 0) {
		return LIB_STACK_ERROR;
	}
	if (stack->mem_addr != NULL) {
		return LIB_STACK_ALREADY_INIT;
	}
	stack->mem_addr = members;
	stack->mem_size = mem_size;
	stack->size_max = size;
	stack->top = -1;
	return LIB_STACK_OK;
}

inline _Bool lib_stack_static_empty(lib_stack_static_t *stack)
{
	if (stack == NULL) {
		return 0;
	}
	return lib_stack_static_size(stack) <= 0 ? LIB_STACK_IS_EMPTY :
						   LIB_STACK_IS_NOT_EMPTY;
}

inline _Bool lib_stack_static_full(lib_stack_static_t *stack)
{
	if (stack == NULL) {
		return 0;
	}
	return lib_stack_static_size(stack) >= stack->size_max ?
		       LIB_STACK_IS_FULL :
		       LIB_STACK_IS_NOT_FULL;
}

lib_stack_status lib_stack_static_push(lib_stack_static_t *stack, void *val)
{
	if (stack == NULL || val == NULL) {
		return LIB_STACK_ERROR;
	}
	if (stack->mem_size == 0 || stack->mem_addr == NULL) {
		return LIB_STACK_NOT_INIT;
	}
	if (lib_stack_static_full(stack) == LIB_STACK_IS_FULL) {
		return LIB_STACK_FULL;
	}
	++stack->top;
	memcpy((char *)stack->mem_addr + (stack->mem_size * stack->top),
	       val, stack->mem_size);
	return LIB_STACK_OK;
}

lib_stack_status lib_stack_static_pop(lib_stack_static_t *stack, void *val)
{
	if (stack == NULL) {
		return LIB_STACK_ERROR;
	}
	if (stack->mem_size == 0 || stack->mem_addr == NULL) {
		return LIB_STACK_NOT_INIT;
	}
	if (lib_stack_static_empty(stack) == LIB_STACK_IS_EMPTY) {
		return LIB_STACK_EMPTY;
	}
	if (val != NULL) {
		memcpy(val,
		       (char *)stack->mem_addr +
			       (stack->mem_size * stack->top),
		       stack->mem_size);
	}
	--stack->top;
	return LIB_STACK_OK;
}

lib_stack_status lib_stack_static_top(lib_stack_static_t *stack, void *val)
{
	if (stack == NULL || val == NULL) {
		return LIB_STACK_ERROR;
	}
	if (stack->mem_size == 0 || stack->mem_addr == NULL) {
		return LIB_STACK_NOT_INIT;
	}
	if (lib_stack_static_empty(stack) == LIB_STACK_IS_EMPTY) {
		return LIB_STACK_EMPTY;
	}
	memcpy(val,
	       (char *)stack->mem_addr + (stack->mem_size * stack->top),
	       stack->mem_size);
	return LIB_STACK_OK;
}

inline int32_t lib_stack_static_size(lib_stack_static_t *stack)
{
	if (stack == NULL) {
		return -1;
	}
	return stack->top + 1;
}

void lib_stack_static_clear(lib_stack_static_t *stack)
{
	if (stack == NULL) {
		return;
	}
	stack->top = -1;
}

lib_stack_status lib_stack_init(lib_stack_t *stack, size_t mem_size,
				int32_t size)
{
	if (stack == NULL) {
		return LIB_STACK_ERROR;
	}
	if (size > LIB_STACK_STATIC_SIZE_MAX || size <= 0) {
		return LIB_STACK_ERROR;
	}
	if (stack->mem_addr != NULL) {
		return LIB_STACK_ALREADY_INIT;
	}
	void *members = STACK_MALLOC(mem_size * size);
	if (members == NULL) {
		return LIB_STACK_ERROR;
	}
	stack->mem_addr = members;
	stack->mem_size = mem_size;
	stack->size_max = size;
	stack->top = -1;
	return LIB_STACK_OK;
}

inline _Bool lib_stack_empty(lib_stack_t *stack)
{
	if (stack == NULL) {
		return 0;
	}
	return lib_stack_size(stack) <= 0 ? LIB_STACK_IS_EMPTY :
					    LIB_STACK_IS_NOT_EMPTY;
}

inline _Bool lib_stack_full(lib_stack_t *stack)
{
	if (stack == NULL) {
		return 0;
	}
	return lib_stack_size(stack) >= stack->size_max ? LIB_STACK_IS_FULL :
							  LIB_STACK_IS_NOT_FULL;
}

lib_stack_status lib_stack_push(lib_stack_t *stack, void *val)
{
	if (stack == NULL || val == NULL) {
		return LIB_STACK_ERROR;
	}
	if (stack->mem_size == 0 || stack->mem_addr == NULL) {
		return LIB_STACK_NOT_INIT;
	}
	if (lib_stack_full(stack) == LIB_STACK_IS_FULL) {
		return LIB_STACK_FULL;
	}
	++stack->top;
	memcpy((char *)stack->mem_addr + (stack->mem_size * stack->top),
	       val, stack->mem_size);
	return LIB_STACK_OK;
}

lib_stack_status lib_stack_pop(lib_stack_t *stack, void *val)
{
	if (stack == NULL) {
		return LIB_STACK_ERROR;
	}
	if (stack->mem_size == 0 || stack->mem_addr == NULL) {
		return LIB_STACK_NOT_INIT;
	}
	if (lib_stack_empty(stack) == LIB_STACK_IS_EMPTY) {
		return LIB_STACK_EMPTY;
	}
	if (val != NULL) {
		memcpy(val,
		       (char *)stack->mem_addr +
			       (stack->mem_size * stack->top),
		       stack->mem_size);
	}
	--stack->top;
	return LIB_STACK_OK;
}

lib_stack_status lib_stack_top(lib_stack_t *stack, void *val)
{
	if (stack == NULL || val == NULL) {
		return LIB_STACK_ERROR;
	}
	if (stack->mem_size == 0 || stack->mem_addr == NULL) {
		return LIB_STACK_NOT_INIT;
	}
	if (lib_stack_empty(stack) == LIB_STACK_IS_EMPTY) {
		return LIB_STACK_EMPTY;
	}
	memcpy(val,
	       (char *)stack->mem_addr + (stack->mem_size * stack->top),
	       stack->mem_size);
	return LIB_STACK_OK;
}

inline int32_t lib_stack_size(lib_stack_t *stack)
{
	if (stack == NULL) {
		return -1;
	}
	return stack->top + 1;
}

void lib_stack_clear(lib_stack_t *stack)
{
	if (stack == NULL) {
		return;
	}
	stack->top = -1;
}

void lib_stack_delete(lib_stack_t *stack)
{
	if (stack == NULL) {
		return;
	}
	STACK_FREE(stack->mem_addr);
	stack->mem_addr = NULL;
}

lib_stack_status Lib_StackList_Init(Lib_StackList_t *stack, size_t mem_size)
{
	if (stack == NULL) {
		return LIB_STACK_ERROR;
	}
	if (stack->top_member != NULL) {
		return LIB_STACK_ALREADY_INIT;
	}
	stack->mem_size = mem_size;
	stack->top = -1;
	return LIB_STACK_OK;
}

_Bool Lib_StackList_Empty(Lib_StackList_t *stack, _Bool i)
{
	if (stack == NULL) {
		return 0;
	}
	return Lib_StackList_Size(stack, i) - 1 >= 0 ? LIB_STACK_IS_NOT_EMPTY :
						       LIB_STACK_IS_EMPTY;
}

_Bool Lib_StackList_Full(Lib_StackList_t *stack, _Bool i)
{
	if (stack == NULL) {
		return 0;
	}
	return Lib_StackList_Size(stack, i) >= INT32_MAX ?
		       LIB_STACK_IS_FULL :
		       LIB_STACK_IS_NOT_FULL;
}

lib_stack_status Lib_StackList_Push(Lib_StackList_t *stack, void *val)
{
	if (stack == NULL || val == NULL) {
		return LIB_STACK_ERROR;
	}
	if (stack->mem_size == 0) {
		return LIB_STACK_NOT_INIT;
	}
	if (Lib_StackList_Full(stack, 0) == LIB_STACK_IS_FULL) {
		return LIB_STACK_FULL;
	}

	lib_stack_member_t *member =
		(lib_stack_member_t *)STACK_MALLOC(LIB_STACK_MEMBER_SIZE);
	if (member == NULL) {
		return LIB_STACK_ERROR;
	}
#if DEF_LIB_STACK_INIT_EMPTY == 1
	memset(member, 0, LIB_STACK_MEMBER_SIZE);
#endif

	void *value = STACK_MALLOC(stack->mem_size);
	if (value == NULL) {
		STACK_FREE(member);
		STACK_FREE(value);
		return LIB_STACK_ERROR;
	}
	memcpy(value, val, stack->mem_size);

	if (stack->top_member == NULL) {
		member->val = value;
		stack->top_member = member;
	} else {
		lib_stack_member_t *prev_member = stack->top_member;
		member->prev = prev_member;
		member->val = value;
		stack->top_member = member;
	}
	++stack->top;
	return LIB_STACK_OK;
}

lib_stack_status Lib_StackList_Pop(Lib_StackList_t *stack, void *val)
{
	if (stack == NULL) {
		return LIB_STACK_ERROR;
	}
	if (stack->mem_size == 0) {
		return LIB_STACK_NOT_INIT;
	}
	if (Lib_StackList_Empty(stack, 0) == LIB_STACK_IS_EMPTY) {
		return LIB_STACK_EMPTY;
	}

	lib_stack_member_t *top_member = stack->top_member;
	if (val != NULL) {
		memcpy(val, top_member->val, stack->mem_size);
	}
	stack->top_member = top_member->prev;
	STACK_FREE(top_member->val);
	STACK_FREE(top_member);
	--stack->top;
	return LIB_STACK_OK;
}

lib_stack_status Lib_StackList_Top(Lib_StackList_t *stack, void *val)
{
	if (stack == NULL || val == NULL) {
		return LIB_STACK_ERROR;
	}
	if (stack->mem_size == 0) {
		return LIB_STACK_NOT_INIT;
	}
	if (Lib_StackList_Empty(stack, 0) == LIB_STACK_IS_EMPTY) {
		return LIB_STACK_EMPTY;
	}

	lib_stack_member_t *top_member = stack->top_member;
	memcpy(val, top_member->val, stack->mem_size);
	return LIB_STACK_OK;
}

inline int32_t Lib_StackList_Size(Lib_StackList_t *stack, _Bool mod)
{
	if (stack == NULL) {
		return -1;
	}
	if (mod == 1) {
		int32_t num = -1;
		for (lib_stack_member_t *member = stack->top_member;
		     member != NULL; member = member->prev) {
			++num;
		}
		return num + 1;
	} else {
		return stack->top + 1;
	}
}

void Lib_StackList_Clear(Lib_StackList_t *stack)
{
	if (stack == NULL) {
		return;
	}
	while (Lib_StackList_Size(stack, 0) > 0) {
		Lib_StackList_Pop(stack, NULL);
	}
}

void Lib_StackList_Delete(Lib_StackList_t *stack)
{
	if (stack == NULL) {
		return;
	}
	while (Lib_StackList_Size(stack, 0) > 0) {
		Lib_StackList_Pop(stack, NULL);
	}
}
