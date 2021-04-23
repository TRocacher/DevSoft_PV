/*
 * ComUART.h
 *
 *  Created on: Apr 22, 2021
 *      Author: trocache
 */

#ifndef COMUART_COMUART_H_
#define COMUART_COMUART_H_

#include "stm32l4xx_hal.h"

void ComUART_Init_IT(UART_HandleTypeDef * UsedUSART, void (*Callback)(void));
char ComUART_GetCmd(void);

void ComUART_Init(UART_HandleTypeDef * UsedUSART);
char ComUART_IsNewCmde(void);
void ComUART_ClrReceptFlag(void);


// pour les droitiers
#define Uart_Up_R 'o'
#define Uart_Down_R 'l'
#define Uart_Left_R 'k'
#define Uart_Right_R 'm'
// pour les gauchers
#define Uart_Up_L 'z'
#define Uart_Down_L 's'
#define Uart_Left_L 'q'
#define Uart_Right_L 'd'

#define ComUART_Prio 6

#endif /* COMUART_COMUART_H_ */
