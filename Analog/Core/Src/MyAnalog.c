/*
 * MyAnalog.c
 *
 *  Created on: 1 févr. 2021
 *      Author: trocache
 *
 *  CIBLE : stm32L476RG (Nucléo board)
 *  IDE : Cube IDE vers 1.5.1
 *  CONTEXTE : pilotage, régulation de systèmes de puissance. Partie analogique
 *  DEPENDENCES : Le code s'appuie sur les registres du stm32L476 donc
 *  essentiellement sur stm32l476xx.h via stm32l4xx.h (voir Drivers/CMSIS/ST/STM324xx/Include
 *
 *
 */


#include "MyAnalog.h"


short int MyAnalog_DMA_Buffer[32]; // buffer volontairement plus large







/******************************************************************************
*  PRIVATE FUNCTION
*  See stm32l47xx RM
*  ADC1 mappé sur Channel 1 de DMA1.
*  Cette fonction organise la DMA sur le channel 1. L'ADC est supposé configuré
*  pour utiliser la DMA sur le nombre de voies égal à Chan_Nb.
*  Une fois la fonction exécutée, la DMA est lancée.
******************************************************************************/


/**
  * @brief Congigure DMA only
  */
void MyAnalog_DMAInit(int Chan_Nb)
{

	// validation Ck DMA
	RCC->AHB1ENR|=RCC_AHB1ENR_DMA1EN;
    // channel selection
	DMA1_CSELR->CSELR&=~0xF; // ADC1 sélectionné pour la DMA
	DMA1_Channel1->CPAR=ADC1_BASE+0x40; // adresse du DR
	DMA1_Channel1->CMAR=(unsigned long)MyAnalog_DMA_Buffer;   // buffer d'arrivée
	DMA1_Channel1->CNDTR=Chan_Nb;
	// config channel : Higher Prio, Mem size 16 bits, periph Size 16bits, Inc Mem, Circular mode,
	DMA1_Channel1->CCR = DMA_CCR_PL| DMA_CCR_MSIZE_0|DMA_CCR_PSIZE_0|DMA_CCR_MINC|DMA_CCR_CIRC;
	DMA1_Channel1->CCR&=~DMA_CCR_PINC; // No inc on perpih
	DMA1_Channel1->CCR&=~DMA_CCR_DIR;  // Dir Periph to Mem
	// validation de la DMA
	DMA1_Channel1->CCR|=DMA_CCR_EN;
}





/******************************************************************************
*  PUBLIC FUNCTION
*  See stm32l47xx RM
*  Valide la DMA côté ADC, lance l'init DMA, valide les IT ADC si demandé (see .h)
*  lance l'ADC.
******************************************************************************/


/**
  * @brief Termine la configuration ADC en DMA, lance le processus (le timer qui déclenche
  * l'ADC doit tourner !
  */
void MyAnalog_Init(int Chan_Nb)
{

	  // Modifs conf HAL
	  ADC1->IER&=0; // plus aucune interruption autorisée
	  // Activation ADC en DMA + adaptation ADC au mode circular de la DMA
	  ADC1->CFGR|=ADC_CFGR_DMACFG; // ADC pour une DMA en circular mode
	  ADC1->CFGR|=ADC_CFGR_DMAEN;

	  // init DMA
	  MyAnalog_DMAInit(Chan_Nb);

	  // validation interruption si demandé
#ifdef DebugLED2
	  // validation IT
	  ADC1->IER|=ADC_IER_EOCIE;
	  NVIC_EnableIRQ(ADC1_2_IRQn);
	  NVIC_SetPriority(ADC1_2_IRQn,ADC_Prio);
#endif

	  // Validation ADC
	  ADC1->CR|=ADC_CR_ADEN;
	  // Lancement ADC
	  ADC1->CR|=ADC_CR_ADSTART;

}





/******************************************************************************
*  PRIVATE FUNCTION (IT fct)
*  Handler IT ADC pour debug sur la LED2 (PA5)
******************************************************************************/


/**
  * @brief Congigure DMA only
  */
void ADC1_2_IRQHandler(void)
{
  GPIOA->BSRR=GPIO_PIN_5;  //-- set IO
  ADC1->ISR|=ADC_ISR_EOC; //-- release flag EOC
  GPIOA->BRR=GPIO_PIN_5; //-- reset IO
}
