/*
 * ComUART.c
 *
 *  Created on: Apr 22, 2021
 *      Author: trocache
 *  CIBLE : stm32L476RG (Nucléo board)
 *  IDE : Cube IDE vers 1.5.1
 *  CONTEXTE : pilotage, régulation de systèmes de puissance. Partie analogique
 *  DEPENDENCES : Le code s'appuie sur les registres du stm32L476 donc
 *  essentiellement sur stm32l476xx.h via stm32l4xx.h (voir Drivers/CMSIS/ST/STM324xx/Include
 *
 *
 *  Notes sur les lib
 *
 *  PROGRAMMATION au niveau registres, .h utiles :
 *  		stm32l476xx.h (pour les registres et  Irqn )
 *  		core_cm4.h (pour les fct bas niveau du NVIC, enable, prio)
 *
 *  IMBRICATION des .h :
 *
 * stm32l4xx_hal.h
 * -> stm32l4xx_hal_conf.h
 * 		-> stm32l4xx_hal_xxperiphxx.h comme par ex stm32l4xx_hal_uart.h
 *			-> stm32l4xx_hal_def.h
 *				-> stm32l4xx.h
 *					-> stm32l476xx.h (les Irqn les typedef niveau registres
 *						-> core_cm4.h (les fct NVIC, reg du coeur, Systick, nvic...)
 */


#include "ComUART.h"


/******************************************************************************
*  PRIVATE Variables
*
******************************************************************************/

/**
  * @brief Variable d'état du module
  * Cmde, RecepFlag, TransmFlag accessibles
  **/
struct {
	char Cmde;      // le caractère filtré
	char CurrentByte; // caractère effectivement reçu non filtré
	char RecepFlag;  // indique qu'une commande est arrivée
	char TransmFlag; // indique qu'un print est en cours
	void (*ComUART_Callback)(void);
}ComUART_Status;




static UART_HandleTypeDef * ComUART_HandlerPtr;
static char Interrupt;



/******************************************************************************
*  PUBLIC FUNCTION
*  Fonctions d'initialisation
******************************************************************************/

/**
  * @brief  Initialise le module UARTCom en mode interruption avec
  * 		callback sur la réception d'une commande (caractère reçu filtré)
  * @param  UsedUSART = le handle de l'usart au sens de Hal
  * @param  Callback = nom de la fonction appelé sur réception d'une commande
  * @retval None
  **/
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


/**
  * @brief  Initialise le module UARTCom en mode polling sur la réception
  * @param  UsedUSART = le handle de l'usart au sens de Hal
  * @retval None
  **/
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


/******************************************************************************
*  PUBLIC FUNCTION
*  Fonctions d'écriture (print)
******************************************************************************/

/**
  * @brief  Renseigne sur l'état du module en terme de transmission
  * @param  none
  * @retval 1 si transmission en cours, 0 si Tx libre
  **/
char ComUART_IsTansmitting(void)
{
	return ComUART_Status.TransmFlag;
}


/**
  * @brief  Printe une chaîne de caractère sur l'USART (Tx)
  * @note : utilise fct HAL_UART_Transmit, timing testé à 9600Bds,
  * 		correspond au débit attendu
  * @param  char * str adresse de la châine à envoyer
  * @param  Lenght longueur de la chaîne à transmettre
  * @retval none
  **/
void ComUART_Print(char * Str, char Lenght)
// testé avec IO set/reset entrée et sortie de fct.
// la durée est parfaitement respectée.
{
	ComUART_Status.TransmFlag=1;
	HAL_UART_Transmit(ComUART_HandlerPtr,(uint8_t *) Str, Lenght, HAL_MAX_DELAY);
	ComUART_Status.TransmFlag=0;
}


/******************************************************************************
*  PUBLIC FUNCTION
*  Fonctions de lecture ComUART
******************************************************************************/

/**
  * @brief  Renseigne sur l'arrivée d'une nouvelle commande
  * @note  : Ne pas utiliser si init avec ComUART_Init_IT.
  * 		prévu en mode polling pour savoir si une cmde est arrivée
  * 	    Les cmdes sont définies dans le .h
  * @param  none
  * @retval 1 si réception d'une Cmde
  **/
char ComUART_IsNewCmde(void)
{
	return ComUART_Status.RecepFlag;
}

/**
  * @brief  Efface le drapeau de réception commande
  * @note  : Ne pas utiliser si init avec ComUART_Init_IT
  *          Doit être appelé immédiatement après lecture de la Cmde
  * @param  none
  * @retval none
  **/
void ComUART_ClrReceptFlag(void)
{
	ComUART_Status.RecepFlag=0;
}

/**
  * @brief  Lit la Cmde contenu dans la variable d'état
  * @note   à utiliser quelque soit le mode d'init.
  * @param  none
  * @retval char, la Cmde.
  **/
char ComUART_GetCmd(void)
{
	return ComUART_Status.Cmde;
}



/******************************************************************************
*  Private Interrupt FUNCTION
*  Fonction associée exclusivement à la réception d'un caractère quelconque
******************************************************************************/

/**
  * @brief  IT déclenchée quelque soit le mode d'init, sur réception d'un byte
  * @note   filtre les commandes
  * 		met à jour le Flag de réception en mode init non interruptif.
  * 		ne gère pas le flag en mode interruptif.
  * 		lance le callback si Cmde détectée et en mode init interruptif
  * @param  none
  * @retval char, la Cmde.
  **/
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