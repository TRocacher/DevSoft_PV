/*
 * MyAnalog.h
 *
 *  Created on: 15 Avril. 2021
 *      Author: Thierry Rocacher
 *
 *  ======= CONTEXTE DU LOGICIEL ===================
 *  CIBLE : stm32L476RG (Nucléo board)
 *  IDE : Cube IDE vers 1.5.1
 *  CONTEXTE : gestion soft système PV
 *  DEPENDENCES :
 *
 *
 *  ======= GENERALITE  ===================
 *

 *
 *
 */




#ifndef MENU_H_
#define MENU_H_

#include "StringFct.h"
#include "stm32l4xx_hal.h"


typedef struct
{
	float Val;
	float MinVal;
	float MaxVal;
	char DigitNb;
	char DecimalNb;
	char NewUserInputFlag;
} Menu_ParamValTypedef;




typedef struct Menu_Node Menu_Node;
struct Menu_Node {
	Menu_Node *  NextNode;
	Menu_Node *  PreviousNode;
	Menu_Node *  UpNode;
	Menu_Node *  DownNode;
	char * Title;
	char MenuPosition;
	char * ItemToPrint;
	void (*NodeExecFct)(void);
	Menu_ParamValTypedef * Param;
};

extern Menu_Node Item_Menu1, Item_Menu2, Item_Menu3, Item_Menu4;
extern Menu_Node Item_Menu1_1,Item_Menu1_2;
extern Menu_Node Item_Menu2_1;
extern Menu_Node Item_Menu3_1,Item_Menu3_2,Item_Menu3_3,Item_Menu3_4,Item_Menu3_5,Item_Menu3_6,Item_Menu3_7;
extern Menu_Node ItemInput_VoltParam;

void Menu_Init(UART_HandleTypeDef * UsedUSART);




#endif /* INC_MYANALOG_H_ */
