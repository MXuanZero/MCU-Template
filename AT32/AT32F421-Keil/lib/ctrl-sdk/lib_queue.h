#ifndef LIB_QUEUE_H
#define LIB_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

/* include */
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

/* define */
#define LIB_QUEUE_IS_EMPTY 1
#define LIB_QUEUE_IS_NOT_EMPTY 0
#define LIB_QUEUE_IS_FULL 1
#define LIB_QUEUE_IS_NOT_FULL 0

/* typedef */
typedef enum lib_queue_status {
	LIB_QUEUE_STATE_TRUE = 0,
	LIB_QUEUE_STATE_FALSE,
	LIB_QUEUE_STATE_NOT_INIT,
	LIB_QUEUE_STATE_ALREADY_INIT,
	LIB_QUEUE_STATE_FULL,
	LIB_QUEUE_STATE_EMPTY,
	LIB_QUEUE_STATE_ERROR,
} lib_queue_status;

typedef struct lib_queue_static {
	void *addr;
	size_t member_size; // 单个元素的大小
	int32_t size_max; // 容积
	int32_t size; // 目前队列中的元素
	int32_t front; // 队列中第一个元素
	int32_t back; // 队列中最后一个元素
} lib_queue_static_t;

/* function prototypes */
/* static */
lib_queue_status lib_queue_static_init(lib_queue_static_t *queue, void *members,
				      size_t member_size, int32_t size);
bool lib_queue_static_empty(lib_queue_static_t *queue);
bool lib_queue_static_full(lib_queue_static_t *queue);
lib_queue_status lib_queue_static_push(lib_queue_static_t *queue, void *val);
lib_queue_status lib_queue_static_pop(lib_queue_static_t *queue, void *val);
lib_queue_status lib_queue_static_front(lib_queue_static_t *queue, void *val);
lib_queue_status lib_queue_static_back(lib_queue_static_t *queue, void *val);
int32_t lib_queue_static_size(lib_queue_static_t *queue);
void lib_queue_static_clear(lib_queue_static_t *queue);
lib_queue_status lib_queue_static_delete(lib_queue_static_t *queue);

#ifdef __cplusplus
}
#endif

#endif //__LIB_QUEUE_H__
