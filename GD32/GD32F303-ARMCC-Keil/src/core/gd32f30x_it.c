#include "systick.h"

/**
 * @brief this function handles BusFault exception
 **/
void NMI_Handler(void)
{
}

/**
 * @brief this function handles BusFault exception
 */
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/**
 * @brief this function handles BusFault exception
 */
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while (1){
    }
}

/**
 * @brief this function handles BusFault exception
 */
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/**
 * @brief this function handles BusFault exception
 */
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/**
 * @brief this function handles BusFault exception
 */
void SVC_Handler(void)
{
}

/**
 * @brief this function handles BusFault exception
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief this function handles BusFault exception
 */
void PendSV_Handler(void)
{
}

/**
 * @brief this function handles BusFault exception
 */
void SysTick_Handler(void)
{
    delay_decrement();
}
