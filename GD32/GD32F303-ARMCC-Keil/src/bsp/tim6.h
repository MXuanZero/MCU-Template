#ifndef TIM6_H
#define TIM6_H
/* Includes --------------------------------------------------------------------------------------*/
#include <stdint.h>
/* Define ----------------------------------------------------------------------------------------*/ 
/* Exported macro --------------------------------------------------------------------------------*/
/* Exported typedef ------------------------------------------------------------------------------*/
/* Exported constants ----------------------------------------------------------------------------*/
/* Exported functions ----------------------------------------------------------------------------*/
void tim6_init(void);
void tim6_callback_10khz(void);
#endif // TIM6_H