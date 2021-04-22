/*
 * ComUART.c
 *
 *  Created on: Apr 22, 2021
 *      Author: trocache
 */

// CubeIDE : choix de l'USART et choix du débit
// surtout ne pas valider les IT, on gère au niveau du module
// Réglage à la main sur le NVIC :
// 		  NVIC_EnableIRQ
//		  NVIC_SetPriority dans core_cm4.h
//      Les numéros IRQ sont donnés dans stm32l476xx.h
// Les inclusions:
// stm32l4xx_hal.h
// -> stm32l4xx_hal_conf.h
// 		-> stm32l4xx_hal_xxperiphxx.h comme par ex stm32l4xx_hal_uart.h
//			-> stm32l4xx_hal_def.h
//				-> stm32l4xx.h
//					-> stm32l476xx.h (les Irqn les typedef niveau registres
//						-> core_cm4.h (les fct NVIC, reg du coeur, Systick, nvic...)
//  Recup handler it au niveau startup.
#include "ComUART.h"


struct {
	char Cmde;      // le caractère filtré
	char CurrentByte; // caractère effectivement reçu non filtré
	char RecepFlag;  // indique qu'une commande est arrivée
	char TransmFlag; // indique qu'un print est en cours
	void (*ComUART_Callback)(void);
}ComUART_Status;


static UART_HandleTypeDef * ComUART_HandlerPtr;
static char Interrupt;

void ComUART_Init_IT(UART_HandleTypeDef * UsedUSART, void (*Callback)(void))
{
	ComUART_Status.Cmde=0; // aucune cmde valide
	ComUART_Status.CurrentByte=0;
    ComUART_Status.ComUART_Callback=Callback;
	ComUART_Status.RecepFlag=0;
	ComUART_Status.TransmFlag=0;
	ComUART_HandlerPtr=UsedUSART;
	Interrupt=1;
	// Configuration NVIC
	if ((UsedUSART->Instance)==USART1)
	{
		NVIC_EnableIRQ(USART1_IRQn);
		NVIC_SetPriority(USART1_IRQn,ComUART_Prio);
	}
	else if ((UsedUSART->Instance)==USART2)
	{
		NVIC_EnableIRQ(USART2_IRQn);
		NVIC_SetPriority(USART2_IRQn,ComUART_Prio);
	}
	else if ((UsedUSART->Instance)==USART3)
	{
		NVIC_EnableIRQ(USART3_IRQn);
		NVIC_SetPriority(USART3_IRQn,ComUART_Prio);
	}

	// Validation locale
	UsedUSART->Instance->CR1|=USART_CR1_RXNEIE;

}

void ComUART_Init(UART_HandleTypeDef * UsedUSART)
{
	ComUART_Status.Cmde=0; // aucune cmde valide
	ComUART_Status.CurrentByte=0;
    ComUART_Status.ComUART_Callback=0;
	ComUART_Status.RecepFlag=0;
	ComUART_Status.TransmFlag=0;
	ComUART_HandlerPtr=UsedUSART;
	Interrupt=0;
	// Configuration NVIC
	if ((UsedUSART->Instance)==USART1)
	{
		NVIC_EnableIRQ(USART1_IRQn);
		NVIC_SetPriority(USART1_IRQn,ComUART_Prio);
	}
	else if ((UsedUSART->Instance)==USART2)
	{
		NVIC_EnableIRQ(USART2_IRQn);
		NVIC_SetPriority(USART2_IRQn,ComUART_Prio);
	}
	else if ((UsedUSART->Instance)==USART3)
	{
		NVIC_EnableIRQ(USART3_IRQn);
		NVIC_SetPriority(USART3_IRQn,ComUART_Prio);
	}

	// Validation locale
	UsedUSART->Instance->CR1|=USART_CR1_RXNEIE;

}



void ComUART_Print(char * Str)
{

}


void ComUART_ClrReceptFlag(void)
{
	ComUART_Status.RecepFlag=0;
}

char ComUART_GetCmd(void)
{
	return ComUART_Status.Cmde;
}

char ComUART_IsNewCmde(void)
{
	return ComUART_Status.RecepFlag;
}


void USART2_IRQHandler(void)
{
	char Byte;
	ComUART_HandlerPtr->Instance->ICR|=USART_ICR_ORECF; // mise à 0 si l'IT provient d'un overrun
	Byte=ComUART_HandlerPtr->Instance->RDR;
	ComUART_Status.CurrentByte=Byte;
	switch (Byte)
	{
	case Uart_Up_R:ComUART_Status.Cmde=Byte;ComUART_Status.RecepFlag=1;break;
	case Uart_Up_L:ComUART_Status.Cmde=Byte;ComUART_Status.RecepFlag=1;break;
	case Uart_Down_R:ComUART_Status.Cmde=Byte;ComUART_Status.RecepFlag=1;break;
	case Uart_Down_L:ComUART_Status.Cmde=Byte;ComUART_Status.RecepFlag=1;break;
	case Uart_Left_R:ComUART_Status.Cmde=Byte;ComUART_Status.RecepFlag=1;break;
	case Uart_Left_L:ComUART_Status.Cmde=Byte;ComUART_Status.RecepFlag=1;break;
	case Uart_Right_R:ComUART_Status.Cmde=Byte;ComUART_Status.RecepFlag=1;break;
	case Uart_Right_L:ComUART_Status.Cmde=Byte;ComUART_Status.RecepFlag=1;break;
	default : ComUART_Status.Cmde=0;

	}

	if ((Interrupt==1) && (ComUART_Status.RecepFlag==1))
		{
		ComUART_Status.RecepFlag=0;
		ComUART_Status.ComUART_Callback();
		}
}