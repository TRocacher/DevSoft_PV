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


static short int MyAnalog_DMA_Buffer[32]; // buffer volontairement plus large
static short int MyAnalog_Sn_Filter[32];  // valeurs des sorties filtres






/******************************************************************************
*  PRIVATE FUNCTION
*  See stm32l47xx RM
*  ADC1 mappé sur Channel 1 de DMA1.
*  Cette fonction organise la DMA sur le channel 1. L'ADC est supposé configuré
*  pour utiliser la DMA sur le nombre de voies égal à Chan_Nb.
*  Une fois la fonction exécutée, la DMA est lancée.
******************************************************************************/

void MyAnalog_Filter_Init(void);

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


#ifndef AllChan_In_One_Trig
	  ADC1->CFGR&=~ADC_CFGR_DISCNUM_Msk; // Discnum = 000, cad 1 voie à chaque trig
	  ADC1->CFGR|=ADC_CFGR_DISCEN;       // validation mode discontinu (non raffale)
#endif

#if defined(Filter_0_On)||defined(Filter_1_On)||defined(Filter_2_On)||defined(Filter_3_On)\
 ||defined(Filter_4_On)||defined(Filter_5_On)||defined(Filter_6_On)\
 ||defined(Filter_7_On)||defined(Filter_8_On)||defined(Filter_9_On)

	  // Initialisation des filtres
      MyAnalog_Filter_Init();
      // Activation IT DMA
      DMA1_Channel1->CCR&=~DMA_CCR_EN;
      DMA1_Channel1->CCR|=DMA_CCR_TCIE;
      DMA1_Channel1->CCR|=DMA_CCR_EN;
	  NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	  NVIC_SetPriority(DMA1_Channel1_IRQn,DMA_Prio);

#endif

	  // Validation ADC
	  ADC1->CR|=ADC_CR_ADEN;
	  // Lancement ADC
	  ADC1->CR|=ADC_CR_ADSTART;
}








/******************************************************************************
*  PRIVATE FUNCTION
*  Les filtres numériques second odre
******************************************************************************/
#define pi 3.14159265358979323
#define m 0.707106781
#define Fract_28 (1<<28)


/*
 * Filter _0
 */
#ifdef Filter_0_On
static float a0_0,a1_0,a2_0,b1_0,b2_0;
static float en_0,en1_0,en2_0,sn_0,sn1_0,sn2_0;
#endif

/*
 * Filter _1
 */
#ifdef Filter_1_On
static int64_t a0_1,a1_1,a2_1,b1_1,b2_1;
static int64_t en_1,en1_1,en2_1,sn_1,sn1_1,sn2_1;
#endif

