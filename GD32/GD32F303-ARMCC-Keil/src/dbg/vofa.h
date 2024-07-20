#ifndef VOFA_H
#define VOFA_H
/* Includes --------------------------------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include <ctrl_defs.h>
/* Define ----------------------------------------------------------------------------------------*/
/* Exported macro --------------------------------------------------------------------------------*/
/* Exported typedef ------------------------------------------------------------------------------*/

#define VOFA_CH_CNT 10// 通道数量
typedef struct vofa {
    float fdata[VOFA_CH_CNT];
    uint8_t tail[4];
} vofa_t;
/* Exported constants ----------------------------------------------------------------------------*/
/* Exported functions ----------------------------------------------------------------------------*/
void vofa_set_data(uint8_t ch, float data);
void vofa_set_data_va(int ch_num, ...);
void vofa_send_data(void);
#endif // VOFA_H
