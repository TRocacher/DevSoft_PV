/*
 * MyAnalog.h
 *
 *  Created on: 1 févr. 2021
 *      Author: trocache
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
 *
 *  MyAnalog fait le reste, à savoir :
 *  - configurer la DMA avec son propre buffer (privé)
 *  - mise en oeuvre de filtre ou pas (filtre à moyenne glissante, format fract.)
 *  - réglage du nombre de coeff de filtre
 *  - possibilité de lancer un callback en fin de DMA
 *  - Handler d'IT ADC privé au module, en option pour lancer des pulses sur
 *  LED2 (PA5) lors des acquisitions. Permet de débugger les acq ADC
 *
 *
 *  ======= CHRONOGRAMMES TYPE (exemple 3 voies) ===================
 *
 *  Timer overflow = Trig ADC :
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
//si defini l'ADC est en mode scan rafale, si non défini,
//l'ADC prend l'échantillon à chaque trig
// -> prend le pas sur CubeMx
#define AllChan_In_One_Trig

//si défini, PA5 reçoit un pulse lorsqu'un channel est converti
//(validation IT ADC et récupération handler dans MyAnalog.c)
#define DebugLED2

#define ADC_Prio 0 // utilisé uniquement si DebugLED2 activé

void MyAnalog_Init(int Chan_Nb);


#endif /* INC_MYANALOG_H_ */
