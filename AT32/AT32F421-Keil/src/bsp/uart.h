#ifndef UART1_H
#define UART1_H

/* Includes --------------------------------------------------------------------------------------*/
#include <stdint.h>
/* Define ----------------------------------------------------------------------------------------*/ 
/* Exported macro --------------------------------------------------------------------------------*/
/* Exported typedef ------------------------------------------------------------------------------*/
/* Exported constants ----------------------------------------------------------------------------*/
/* Exported functions ----------------------------------------------------------------------------*/
void bsp_uart_init(void);
uint16_t bsp_uart_dma_get_dtcnt(void);
void bsp_uart_dma_send_data(const uint8_t *data, uint16_t len);
#endif /* UART1_H */
