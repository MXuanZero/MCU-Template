#ifndef USART2_DMA_H
#define USART2_DMA_H
/* Includes --------------------------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
/* Define ----------------------------------------------------------------------------------------*/
/* Exported macro --------------------------------------------------------------------------------*/
/* Exported typedef ------------------------------------------------------------------------------*/
/* Exported constants ----------------------------------------------------------------------------*/
/* Exported functions ----------------------------------------------------------------------------*/
void usart2_dma_init(void);

/* send */
void usart2_send_data(uint8_t* data, uint16_t len);
void usart2_dma_send_data(const uint8_t* data, uint16_t len);
uint16_t usart2_dma_send_len(void);
void usart2_dma0_ch1_iqr(void);

/* recv */
uint16_t usart2_dma_recv_len(uint16_t rx_max);
void usart2_dma_recv_data(const uint8_t* rx_data, uint16_t len, bool cmen);
void usart2_dma0_ch2_iqr(void);
void usart2_interrupt_idle_cb(void);   // 串口空闲中断回调

#endif // USART2_DMA_H
