#ifndef VOFA_H
#define VOFA_H
/* Includes --------------------------------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "ctrl_defs.h"
#include "lib_timer.h"
/* Define ----------------------------------------------------------------------------------------*/
#define VOFA_CH_CNT 10// 通道数量
/* Exported macro --------------------------------------------------------------------------------*/
/* Exported typedef ------------------------------------------------------------------------------*/
typedef struct vofa {
    float fdata[VOFA_CH_CNT];
    uint8_t tail[4];
} vofa_t;
/* Exported constants ----------------------------------------------------------------------------*/
extern lib_timer_t curve_task_handle;
/* Exported functions ----------------------------------------------------------------------------*/
void vofa_init(void);
void vofa_set_data(uint8_t ch, float data);
void vofa_set_data_va(int ch_num, ...);
void vofa_send_data(void);
#endif // VOFA_H
