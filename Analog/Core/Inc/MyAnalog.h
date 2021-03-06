/*
 * MyAnalog.h
 *
 *  Created on: 1 févr. 2021
 *      Author: Thierry Rocacher
 *
 *  ======= CONTEXTE DU LOGICIEL ===================
 *  CIBLE : stm32L476RG (Nucléo board)
 *  IDE : Cube IDE vers 1.5.1
 *  CONTEXTE : pilotage, régulation de systèmes de puissance. Partie analogique
 *  DEPENDENCES : Le code s'appuie sur les registres du stm32L476 donc
 *  essentiellement sur stm32l476xx.h via stm32l4xx.h (voir Drivers/CMSIS/ST/STM324xx/Include)
 *  Le codage s'appuie sur les registres directement.
 *
 *
 *  ======= GENERALITE  ===================
 *
 *  Ce module est à utiliser conjointement à Cubemx. Il gère la DMA et la partie
 *  filtrage numérique.
 *
 *  Cubemx configure l'ADC1 avec autant de voies qu'on le souhaite, configuration
 *  des IO analogique, réglage du sampling time.
 *  réglage des horloges RCC et dédiées ADC1
 *  configuration d'un timer pour le déclenchement périodique en
 *  raffale ou non
 *  Pour des situation de débuggage, configurer le DAC1 (PA4), au plus simple
 *
 *  MyAnalog fait le reste, à savoir :
 *  - configurer la DMA avec son propre buffer (privé)
 *  - mise en oeuvre de filtre ou pas (filtre au choix ordre 1 /2, format float)
 *  - réglage de la fréquence de coupure
 *  - possibilité de lancer un callback en fin de DMA
 *  - Handler d'IT ADC privé au module, en option pour lancer des pulses sur
 *  LED2 (PA5) lors des acquisitions. A utiliser pour la mise au point
 *
 *
 *  ======= CHRONOGRAMMES TYPE (exemple 3 voies) ===================
 *
 *  Timer overflow => Trig ADC :
 *  ^          ^          ^          ^          ^          ^
 *  Acq en mode raffale (AllChan_In_One_Trig):
 *   |||(1,2,3) |||(1,2,3) |||(1,2,3) |||(1,2,3) |||(1,2,3) |||(1,2,3)
 *   Acq mode non raffale :
 *   |(1)       |(2)       |(3)       |(1)       |(2)       |(3)
 *   DMA IT processing (mode rafale) :
 *    ____X__________X__________X__________X__________X__________X
 *   DMA IT processing (mode non rafale)
 *   __X__________X__________X__________X__________X__________X___
 *
 *
 *  ======= LIMITATIONS ===================
 *
 * A chaque fin de DMA une IT est lancée si :
 *  - au moins un filtre est mis en place (via les #define du .h)
 *  - un callback est demandé
 *
 *  Le temps d'éxécution de cette interruption est importante. Elle ne doit
 *  représenter qu'une faible proportion de la période de déclenchement de l'ADC
 *  via le Timer (max 10 %?). L'idée est de laisser du temps pour un autre système
 *  par exemple un logiciel type FreeRTOS
 *  Il est donc important de mesurer les durées d'IT de DMA en activant
 *  l'option de débugage LED2 et de procéder à des mesures à l'oscilloscope
 */



/*
 *  ======= EXEMPLE DE CONFIGURATION CUBEmx ========================
 *  ======= ADC1 en scan mode sur deux voies, trigg TIM6 DMA off ===
 *
+++RCC +++
HSE à 8MHz avec BypassClocksource (car clocké sans quartz, directement depuis STlink)
Cette conf se fait sur la pin PH0 (RCC_osc_in)
PLLx20p pour monter le CPU à 80MHz (Hclk)


+++Timer 6 clocké à 80MHz+++
Autoreload à 4µs
Trigger output = Update Event

+++ADC1+++
ADC_in5 (PA0)
ADC_in6 (PA1)
NB : on peut configurer des entrées en mode différentiel
(utile par exemple pour un capteur courant type CASR6NP)
Clock prescaler = Synchronous clk mode divide by 4 (pour
être synchro avec le trig Timer). Div 4 pour ralentir l'ADC et
diminuer la puissance (il tourne à 20MHz)
Scan conv activé
End of conversion selec = End of Single conv (pour avoir des IT
à chaque acq)
External trigger conv source = TIM6 Trig out Event
Rank 1 = ADC_in5
Rank 2 = ADC_in6

 */



#ifndef INC_MYANALOG_H_
#define INC_MYANALOG_H_

#include "stm32l4xx_hal.h"



/******************************************************************************
*  #define généraux du module
*
******************************************************************************/

