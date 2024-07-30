#ifndef LIB_BUFFER_H
#define LIB_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lib_tick.h"
/* Define --------------------------------------------------------------------*/
#define LIB_BUFFER_DELAY_ENABLE 1
/* Exported macro ------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
typedef struct lib_simple_buffer {
	void *buffer[2];
	volatile uint8_t avaliable[2];
	volatile uint8_t w_index;
	volatile uint8_t r_index;
} lib_simple_buffer_t;
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Ping-pong buffer initialization
 * @retval None
 */
void lib_simple_buffer_init(lib_simple_buffer_t *handle, void *buf0, void *buf1);

/**
 * @brief  Get a readable buffer
 * @retval Returns true if there is a buffer to be read
 */
bool lib_simple_buffer_read(lib_simple_buffer_t *handle, void **ppbuf, time_ms_t delay);

/**
 * @brief  Notify buffer read completion
 */
void lib_simple_buffer_read_done(lib_simple_buffer_t *handle);

/**
 * @brief  Get writable buffer
 * @retval None
 */
void lib_simple_buffer_get_write_buf(lib_simple_buffer_t *handle, void **ppbuf);

/**
 * @brief  Notify buffer write completion
 */
void lib_simple_buffer_write_done(lib_simple_buffer_t *handle);


#ifdef __cplusplus
}
#endif

#endif // LIB_BUFFER_H
