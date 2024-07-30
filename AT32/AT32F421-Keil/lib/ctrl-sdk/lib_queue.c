/* include */
#include <string.h>
#include "lib_queue.h"
/* define */

/* typedef */

/* variables */

/* function prototypes */
lib_queue_status lib_queue_static_init(lib_queue_static_t *queue, void *members,
				      size_t member_size, int32_t size)
{
	if (queue == NULL || members == NULL) {
		return LIB_QUEUE_STATE_ERROR;
	}
	if (size <= 0) {
		return LIB_QUEUE_STATE_ERROR;
	}
	if (queue->addr != NULL) {
		return LIB_QUEUE_STATE_ALREADY_INIT;
	}
	queue->addr = members;
	queue->member_size = member_size;
	queue->size_max = size;
	queue->size = 0;
	queue->back = 0;
	queue->front = 0;
	return LIB_QUEUE_STATE_TRUE;
}

inline int32_t lib_queue_static_size(lib_queue_static_t *queue)
{
	if (queue == NULL) {
		return -1;
	}
	return queue->size;
}

inline bool lib_queue_static_empty(lib_queue_static_t *queue)
{
	if (queue == NULL) {
		return 0;
	}
	return lib_queue_static_size(queue) <= 0 ? LIB_QUEUE_IS_EMPTY :
						  LIB_QUEUE_IS_NOT_EMPTY;
}

inline bool lib_queue_static_full(lib_queue_static_t *queue)
{
	if (queue == NULL) {
		return 0;
	}
	return lib_queue_static_size(queue) >= queue->size_max ?
		       LIB_QUEUE_IS_FULL :
		       LIB_QUEUE_IS_NOT_FULL;
}

lib_queue_status lib_queue_static_push(lib_queue_static_t *queue, void *val)
{
	if (queue == NULL || val == NULL) {
		return LIB_QUEUE_STATE_ERROR;
	}
	if (queue->member_size == 0 || queue->addr == NULL) {
		return LIB_QUEUE_STATE_NOT_INIT;
	}
	if (lib_queue_static_full(queue) == LIB_QUEUE_IS_FULL) {
		return LIB_QUEUE_STATE_FULL;
	}
	memcpy((char *)queue->addr + (queue->member_size * queue->back), val,
	       queue->member_size);
	++queue->size;
	++queue->back;
	if (queue->back >= queue->size_max) {
		queue->back = 0;
	}
	return LIB_QUEUE_STATE_TRUE;
}

lib_queue_status lib_queue_static_pop(lib_queue_static_t *queue, void *val)
{
	if (queue == NULL) {
		return LIB_QUEUE_STATE_ERROR;
	}
	if (queue->member_size == 0 || queue->addr == NULL) {
		return LIB_QUEUE_STATE_NOT_INIT;
	}
	if (lib_queue_static_empty(queue) == LIB_QUEUE_IS_EMPTY) {
		if (val != NULL) {
			memset(val, 0, queue->member_size);
		}
		return LIB_QUEUE_STATE_EMPTY;
	}
	if (val != NULL) {
		memcpy(val,
		       (char *)queue->addr +
			       (queue->member_size * queue->front),
		       queue->member_size);
	}
	--queue->size;
	++queue->front;
	if (queue->front >= queue->size_max) {
		queue->front = 0;
	}
	return LIB_QUEUE_STATE_TRUE;
}

lib_queue_status lib_queue_static_front(lib_queue_static_t *queue, void *val)
{
	if (queue == NULL || val == NULL) {
		return LIB_QUEUE_STATE_ERROR;
	}
	if (queue->member_size == 0 || queue->addr == NULL) {
		return LIB_QUEUE_STATE_NOT_INIT;
	}
	if (lib_queue_static_empty(queue) == LIB_QUEUE_IS_EMPTY) {
		return LIB_QUEUE_STATE_EMPTY;
	}

	memcpy(val, (char *)queue->addr + (queue->member_size * queue->front),
	       queue->member_size);
	return LIB_QUEUE_STATE_TRUE;
}

lib_queue_status lib_queue_static_back(lib_queue_static_t *queue, void *val)
{
	if (queue == NULL || val == NULL) {
		return LIB_QUEUE_STATE_ERROR;
	}
	if (queue->member_size == 0 || queue->addr == NULL) {
		return LIB_QUEUE_STATE_NOT_INIT;
	}
	if (lib_queue_static_empty(queue) == LIB_QUEUE_IS_EMPTY) {
		return LIB_QUEUE_STATE_EMPTY;
	}
	int32_t pos = queue->back - 1 < 0 ? queue->size_max - 1 :
					    queue->back - 1;
	memcpy(val, (char *)queue->addr + (queue->member_size * (pos)),
	       queue->member_size);
	return LIB_QUEUE_STATE_TRUE;
}

void lib_queue_static_clear(lib_queue_static_t *queue)
{
	if (queue == NULL) {
		return;
	}
	queue->size = 0;
	queue->front = 0;
	queue->back = 0;
}

lib_queue_status lib_queue_static_delete(lib_queue_static_t *queue)
{
	if (queue == NULL) {
		return LIB_QUEUE_STATE_ERROR;
	}
	queue->addr = NULL;
	queue->member_size = 0;
	queue->size_max = 0;
	queue->size = 0;
	queue->back = 0;
	queue->front = 0;
	return LIB_QUEUE_STATE_TRUE;
}
