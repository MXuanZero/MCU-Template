/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include <string.h>
#include "hal_ccp_recv.h"
#include "lib_assert.h"
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void hal_ccp_recv_init(hal_ccp_recv_handle_t *recv, uint8_t id,
		       hal_ccp_recv_processing_fn fn,
		       hal_ccp_recv_update_fn update_fn)
{
	lib_assert(recv == NULL);
	lib_assert(recv->id != 0 || id == 0);
	lib_assert(fn == NULL || update_fn == NULL);

	if (recv == NULL) {
		return;
	}

	memset(recv, 0, sizeof(hal_ccp_recv_handle_t));
	recv->id = id;
	recv->processing_fn = fn;
	recv->update_fn = update_fn;
	lib_queue_static_init(&recv->queue, recv->queue_buffer,
			      sizeof(hal_ccp_recv_data_t *),
			      HAL_CCP_RECV_BUF_NUM);
	hal_ccp_recv_prot_init(recv);
	recv->update_fn(&recv->data[recv->p]);
}

void hal_ccp_recv_group_reg(hal_ccp_recv_group_handle_t *group,
			    hal_ccp_recv_handle_t *recv)
{
	if (group == NULL || recv == NULL) {
		return;
	}
	uint8_t id = recv->id;
	for (hal_ccp_recv_handle_t *pos = group->first; pos != NULL;
	     pos = pos->next) {
		if (pos->id == id) {
			return;
		}
	}
	if (group->first == NULL) {
		group->first = recv;
	} else {
		hal_ccp_recv_handle_t *pos = group->first;
		while (pos->next != NULL) {
			pos = pos->next;
		}
		pos->next = recv;
	}
	++group->num;
}

hal_ccp_recv_status hal_ccp_recv_handler(hal_ccp_recv_handle_t *recv, uint16_t rx_len)
{
	lib_queue_status state;
	hal_ccp_recv_data_t *data;
	if (recv == NULL) {
		return HAL_CCP_RECV_ERROR;
	}

	HAL_CCP_RECV_LOCK();
	data = &recv->data[recv->p]; // 保存当前位置
	data->size = rx_len;
	state = lib_queue_static_push(&recv->queue, &data);
	recv->p++;
	recv->p = recv->p < HAL_CCP_RECV_BUF_NUM ? recv->p : 0;
	recv->update_fn(&recv->data[recv->p]);
	HAL_CCP_RECV_UNLOCK();
	if (state == LIB_QUEUE_STATE_TRUE) {
		return HAL_CCP_RECV_TRUE;
	} else if (state == LIB_QUEUE_STATE_FULL) {
		return HAL_CCP_RECV_OVERFLOW;
	} else {
		return HAL_CCP_RECV_ERROR;
	}
}

void hal_ccp_recv_processor(hal_ccp_recv_group_handle_t *center)
{
	if (center == NULL) {
		return;
	}
	/* 数据处理 */
	hal_ccp_recv_data_t *data = NULL;
	lib_queue_status state = LIB_QUEUE_STATE_EMPTY;
	for (hal_ccp_recv_handle_t *recv = center->first; recv != NULL;
	     recv = recv->next) {
HAL_CCP_RECV_EACH_MEMBER:
		HAL_CCP_RECV_LOCK();
		state = lib_queue_static_pop(&recv->queue, &data);
		HAL_CCP_RECV_UNLOCK();
		switch (state) {
		case LIB_QUEUE_STATE_TRUE:
			recv->processing_fn(data->data, data->size);
			/* 遍历队列成员，直到把所待处理的数据全部处理完 */
			/* @warning 发送数据过快可能会导致一直卡在这一段程序 */
			goto HAL_CCP_RECV_EACH_MEMBER;
		case LIB_QUEUE_STATE_EMPTY:
			continue;
		default:
			/* error */
			break;
		}
	}
}
