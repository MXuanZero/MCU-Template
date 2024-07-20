#ifndef SPI2_H
#define SPI2_H
/* Includes --------------------------------------------------------------------------------------*/
#include <stdint.h>

/* Define ----------------------------------------------------------------------------------------*/ 
/* Exported macro --------------------------------------------------------------------------------*/
/* Exported typedef ------------------------------------------------------------------------------*/
/* Exported constants ----------------------------------------------------------------------------*/
/* Exported functions ----------------------------------------------------------------------------*/

void spi2_init(void);
void spi2_send_data_16bit(uint16_t *data, uint16_t len);
void spi2_recv_data_16bit(uint16_t *data, uint16_t len);

#endif // SPI2_H
