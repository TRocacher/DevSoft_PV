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

/******************************************************************************
*  PRIVATE Variables
*
******************************************************************************/

static short int MyAnalog_DMA_Buffer[32]; // buffer volontairement plus large

#if defined(Filter_Rank_1)||defined(Filter_Rank_2)||defined(Filter_Rank_3)||defined(Filter_Rank_4)\
 ||defined(Filter_Rank_5)||defined(Filter_Rank_6)||defined(Filter_Rank_7)\
 ||defined(Filter_Rank_8)||defined(Filter_Rank_9)||defined(Filter_Rank_10)
static short int MyAnalog_Sn_Filter[32];  // valeurs des sorties filtres
#endif





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
void MyAnalog_DMAInit(void)
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




static void (*Ptr_DMA_Callback)(void);

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
void MyAnalog_Init(void)
{
	  Ptr_DMA_Callback=NULL;
	  // Modifs conf HAL
	  ADC1->IER&=0; // plus aucune interruption autorisée
	  // Activation ADC en DMA + adaptation ADC au mode circular de la DMA
	  ADC1->CFGR|=ADC_CFGR_DMACFG; // ADC pour une DMA en circular mode
	  ADC1->CFGR|=ADC_CFGR_DMAEN;

	  // init DMA
	  MyAnalog_DMAInit();

	  // validation interruption si demandé
#ifdef DebugLED2_ADC
	  // validation IT
	  ADC1->IER|=ADC_IER_EOCIE;
	  NVIC_EnableIRQ(ADC1_2_IRQn);
	  NVIC_SetPriority(ADC1_2_IRQn,ADC_Prio);
#endif


#ifndef AllChan_In_One_Trig
	  ADC1->CFGR&=~ADC_CFGR_DISCNUM_Msk; // Discnum = 000, cad 1 voie à chaque trig
	  ADC1->CFGR|=ADC_CFGR_DISCEN;       // validation mode discontinu (non raffale)
#endif

#if defined(Filter_Rank_1)||defined(Filter_Rank_2)||defined(Filter_Rank_3)||defined(Filter_Rank_4)\
 ||defined(Filter_Rank_5)||defined(Filter_Rank_6)||defined(Filter_Rank_7)\
 ||defined(Filter_Rank_8)||defined(Filter_Rank_9)||defined(Filter_Rank_10)

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

/**
  * @brief Identique à MyAnalog_Init(void)
  * mais en plus initialise un pointeur de fonction pour lancer un callback
  * en fin de DMA
  */

void MyAnalog_Init_WithCallback(void (*CallBack_EO_DMA_function) (void))
{
	MyAnalog_Init();
	Ptr_DMA_Callback=CallBack_EO_DMA_function;
    // Activation IT DMA nécessaire au cas car cette action n'est faite dans MyAnalog_Init que
	// si au moins un filtre est demandé.
    DMA1_Channel1->CCR&=~DMA_CCR_EN;
    DMA1_Channel1->CCR|=DMA_CCR_TCIE;
    DMA1_Channel1->CCR|=DMA_CCR_EN;
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	NVIC_SetPriority(DMA1_Channel1_IRQn,DMA_Prio);
}



short int MyAnalog_GetVakue(int Rank)
{
return MyAnalog_DMA_Buffer[Rank-1];
}


#if defined(Filter_Rank_1)||defined(Filter_Rank_2)||defined(Filter_Rank_3)||defined(Filter_Rank_4)\
 ||defined(Filter_Rank_5)||defined(Filter_Rank_6)||defined(Filter_Rank_7)\
 ||defined(Filter_Rank_8)||defined(Filter_Rank_9)||defined(Filter_Rank_10)

short int MyAnalog_GetFilteredVakue(int Rank)
{
return MyAnalog_Sn_Filter[Rank-1];
}

#endif

/******************************************************************************
*  PRIVATE FUNCTION
*  Les filtres numériques second odre
******************************************************************************/
#define pi 3.14159265358979323
#define m 0.707106781


/*
 * Filter Rank 1
 */
#ifdef Filter_Rank_1
static float F1_a0,F1_a1,F1_b1;
static float F1_en,F1_en1,F1_sn1;
#ifndef FirstOrder_Rank1
static float F1_a2,F1_b2;
static float F1_en2,F1_sn2;
#endif
#endif


/*
 * Filter Rank 2
 */
#ifdef Filter_Rank_2
static float F2_a0,F2_a1,F2_b1;
static float F2_en,F2_en1,F2_sn1;
#ifndef FirstOrder_Rank2
static float F2_a2,F2_b2;
static float F2_en2,F2_sn2;
#endif
#endif

/*
 * Filter Rank 3
 */
#ifdef Filter_Rank_3
static float F3_a0,F3_a1,F3_b1;
static float F3_en,F3_en1,F3_sn1;
#ifndef FirstOrder_Rank3
static float F3_a2,F3_b2;
static float F3_en2,F3_sn2;
#endif
#endif

/*
 * Filter Rank 4
 */
#ifdef Filter_Rank_4
static float F4_a0,F4_a1,F4_b1;
static float F4_en,F4_en1,F4_sn1;
#ifndef FirstOrder_Rank4
static float F4_a2,F4_b2;
static float F4_en2,F4_sn2;
#endif
#endif

/*
 * Filter Rank 5
 */
#ifdef Filter_Rank_5
static float F5_a0,F5_a1,F5_b1;
static float F5_en,F5_en1,F5_sn1;
#ifndef FirstOrder_Rank5
static float F5_a2,F5_b2;
static float F5_en2,F5_sn2;
#endif
#endif

/*
 * Filter Rank 6
 */
#ifdef Filter_Rank_6
static float F6_a0,F6_a1,F6_b1;
static float F6_en,F6_en1,F6_sn1;
#ifndef FirstOrder_Rank6
static float F6_a2,F6_b2;
static float F6_en2,F6_sn2;
#endif
#endif

/*
 * Filter Rank 7
 */
#ifdef Filter_Rank_7
static float F7_a0,F7_a1,F7_b1;
static float F7_en,F7_en1,F7_sn1;
#ifndef FirstOrder_Rank7
static float F7_a2,F7_b2;
static float F7_en2,F7_sn2;
#endif
#endif

/*
 * Filter Rank 8
 */
#ifdef Filter_Rank_8
static float F8_a0,F8_a1,F8_b1;
static float F8_en,F8_en1,F8_sn1;
#ifndef FirstOrder_Rank8
static float F8_a2,F8_b2;
static float F8_en2,F8_sn2;
#endif
#endif

/*
 * Filter Rank 9
 */
#ifdef Filter_Rank_9
static float F9_a0,F9_a1,F9_b1;
static float F9_en,F9_en1,F9_sn1;
#ifndef FirstOrder_Rank9
static float F9_a2,F9_b2;
static float F9_en2,F9_sn2;
#endif
#endif

/*
 * Filter Rank 10
 */
#ifdef Filter_Rank_10
static float F10_a0,F10_a1,F10_b1;
static float F10_en,F10_en1,F10_sn1;
#ifndef FirstOrder_Rank10
static float F10_a2,F10_b2;
static float F10_en2,F10_sn2;
#endif
#endif


/*
 * Fonction d'initialisation des filtres
 */
void MyAnalog_Filter_Init(void)
{
#if defined(Filter_Rank_1)||defined(Filter_Rank_2)||defined(Filter_Rank_3)||defined(Filter_Rank_4)\
 ||defined(Filter_Rank_5)||defined(Filter_Rank_6)||defined(Filter_Rank_7)\
 ||defined(Filter_Rank_8)||defined(Filter_Rank_9)||defined(Filter_Rank_10)

float a,b,wnTe;

#endif

#ifdef Filter_Rank_1
   #ifdef FirstOrder_Rank1
	b=1.0/(2.0*pi*Fc1); // b est utilisé pour Tau
	a=1.0+2.0*b/Te; // a0
	F1_a0=1.0/a;
	F1_a1=1.0/a;


	b=1.0-2.0*b/Te;
	wnTe=1.0; // affectation pour éviter un warning si ordre 1
	F1_b1=(wnTe*b)/a;


   #else
	wnTe=2*(pi)*(Fc1)*(Te);
	a=1.0+(4.0*(m))/(wnTe) + 4.0/(wnTe*wnTe);
	F1_a0=1.0/a;
	F1_a1=2.0/a;
	F1_a2=F1_a0;

	b=2.0-8.0/(wnTe*wnTe);
	F1_b1=b/a;
	b=1.0-(4.0*(m))/(wnTe)+4.0/(wnTe*wnTe);
	F1_b2=b/a;

	 // mise à 0 des mémoires
	 MyAnalog_Sn_Filter[0]=0;
	 F1_en=0.0;
	 F1_en1=0.0;
	 F1_en2=0.0;
	 F1_sn1=0.0;
	 F1_sn2=0.0;
	#endif
#endif

#ifdef Filter_Rank_2
#ifdef FirstOrder_Rank2
	b=1.0/(2.0*pi*Fc2); // b est utilisé pour Tau
	a=1.0+2.0*b/Te; // a0
	F2_a0=1.0/a;
	F2_a1=1.0/a;


	b=1.0-2.0*b/Te;
	wnTe=1.0; // affectation pour éviter un warning si ordre 1
	F2_b1=(wnTe*b)/a;


#else
	wnTe=2*(pi)*(Fc2)*(Te);
	a=1.0+(4.0*(m))/(wnTe) + 4.0/(wnTe*wnTe);
	F2_a0=1.0/a;
	F2_a1=2.0/a;
	F2_a2=F2_a0;

	b=2.0-8.0/(wnTe*wnTe);
	F2_b1=b/a;
	b=1.0-(4.0*(m))/(wnTe)+4.0/(wnTe*wnTe);
	F2_b2=b/a;

	 // mise à 0 des mémoires
	 MyAnalog_Sn_Filter[1]=0;
	 F2_en=0.0;
	 F2_en1=0.0;
	 F2_en2=0.0;
	 F2_sn1=0.0;
	 F2_sn2=0.0;
	#endif
#endif

#ifdef Filter_Rank_3
#ifdef FirstOrder_Rank3
	b=1.0/(2.0*pi*Fc3); // b est utilisé pour Tau
	a=1.0+2.0*b/Te; // a0
	F3_a0=1.0/a;
	F3_a1=1.0/a;


	b=1.0-2.0*b/Te;
	wnTe=1.0; // affectation pour éviter un warning si ordre 1
	F3_b1=(wnTe*b)/a;


#else
	wnTe=2*(pi)*(Fc3)*(Te);
	a=1.0+(4.0*(m))/(wnTe) + 4.0/(wnTe*wnTe);
	F3_a0=1.0/a;
	F3_a1=2.0/a;
	F3_a2=F3_a0;

	b=2.0-8.0/(wnTe*wnTe);
	F3_b1=b/a;
	b=1.0-(4.0*(m))/(wnTe)+4.0/(wnTe*wnTe);
	F3_b2=b/a;

	 // mise à 0 des mémoires
	 MyAnalog_Sn_Filter[2]=0;
	 F3_en=0.0;
	 F3_en1=0.0;
	 F3_en2=0.0;
	 F3_sn1=0.0;
	 F3_sn2=0.0;
	#endif
#endif

#ifdef Filter_Rank_4
#ifdef FirstOrder_Rank4
	b=1.0/(2.0*pi*Fc4); // b est utilisé pour Tau
	a=1.0+2.0*b/Te; // a0
	F4_a0=1.0/a;
	F4_a1=1.0/a;


	b=1.0-2.0*b/Te;
	wnTe=1.0; // affectation pour éviter un warning si ordre 1
	F4_b1=(wnTe*b)/a;


#else
	wnTe=2*(pi)*(Fc4)*(Te);
	a=1.0+(4.0*(m))/(wnTe) + 4.0/(wnTe*wnTe);
	F4_a0=1.0/a;
	F4_a1=2.0/a;
	F4_a2=F4_a0;

	b=2.0-8.0/(wnTe*wnTe);
	F4_b1=b/a;
	b=1.0-(4.0*(m))/(wnTe)+4.0/(wnTe*wnTe);
	F4_b2=b/a;

	 // mise à 0 des mémoires
	 MyAnalog_Sn_Filter[3]=0;
	 F4_en=0.0;
	 F4_en1=0.0;
	 F4_en2=0.0;
	 F4_sn1=0.0;
	 F4_sn2=0.0;
	#endif
#endif

#ifdef Filter_Rank_5
#ifdef FirstOrder_Rank5
	b=1.0/(2.0*pi*Fc5); // b est utilisé pour Tau
	a=1.0+2.0*b/Te; // a0
	F5_a0=1.0/a;
	F5_a1=1.0/a;


	b=1.0-2.0*b/Te;
	wnTe=1.0; // affectation pour éviter un warning si ordre 1
	F5_b1=(wnTe*b)/a;


#else
	wnTe=2*(pi)*(Fc5)*(Te);
	a=1.0+(4.0*(m))/(wnTe) + 4.0/(wnTe*wnTe);
	F5_a0=1.0/a;
	F5_a1=2.0/a;
	F5_a2=F5_a0;

	b=2.0-8.0/(wnTe*wnTe);
	F5_b1=b/a;
	b=1.0-(4.0*(m))/(wnTe)+4.0/(wnTe*wnTe);
	F5_b2=b/a;

	 // mise à 0 des mémoires
	 MyAnalog_Sn_Filter[4]=0;
	 F5_en=0.0;
	 F5_en1=0.0;
	 F5_en2=0.0;
	 F5_sn1=0.0;
	 F5_sn2=0.0;
	#endif
#endif

#ifdef Filter_Rank_6
#ifdef FirstOrder_Rank6
	b=1.0/(2.0*pi*Fc6); // b est utilisé pour Tau
	a=1.0+2.0*b/Te; // a0
	F6_a0=1.0/a;
	F6_a1=1.0/a;


	b=1.0-2.0*b/Te;
	wnTe=1.0; // affectation pour éviter un warning si ordre 1
	F6_b1=(wnTe*b)/a;


#else
	wnTe=2*(pi)*(Fc6)*(Te);
	a=1.0+(4.0*(m))/(wnTe) + 4.0/(wnTe*wnTe);
	F6_a0=1.0/a;
	F6_a1=2.0/a;
	F6_a2=F6_a0;

	b=2.0-8.0/(wnTe*wnTe);
	F6_b1=b/a;
	b=1.0-(4.0*(m))/(wnTe)+4.0/(wnTe*wnTe);
	F6_b2=b/a;

	 // mise à 0 des mémoires
	 MyAnalog_Sn_Filter[5]=0;
	 F6_en=0.0;
	 F6_en1=0.0;
	 F6_en2=0.0;
	 F6_sn1=0.0;
	 F6_sn2=0.0;
	#endif
#endif

#ifdef Filter_Rank_7
#ifdef FirstOrder_Rank7
	b=1.0/(2.0*pi*Fc7); // b est utilisé pour Tau
	a=1.0+2.0*b/Te; // a0
	F7_a0=1.0/a;
	F7_a1=1.0/a;


	b=1.0-2.0*b/Te;
	wnTe=1.0; // affectation pour éviter un warning si ordre 1
	F7_b1=(wnTe*b)/a;


#else
	wnTe=2*(pi)*(Fc7)*(Te);
	a=1.0+(4.0*(m))/(wnTe) + 4.0/(wnTe*wnTe);
	F7_a0=1.0/a;
	F7_a1=2.0/a;
	F7_a2=F7_a0;

	b=2.0-8.0/(wnTe*wnTe);
	F7_b1=b/a;
	b=1.0-(4.0*(m))/(wnTe)+4.0/(wnTe*wnTe);
	F7_b2=b/a;

	 // mise à 0 des mémoires
	 MyAnalog_Sn_Filter[6]=0;
	 F7_en=0.0;
	 F7_en1=0.0;
	 F7_en2=0.0;
	 F7_sn1=0.0;
	 F7_sn2=0.0;
	#endif
#endif

#ifdef Filter_Rank_8
#ifdef FirstOrder_Rank8
	b=1.0/(2.0*pi*Fc8); // b est utilisé pour Tau
	a=1.0+2.0*b/Te; // a0
	F8_a0=1.0/a;
	F8_a1=1.0/a;


	b=1.0-2.0*b/Te;
	wnTe=1.0; // affectation pour éviter un warning si ordre 1
	F8_b1=(wnTe*b)/a;


#else
	wnTe=2*(pi)*(Fc8)*(Te);
	a=1.0+(4.0*(m))/(wnTe) + 4.0/(wnTe*wnTe);
	F8_a0=1.0/a;
	F8_a1=2.0/a;
	F8_a2=F8_a0;

	b=2.0-8.0/(wnTe*wnTe);
	F8_b1=b/a;
	b=1.0-(4.0*(m))/(wnTe)+4.0/(wnTe*wnTe);
	F8_b2=b/a;

	 // mise à 0 des mémoires
	 MyAnalog_Sn_Filter[7]=0;
	 F8_en=0.0;
	 F8_en1=0.0;
	 F8_en2=0.0;
	 F8_sn1=0.0;
	 F8_sn2=0.0;
	#endif
#endif

#ifdef Filter_Rank_9
#ifdef FirstOrder_Rank9
	b=1.0/(2.0*pi*Fc9); // b est utilisé pour Tau
	a=1.0+2.0*b/Te; // a0
	F9_a0=1.0/a;
	F9_a1=1.0/a;


	b=1.0-2.0*b/Te;
	wnTe=1.0; // affectation pour éviter un warning si ordre 1
	F9_b1=(wnTe*b)/a;


#else
	wnTe=2*(pi)*(Fc9)*(Te);
	a=1.0+(4.0*(m))/(wnTe) + 4.0/(wnTe*wnTe);
	F9_a0=1.0/a;
	F9_a1=2.0/a;
	F9_a2=F9_a0;

	b=2.0-8.0/(wnTe*wnTe);
	F9_b1=b/a;
	b=1.0-(4.0*(m))/(wnTe)+4.0/(wnTe*wnTe);
	F9_b2=b/a;

	 // mise à 0 des mémoires
	 MyAnalog_Sn_Filter[8]=0;
	 F9_en=0.0;
	 F9_en1=0.0;
	 F9_en2=0.0;
	 F9_sn1=0.0;
	 F9_sn2=0.0;
	#endif
#endif

#ifdef Filter_Rank_10
#ifdef FirstOrder_Rank10
	b=1.0/(2.0*pi*Fc10); // b est utilisé pour Tau
	a=1.0+2.0*b/Te; // a0
	F10_a0=1.0/a;
	F10_a1=1.0/a;


	b=1.0-2.0*b/Te;
	wnTe=1.0; // affectation pour éviter un warning si ordre 1
	F10_b1=(wnTe*b)/a;


#else
	wnTe=2*(pi)*(Fc10)*(Te);
	a=1.0+(4.0*(m))/(wnTe) + 4.0/(wnTe*wnTe);
	F10_a0=1.0/a;
	F10_a1=2.0/a;
	F10_a2=F10_a0;

	b=2.0-8.0/(wnTe*wnTe);
	F10_b1=b/a;
	b=1.0-(4.0*(m))/(wnTe)+4.0/(wnTe*wnTe);
	F10_b2=b/a;

	 // mise à 0 des mémoires
	 MyAnalog_Sn_Filter[9]=0;
	 F10_en=0.0;
	 F10_en1=0.0;
	 F10_en2=0.0;
	 F10_sn1=0.0;
	 F10_sn2=0.0;
	#endif
#endif
}








/******************************************************************************
*  IT FUNCTIONS
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
/**
  * @brief Traitement des filtres, appel callback
  */

void DMA1_Channel1_IRQHandler (void)
{
#ifdef DebugLED2_DMA
   GPIOA->BSRR=GPIO_PIN_5;  //-- set IO
#endif

   DMA1->IFCR|=DMA_IFCR_CTCIF1; //-- release flag DMA


#ifdef Filter_Rank_1
    // récupération en
   	F1_en=(float)MyAnalog_DMA_Buffer[0];
	#ifdef FirstOrder_Rank1
   	sn=F1_a0*F1_en+F1_a1*F1_en1-F1_b1*F1_sn1;
	F1_sn1=sn;
	F1_en1=F1_en;
	#else
	sn=F1_a0*F1_en+F1_a1*F1_en1+F1_a2*F1_en2-F1_b1*F1_sn1-F1_b2*F1_sn2;
	F1_sn2=F1_sn1;
	F1_sn1=sn;
	F1_en2=F1_en1;
	F1_en1=F1_en;
	#endif
	// stockage
	MyAnalog_Sn_Filter[0]=(int)sn;
#endif

#ifdef Filter_Rank_2
    // récupération en
   	F2_en=(float)MyAnalog_DMA_Buffer[1];
	#ifdef FirstOrder_Rank2
   	sn=F2_a0*F2_en+F2_a1*F2_en1-F2_b1*F2_sn1;
	F2_sn1=sn;
	F2_en1=F2_en;
	#else
	sn=F2_a0*F2_en+F2_a1*F2_en1+F2_a2*F2_en2-F2_b1*F2_sn1-F2_b2*F2_sn2;
	F2_sn2=F2_sn1;
	F2_sn1=sn;
	F2_en2=F2_en1;
	F2_en1=F2_en;
	#endif
	// stockage
	MyAnalog_Sn_Filter[1]=(int)sn;
#endif

#ifdef Filter_Rank_3
    // récupération en
   	F3_en=(float)MyAnalog_DMA_Buffer[2];
	#ifdef FirstOrder_Rank3
   	sn=F3_a0*F3_en+F3_a1*F3_en1-F3_b1*F3_sn1;
	F3_sn1=sn;
	F3_en1=F3_en;
	#else
	sn=F3_a0*F3_en+F3_a1*F3_en1+F3_a2*F3_en2-F3_b1*F3_sn1-F3_b2*F3_sn2;
	F3_sn2=F3_sn1;
	F3_sn1=sn;
	F3_en2=F3_en1;
	F3_en1=F3_en;
	#endif
	// stockage
	MyAnalog_Sn_Filter[2]=(int)sn;
#endif

#ifdef Filter_Rank_4
    // récupération en
   	F4_en=(float)MyAnalog_DMA_Buffer[3];
	#ifdef FirstOrder_Rank4
   	sn=F4_a0*F4_en+F4_a1*F4_en1-F4_b1*F4_sn1;
	F4_sn1=sn;
	F4_en1=F4_en;
	#else
	sn=F4_a0*F4_en+F4_a1*F4_en1+F4_a2*F4_en2-F4_b1*F4_sn1-F4_b2*F4_sn2;
	F4_sn2=F4_sn1;
	F4_sn1=sn;
	F4_en2=F4_en1;
	F4_en1=F4_en;
	#endif
	// stockage
	MyAnalog_Sn_Filter[3]=(int)sn;
#endif

#ifdef Filter_Rank_5
    // récupération en
   	F5_en=(float)MyAnalog_DMA_Buffer[4];
	#ifdef FirstOrder_Rank5
   	sn=F5_a0*F5_en+F5_a1*F5_en1-F5_b1*F5_sn1;
	F5_sn1=sn;
	F5_en1=F5_en;
	#else
	sn=F5_a0*F5_en+F5_a1*F5_en1+F5_a2*F5_en2-F5_b1*F5_sn1-F5_b2*F5_sn2;
	F5_sn2=F5_sn1;
	F5_sn1=sn;
	F5_en2=F5_en1;
	F5_en1=F5_en;
	#endif
	// stockage
	MyAnalog_Sn_Filter[4]=(int)sn;
#endif

#ifdef Filter_Rank_6
    // récupération en
   	F6_en=(float)MyAnalog_DMA_Buffer[5];
	#ifdef FirstOrder_Rank6
   	sn=F6_a0*F6_en+F6_a1*F6_en1-F6_b1*F6_sn1;
	F6_sn1=sn;
	F6_en1=F6_en;
	#else
	sn=F6_a0*F6_en+F6_a1*F6_en1+F6_a2*F6_en2-F6_b1*F6_sn1-F6_b2*F6_sn2;
	F6_sn2=F6_sn1;
	F6_sn1=sn;
	F6_en2=F6_en1;
	F6_en1=F6_en;
	#endif
	// stockage
	MyAnalog_Sn_Filter[5]=(int)sn;
#endif

#ifdef Filter_Rank_7
    // récupération en
   	F7_en=(float)MyAnalog_DMA_Buffer[6];
	#ifdef FirstOrder_Rank7
   	sn=F7_a0*F7_en+F7_a1*F7_en1-F7_b1*F7_sn1;
	F7_sn1=sn;
	F7_en1=F7_en;
	#else
	sn=F7_a0*F7_en+F7_a1*F7_en1+F7_a2*F7_en2-F7_b1*F7_sn1-F7_b2*F7_sn2;
	F7_sn2=F7_sn1;
	F7_sn1=sn;
	F7_en2=F7_en1;
	F7_en1=F7_en;
	#endif
	// stockage
	MyAnalog_Sn_Filter[6]=(int)sn;
#endif

#ifdef Filter_Rank_8
    // récupération en
   	F8_en=(float)MyAnalog_DMA_Buffer[7];
	#ifdef FirstOrder_Rank8
   	sn=F8_a0*F8_en+F8_a1*F8_en1-F8_b1*F8_sn1;
	F8_sn1=sn;
	F8_en1=F8_en;
	#else
	sn=F8_a0*F8_en+F8_a1*F8_en1+F8_a2*F8_en2-F8_b1*F8_sn1-F8_b2*F8_sn2;
	F8_sn2=F8_sn1;
	F8_sn1=sn;
	F8_en2=F8_en1;
	F8_en1=F8_en;
	#endif
	// stockage
	MyAnalog_Sn_Filter[7]=(int)sn;
#endif

#ifdef Filter_Rank_9
    // récupération en
   	F9_en=(float)MyAnalog_DMA_Buffer[8];
	#ifdef FirstOrder_Rank9
   	sn=F9_a0*F9_en+F9_a1*F9_en1-F9_b1*F9_sn1;
	F9_sn1=sn;
	F9_en1=F9_en;
	#else
	sn=F9_a0*F9_en+F9_a1*F9_en1+F9_a2*F9_en2-F9_b1*F9_sn1-F9_b2*F9_sn2;
	F9_sn2=F9_sn1;
	F9_sn1=sn;
	F9_en2=F9_en1;
	F9_en1=F9_en;
	#endif
	// stockage
	MyAnalog_Sn_Filter[8]=(int)sn;
#endif

#ifdef Filter_Rank_10
    // récupération en
   	F10_en=(float)MyAnalog_DMA_Buffer[9];
	#ifdef FirstOrder_Rank10
   	sn=F10_a0*F10_en+F10_a1*F10_en1-F10_b1*F10_sn1;
	F10_sn1=sn;
	F10_en1=F10_en;
	#else
	sn=F10_a0*F10_en+F10_a1*F10_en1+F10_a2*F10_en2-F10_b1*F10_sn1-F10_b2*F10_sn2;
	F10_sn2=F10_sn1;
	F10_sn1=sn;
	F10_en2=F10_en1;
	F10_en1=F10_en;
	#endif
	// stockage
	MyAnalog_Sn_Filter[9]=(int)sn;
#endif


// sortie DAC
#if defined(Filter_Rank_1)||defined(Filter_Rank_2)||defined(Filter_Rank_3)||defined(Filter_Rank_4)\
 ||defined(Filter_Rank_5)||defined(Filter_Rank_6)||defined(Filter_Rank_7)\
 ||defined(Filter_Rank_8)||defined(Filter_Rank_9)||defined(Filter_Rank_10)

#if (DAC1_Rank==1)
    DAC1->DHR12R1=MyAnalog_Sn_Filter[0];
#elif (DAC1_Rank==2)
    DAC1->DHR12R1=MyAnalog_Sn_Filter[1];
#elif (DAC1_Rank==3)
    DAC1->DHR12R1=MyAnalog_Sn_Filter[2];
#elif (DAC1_Rank==4)
    DAC1->DHR12R1=MyAnalog_Sn_Filter[3];
#elif (DAC1_Rank==5)
    DAC1->DHR12R1=MyAnalog_Sn_Filter[4];
#elif (DAC1_Rank==6)
    DAC1->DHR12R1=MyAnalog_Sn_Filter[5];
#elif (DAC1_Rank==7)
    DAC1->DHR12R1=MyAnalog_Sn_Filter[6];
#elif (DAC1_Rank==8)
    DAC1->DHR12R1=MyAnalog_Sn_Filter[7];
#elif (DAC1_Rank==9)
    DAC1->DHR12R1=MyAnalog_Sn_Filter[8];
#elif (DAC1_Rank==10)
    DAC1->DHR12R1=MyAnalog_Sn_Filter[9];
#endif
#endif
   if (Ptr_DMA_Callback!=NULL)
		   {
	   	   Ptr_DMA_Callback();
		   }

#ifdef DebugLED2_DMA
   GPIOA->BRR=GPIO_PIN_5; //-- reset IO
#endif

}


















