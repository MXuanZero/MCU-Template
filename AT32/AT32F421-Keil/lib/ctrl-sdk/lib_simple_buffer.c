/* Includes ------------------------------------------------------------------*/
#include "lib_simple_buffer.h"
#include <string.h>
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
#if LIB_BUFFER_DELAY_ENABLE
static void lib_simple_buffer_delay(uint32_t delay_time);
#endif // LIB_BUFFER_DELAY_ENABLE
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void lib_simple_buffer_init(lib_simple_buffer_t *handle, void *buf0, void *buf1)
{
	memset(handle, 0, sizeof(lib_simple_buffer_t));
	handle->buffer[0] = buf0;
	handle->buffer[1] = buf1;
}

bool lib_simple_buffer_read(lib_simple_buffer_t *handle, void **ppbuf,
			    time_ms_t delay)
{
	if (handle->avaliable[0]) {
		handle->r_index = 0;
	} else if (handle->avaliable[1]) {
		handle->r_index = 1;
	} else {
#if LIB_BUFFER_DELAY_ENABLE
		if (delay == 0) {
			return false;
		} else {
			lib_simple_buffer_delay(delay);
			return lib_simple_buffer_read(handle, ppbuf, 0);
		}
#else
		return false;
#endif // LIB_BUFFER_DELAY_ENABLE
	}
	*ppbuf = handle->buffer[handle->r_index];
	return true;
}

void lib_simple_buffer_read_done(lib_simple_buffer_t *handle)
{
	handle->avaliable[handle->r_index] = false;
}

void lib_simple_buffer_get_write_buf(lib_simple_buffer_t *handle, void **ppbuf)
{
	if (handle->w_index == handle->r_index) {
		handle->w_index = !handle->r_index;
	}
	*ppbuf = handle->buffer[handle->w_index];
}

void lib_simple_buffer_write_done(lib_simple_buffer_t *handle)
{
	handle->avaliable[handle->w_index] = true;
	handle->w_index = !handle->w_index;
}

#if LIB_BUFFER_DELAY_ENABLE
static inline void lib_simple_buffer_delay(time_ms_t delay_time)
{
	lib_delay_ms(delay_time);
}
#endif // LIB_BUFFER_DELAY_ENABLE