/**
  * @brief
  * Période d'échantillonnage en seconde.
  * ATTENTION !!! doit être la même que la valeur réelle:
  * Te = période Timer qui trigge l'ADC si AllChan_In_One_Trig activé
  * Te = N * période Timer qui trigge l'ADC si pas AllChan_In_One_Trig. N = nbre de voies
  */
#define Te 40e-6

/**
  * @brief
  * Nbre de canaux DMA (doit correspondre à la configuration Hal de l'ADC !!
  */
#define Chan_Nb 10


/**
  * @brief
  * si defini l'ADC est en mode scan rafale, si non défini,
  * l'ADC prend l'échantillon à chaque trig
  */
#define AllChan_In_One_Trig


/**
  * @brief
  * Priorité des interruptions de la partie analogique
  * 0 le plus prioritaire, 15 le moins
  */
#define ADC_Prio 0 // utilisé uniquement si DebugLED2 activé
#define DMA_Prio 1 // uniquement si un filtre au moins est défini




/******************************************************************************
*  #define relatifs au débuggage
*  si aucune définition, mode normal.
*  A n'utiliser que dans des phase de mise au point
*
******************************************************************************/

/**
  * @brief
  * si défini, PA5 émet un pulse lorsqu'un channel est converti
  * utilisé notamment pour mettre au point l'acquisition lors de la
  * mise au point du programme
  * (validation IT ADC et récupération handler dans MyAnalog.c)
  */
#define DebugLED2_ADC

/**
  * @brief
  * si défini, PA5 émet un pulse à l'état haut durant toute l'exécution du handler DMA,
  * Callback compris (si défini)
  */
#define DebugLED2_DMA


/**
  * @brief
  * Le #define DAC_Rank peut aller de 1 à 10.
  * Permet de sortir sn du filtre 1 à 10 sur DAC1 (PA4)
  */
//#define DAC1_Rank 1 // valeur 1 -> Filter_Rank_1





/******************************************************************************
*  Partie filrage
******************************************************************************/



/**
  * @brief
  * Activation des 10 filtres sur chacune des voies si défini
  */

#define Filter_Rank_1
//#define Filter_Rank_2
//#define Filter_Rank_3
//#define Filter_Rank_4
//#define Filter_Rank_5
//#define Filter_Rank_6
//#define Filter_Rank_7
//#define Filter_Rank_8
//#define Filter_Rank_9
//#define Filter_Rank_10


/**
  * @brief
  * Ordre des filtres (par défaut ordre 1 : défini = ordre 1, non défini = ordre 2)
  * Pour les ordres 2, m=0.707 (butterworth)
  */
#define FirstOrder_Rank1
#define FirstOrder_Rank2
#define FirstOrder_Rank3
#define FirstOrder_Rank4
#define FirstOrder_Rank5
#define FirstOrder_Rank6
#define FirstOrder_Rank7
#define FirstOrder_Rank8
#define FirstOrder_Rank9
#define FirstOrder_Rank10

/**
  * @brief
  * frequence de coupure des filtres en Hz (fn =fc pour ordre 2)
  */
#define Fc1 100.0
#define Fc2 200.0
#define Fc3 300.0
#define Fc4 400.0
#define Fc5 500.0
#define Fc6 600.0
#define Fc7 700.0
#define Fc8 800.0
#define Fc9 900.0
#define Fc10 1000.0




/******************************************************************************
*  Public Functions
******************************************************************************/

/**
  * @brief Termine la configuration ADC en DMA, lance le processus (le timer qui déclenche
  * l'ADC doit être fonctionnel (Hal). L'initialisation des filtres est faite.
  */
void MyAnalog_Init(void);


/**
  * @brief Termine la configuration ADC en DMA, lance le processus (le timer qui déclenche
  * l'ADC doit être fonctionnel (Hal). L'initialisation des filtres est faite.
  * -> en plus lance un callback en fin de DMA si besoin.
  * @param : nom de la fonction Callback
  *
  */
void MyAnalog_Init_WithCallback(void (*CallBack_EO_DMA_function) (void));


/**
  * @brief récupère la dernière donnée brute reçue par en fin de DMA
  * @param : indice du rang d'acquisition (de 1 à 10)
  */
short int MyAnalog_GetVakue(int Rank);



/**
  * @brief récupère la dernière donnée filtrée en fin de DMA
  * Disponible que si un au moins des filtres est sélectionné
  * @param : indice du rang d'acquisition (de 1 à 10)
  */

#if defined(Filter_Rank_1)||defined(Filter_Rank_2)||defined(Filter_Rank_3)||defined(Filter_Rank_4)\
 ||defined(Filter_Rank_5)||defined(Filter_Rank_6)||defined(Filter_Rank_7)\
 ||defined(Filter_Rank_8)||defined(Filter_Rank_9)||defined(Filter_Rank_10)
short int MyAnalog_GetFilteredVakue(int Rank);
#endif


#endif /* INC_MYANALOG_H_ */