/*static float a0_1,a1_1,a2_1,b1_1,b2_1;
static float a0_2,a1_2,a2_2,b1_2,b2_2;
static float a0_3,a1_3,a2_3,b1_3,b2_3;
static float a0_4,a1_4,a2_4,b1_4,b2_4;
static float a0_5,a1_5,a2_5,b1_5,b2_5;
static float a0_6,a1_6,a2_6,b1_6,b2_6;
static float a0_7,a1_7,a2_7,b1_7,b2_7;
static float a0_8,a1_8,a2_8,b1_8,b2_8;
static float a0_9,a1_9,a2_9,b1_9,b2_9;
*/
void MyAnalog_Filter_Init(void)
{
float a0,b,wnTe,tampon;

#ifdef Filter_0_On
	wnTe=2*(pi)*(Fc1)*(Te);
	a0=1.0+(4.0*(m))/(wnTe) + 4.0/(wnTe*wnTe);
	a0_0=1.0/a0;
	a1_0=2.0/a0;
	a2_0=a0_0;

	b=2.0-8.0/(wnTe*wnTe);
	b1_0=b/a0;
	b=1.0-(4.0*(m))/(wnTe)+4.0/(wnTe*wnTe);
	b2_0=b/a0;

	 // mise à 0 des mémoires
	 MyAnalog_Sn_Filter[0]=0;
	 en_0=0.0;
	 en1_0=0.0;
	 en2_0=0.0;
	 sn_0=0.0;
	 sn1_0=0.0;
	 sn2_0=0.0;
#endif

#ifdef Filter_1_On // !!!!!!!!!!!!!!!!!!!!!! test en format fractionnaire
	/*
	 * entrée / sortie 16.16.
	 * coeff 4.28 ( +/-8 partie entière, réso 2^-28 = 3.7e-9
	 * Produit en 20.44.
	 * sn en 20.44 doit être déclaré en long
	 * sn remis en format 16.16 pour mémorisation
	 *
	 */
	wnTe=2*(pi)*(Fc1)*(Te);
	a0=1.0+(4.0*(m))/(wnTe) + 4.0/(wnTe*wnTe);
	a0_1=(int)((1.0/a0)*(float)(Fract_28)); // passage en fract 4.28
	a1_1=(int)((2.0/a0)*(float)(Fract_28));
	a2_1=a0_1;

	tampon=2.0-8.0/(wnTe*wnTe);
	b1_1=(int)((tampon/a0)*(float)(Fract_28));
	tampon=1.0-(4.0*(m))/(wnTe)+4.0/(wnTe*wnTe);
	b2_1=(int)((tampon/a0)*(float)(Fract_28));

	 // mise à 0 des mémoires
	 MyAnalog_Sn_Filter[1]=0;
	 en_1=0;
	 en1_1=0;
	 en2_1=0;
	 sn_1=0;
	 sn1_1=0;
	 sn2_1=0;
#endif


}








/******************************************************************************
*  PRIVATE FUNCTION (IT fct)
*  Handler IT ADC pour debug sur la LED2 (PA5)
******************************************************************************/


/**
  * @brief Pulse Led 2 on EOC ADC
  */
void ADC1_2_IRQHandler(void)
{
  GPIOA->BSRR=GPIO_PIN_5;  //-- set IO
  ADC1->ISR|=ADC_ISR_EOC; //-- release flag EOC
  GPIOA->BRR=GPIO_PIN_5; //-- reset IO
}

float sn;
int64_t sn_long;


void DMA1_Channel1_IRQHandler (void)
{

   GPIOA->BSRR=GPIO_PIN_5;  //-- set IO
   DMA1->IFCR|=DMA_IFCR_CTCIF1; //-- release flag DMA


#ifdef Filter_0_On
    // récupération en
    en_0=(float)MyAnalog_DMA_Buffer[0];
	sn=a0_0*en_0+a1_0*en1_0+a2_0*en2_0-b1_0*sn1_0-b2_0*sn2_0;
	sn2_0=sn1_0;
	sn1_0=sn;
	en2_0=en1_0;
	en1_0=en_0;
	// stockage
	MyAnalog_Sn_Filter[0]=(int)sn;
#endif

#ifdef Filter_1_On
    // récupération en
	/*
	a0_1=-1;
	en_1=0x7FFFFFFF;
	sn_long=a0_1*en_1;
	a0_1=2;
	en_1=0x7FFFFFFF;
	sn_long=(int64_t)a0_1*(int64_t)en_1;
*/


    en_1=MyAnalog_DMA_Buffer[1]<<16; // 16.16 32 bits
    sn_long=a0_1*en_1+a1_1*en1_1+a2_1*en2_1-b1_1*sn1_1-b2_1*sn2_1; // resultat 64 bits 20.44
	sn2_1=sn1_1;
	sn1_1=(int)(sn_long>>28); // remise en format 16.16 (déclage de 44-16 = >>28)
	en2_1=en1_1;
	en1_1=en_1;
	// stockage
	MyAnalog_Sn_Filter[1]=(short int)(sn_long>>44);// remise en format 16.0 (déclage de 44-0 = >>44)
#endif


	// sortie DAC
    DAC1->DHR12R1=MyAnalog_Sn_Filter[0];

   GPIOA->BRR=GPIO_PIN_5; //-- reset IO

}


















