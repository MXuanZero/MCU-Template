/* 一次接收完所有数据，实现方式有点蠢，待以后修改 */
#ifndef HAL_CCP_RECV_H
#define HAL_CCP_RECV_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "lib_queue.h"
/* Define --------------------------------------------------------------------*/
#ifndef HAL_CCP_RECV_DATA_SIZE
#define HAL_CCP_RECV_DATA_SIZE 64
#endif

#ifndef HAL_CCP_RECV_BUF_NUM
#define HAL_CCP_RECV_BUF_NUM 3
#endif
/* Exported macro ------------------------------------------------------------*/
#if HAL_CCP_RECV_LOCK_ENABLE == 1
#define HAL_CCP_RECV_LOCK() hal_ccp_recv_prot_lock() // 上锁
#define HAL_CCP_RECV_UNLOCK() hal_ccp_recv_prot_unlock() // 解锁
#else
#define HAL_CCP_RECV_LOCK()
#define HAL_CCP_RECV_UNLOCK()
#endif
/* Exported types ------------------------------------------------------------*/
typedef enum hal_ccp_recv_status {
	HAL_CCP_RECV_TRUE = 0,
	HAL_CCP_RECV_ERROR,
	HAL_CCP_RECV_OVERFLOW,
} hal_ccp_recv_status;

typedef struct hal_ccp_recv_data_t {
	uint16_t size;
	uint8_t data[HAL_CCP_RECV_DATA_SIZE];
} hal_ccp_recv_data_t;

typedef void (*hal_ccp_recv_processing_fn)(uint8_t *, uint16_t);

/* 需要再此函数中更新original的长			度，更新dma接收的存储地址 */
typedef void (*hal_ccp_recv_update_fn)(hal_ccp_recv_data_t *next);

typedef struct hal_ccp_recv_handle_t {
	struct hal_ccp_recv_handle_t *next; // 下一指针
	hal_ccp_recv_processing_fn processing_fn; // 数据处理函数
	hal_ccp_recv_update_fn update_fn;

	
	
	hal_ccp_recv_data_t data[HAL_CCP_RECV_BUF_NUM];

	lib_queue_static_t queue; // 输出队列
	hal_ccp_recv_data_t *queue_buffer[HAL_CCP_RECV_BUF_NUM]; // 缓存
	uint16_t p;
	uint8_t id;
} hal_ccp_recv_handle_t;

typedef struct hal_ccp_recv_group_handle_t {
	struct hal_ccp_recv_handle_t *first;
	uint8_t num;
} hal_ccp_recv_group_handle_t;
/* Exported constants --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
 * @brief 初始化
 */
void hal_ccp_recv_init(hal_ccp_recv_handle_t *recv, uint8_t id,
		       hal_ccp_recv_processing_fn fn,
		       hal_ccp_recv_update_fn update_fn);

/**
 * @brief 注册
 */
void hal_ccp_recv_group_reg(hal_ccp_recv_group_handle_t *group,
			    hal_ccp_recv_handle_t *recv);

/**
 * @brief 数据接收
 * @note  一般写在接收完成中断中
 */
hal_ccp_recv_status hal_ccp_recv_handler(hal_ccp_recv_handle_t *recv, uint16_t rx_len);

/**
 * @brief 输出处理
 * @note  一般写在循环中
 */
void hal_ccp_recv_processor(hal_ccp_recv_group_handle_t *center);

/**
 * @brief 初始化
 */
void hal_ccp_recv_prot_init(hal_ccp_recv_handle_t *recv);

/**
 * @brief 检查错误
 */
_Bool hal_ccp_recv_prot_check_errors(hal_ccp_recv_handle_t *recv);

/**
 * @brief 上锁
 */
void hal_ccp_recv_prot_lock(void);

/**
 * @brief 解锁
 */
void hal_ccp_recv_prot_unlock(void);

#ifdef __cplusplus
}
#endif

#endif // HAL_CCP_RECV_H
