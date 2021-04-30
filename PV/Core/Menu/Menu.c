/*
 * MyAnalog.c
 *
 *  Created on: 15 Avril. 2021
 *      Author: trocache
 *
 *  CIBLE : stm32L476RG (Nucléo board)
 *  IDE : Cube IDE vers 1.5.1
 *  CONTEXTE : gestion soft système PV
 *  DEPENDENCES :
 *
 *
 */


#include "Menu.h"
#include "ComUART.h"
/******************************************************************************
*  PRIVATE Variables
*
******************************************************************************/

/*------------------------------------------------
*  MENU NODES, ITEMS
*  -> user : ajoutez autant de Menu_Node que d'items
*  dans les menus
*  -> user : remplir les menu nodes en fin de fichier
-------------------------------------------------*/
enum {First, Middle, Last};
void Menu_PassivePrint(void);
void Menu_InputStateMachine(void);

// Param nul
Menu_ParamValTypedef ParamNull;

/*==========================================================================================
 *
 * 		USER ZONE BEGIN
 *
 ==========================================================================================*/
/*............................................
 *
 * 		Définitions des Menu_Nodes utiles
 * 		à déclarer dans le menu.h
 *
.............................................*/

Menu_Node MenuNodePassive_1, MenuNodePassive_2, MenuNodePassive_3, Item_Menu4;
Menu_Node MenuNodePassive_1_1,MenuNodePassive_1_2;
Menu_Node MenuNodePassive_2_1;
Menu_Node MenuNodePassive_3_1,MenuNodePassive_3_2,MenuNodePassive_3_3,MenuNodePassive_3_4,MenuNodePassive_3_5,MenuNodePassive_3_6,MenuNodePassive_3_7;
Menu_Node MenuNodeInput_VoltParam;

Menu_Node MenuNodeInput_10;
Menu_Node MenuNodeInput_20,MenuNodeInput_21;


/*............................................
 *
 * 		Définitions des structures param
 * 		privée au module
 *
.............................................*/

// Voltage
Menu_ParamValTypedef VoltageParam;
Menu_ParamValTypedef Param_10;
Menu_ParamValTypedef Param_20,Param_21;



/*............................................
 *
 * 		Chargement des maillons MenuNodes
 *      = CONSTRUCTION DES DIVERS MENUS
 *
 *		chargement des param
 *		pour les menus saisie
.............................................*/

// test$$$$$$$$$$$$$$$$$$$$$$$$$$
char Tab[8];
float res;
// end test $$$$$$$$$$$$$$$$$$$
void MenuInput_PrintPlantageConv(void);

void Menu_Node_Init(void)
{


	/*............................................
	 *
	 * 		Chargement des maillons MenuNodes
	 *      = CONSTRUCTION DES DIVERS MENUS
	.............................................*/



	/*      **********************
	 * 		*Menu principal      *
	 * 		* Nenu 1             *
	 * 		* Nenu 2             *
	 * 		* Nenu 3             *
	 * 		* Nenu 4             *
	 * 		**********************
	 */
	MenuNodePassive_1.DownNode=&MenuNodePassive_2;
	MenuNodePassive_1.NextNode=&MenuNodePassive_1_1;
	MenuNodePassive_1.UpNode=&MenuNodePassive_1;  // lui même
	MenuNodePassive_1.PreviousNode=&MenuNodePassive_1;  // lui même
	MenuNodePassive_1.MenuPosition=First;
	MenuNodePassive_1.Title="Menu principal      ";
	MenuNodePassive_1.ItemToPrint =" Nenu 1             ";
	MenuNodePassive_1.NodeExecFct=Menu_PassivePrint;
	MenuNodePassive_1.Param=&ParamNull; //------------------------------------------------------------------

	MenuNodePassive_2.DownNode=&MenuNodePassive_3;
	MenuNodePassive_2.NextNode=&MenuNodePassive_2_1;
	MenuNodePassive_2.UpNode=&MenuNodePassive_1;
	MenuNodePassive_2.PreviousNode=&MenuNodePassive_2;  // lui même
	MenuNodePassive_2.MenuPosition=Middle;
	MenuNodePassive_2.Title="Menu principal      ";
	MenuNodePassive_2.ItemToPrint =" Nenu 2             ";
	MenuNodePassive_2.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_3.DownNode=&Item_Menu4;
	MenuNodePassive_3.NextNode=&MenuNodePassive_3_1;
	MenuNodePassive_3.UpNode=&MenuNodePassive_2;
	MenuNodePassive_3.PreviousNode=&MenuNodePassive_3;  // lui même
	MenuNodePassive_3.MenuPosition=Middle;
	MenuNodePassive_3.Title="Menu principal      ";
	MenuNodePassive_3.ItemToPrint =" Nenu 3             ";
	MenuNodePassive_3.NodeExecFct=Menu_PassivePrint;

	Item_Menu4.DownNode=&Item_Menu4; // lui même
	Item_Menu4.NextNode=&Item_Menu4;
	Item_Menu4.UpNode=&MenuNodePassive_3;
	Item_Menu4.PreviousNode=&Item_Menu4;  // lui même
	Item_Menu4.MenuPosition=Last;
	Item_Menu4.Title="Menu principal      ";
	Item_Menu4.ItemToPrint =" Nenu 4             ";
	Item_Menu4.NodeExecFct=Menu_PassivePrint;

	/*      **********************
	 * 		*Menu 1...           *
	 * 		* Nenu 1_1           *
	 * 		* Nenu 1_2           *
	 * 		**********************
	 */

	MenuNodePassive_1_1.DownNode=&MenuNodePassive_1_2;
	MenuNodePassive_1_1.NextNode=&MenuNodePassive_1_1;
	MenuNodePassive_1_1.UpNode=&MenuNodePassive_1_1;
	MenuNodePassive_1_1.PreviousNode=&MenuNodePassive_1;
	MenuNodePassive_1_1.MenuPosition=First;
	MenuNodePassive_1_1.Title="Menu 1...           ";
	MenuNodePassive_1_1.ItemToPrint =" Nenu 1_1           ";
	MenuNodePassive_1_1.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_1_2.DownNode=&MenuNodePassive_1_2;
	MenuNodePassive_1_2.NextNode=&MenuNodePassive_1_2;
	MenuNodePassive_1_2.UpNode=&MenuNodePassive_1_1;
	MenuNodePassive_1_2.PreviousNode=&MenuNodePassive_1;
	MenuNodePassive_1_2.MenuPosition=Last;
	MenuNodePassive_1_2.Title="Menu 1...           ";
	MenuNodePassive_1_2.ItemToPrint =" Nenu 1_2           ";
	MenuNodePassive_1_2.NodeExecFct=Menu_PassivePrint;



	/*      **********************
	 * 		*Menu 2...           *
	 * 		* Nenu 2_1           *
	 * 		**********************
	 */
	MenuNodePassive_2_1.DownNode=&MenuNodePassive_2_1;
	MenuNodePassive_2_1.NextNode=&MenuNodePassive_2_1;
	MenuNodePassive_2_1.UpNode=&MenuNodePassive_2_1;
	MenuNodePassive_2_1.PreviousNode=&MenuNodePassive_2;
	MenuNodePassive_2_1.MenuPosition=First;
	MenuNodePassive_2_1.Title="Menu 2...           ";
	MenuNodePassive_2_1.ItemToPrint =" Nenu 2_1           ";
	MenuNodePassive_2_1.NodeExecFct=Menu_PassivePrint;



	/*      **********************
	 * 		*Menu 3...           *
	 * 		* Voltage Param ...  *
	 * 		* Nenu 3_2           *
	 * 		* Nenu 3_3           *
	 * 		* Nenu 3_4           *
	 * 		* Nenu 3_5           *
	 * 		* Nenu 3_6           *
	 * 		* Nenu 3_7           *
	 * 		**********************
	 */
	MenuNodePassive_3_1.DownNode=&MenuNodePassive_3_2;
	MenuNodePassive_3_1.NextNode=&MenuNodeInput_VoltParam;
	MenuNodePassive_3_1.UpNode=&MenuNodePassive_3_1;
	MenuNodePassive_3_1.PreviousNode=&MenuNodePassive_3;
	MenuNodePassive_3_1.MenuPosition=First;
	MenuNodePassive_3_1.Title="Menu 3...           ";
	MenuNodePassive_3_1.ItemToPrint =" Nenu 3_1           ";
	MenuNodePassive_3_1.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_3_2.DownNode=&MenuNodePassive_3_3;
	MenuNodePassive_3_2.NextNode=&MenuNodeInput_10;
	MenuNodePassive_3_2.UpNode=&MenuNodePassive_3_1;
	MenuNodePassive_3_2.PreviousNode=&MenuNodePassive_3;
	MenuNodePassive_3_2.MenuPosition=Middle;
	MenuNodePassive_3_2.Title="Menu 3...           ";
	MenuNodePassive_3_2.ItemToPrint =" Nenu 3_2           ";
	MenuNodePassive_3_2.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_3_3.DownNode=&MenuNodePassive_3_4;
	MenuNodePassive_3_3.NextNode=&MenuNodeInput_20;
	MenuNodePassive_3_3.UpNode=&MenuNodePassive_3_2;
	MenuNodePassive_3_3.PreviousNode=&MenuNodePassive_3;
	MenuNodePassive_3_3.MenuPosition=Middle;
	MenuNodePassive_3_3.Title="Menu 3...           ";
	MenuNodePassive_3_3.ItemToPrint =" Nenu 3_3           ";
	MenuNodePassive_3_3.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_3_4.DownNode=&MenuNodePassive_3_5;
	MenuNodePassive_3_4.NextNode=&MenuNodeInput_21;
	MenuNodePassive_3_4.UpNode=&MenuNodePassive_3_3;
	MenuNodePassive_3_4.PreviousNode=&MenuNodePassive_3;
	MenuNodePassive_3_4.MenuPosition=Middle;
	MenuNodePassive_3_4.Title="Menu 3...           ";
	MenuNodePassive_3_4.ItemToPrint =" Nenu 3_4           ";
	MenuNodePassive_3_4.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_3_5.DownNode=&MenuNodePassive_3_6;
	MenuNodePassive_3_5.NextNode=&MenuNodePassive_3_5;
	MenuNodePassive_3_5.UpNode=&MenuNodePassive_3_4;
	MenuNodePassive_3_5.PreviousNode=&MenuNodePassive_3;
	MenuNodePassive_3_5.MenuPosition=Middle;
	MenuNodePassive_3_5.Title="Menu 3...           ";
	MenuNodePassive_3_5.ItemToPrint =" Nenu 3_5           ";
	MenuNodePassive_3_5.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_3_6.DownNode=&MenuNodePassive_3_7;
	MenuNodePassive_3_6.NextNode=&MenuNodePassive_3_6;
	MenuNodePassive_3_6.UpNode=&MenuNodePassive_3_5;
	MenuNodePassive_3_6.PreviousNode=&MenuNodePassive_3;
	MenuNodePassive_3_6.MenuPosition=Middle;
	MenuNodePassive_3_6.Title="Menu 3...           ";
	MenuNodePassive_3_6.ItemToPrint =" Nenu 3_6           ";
	MenuNodePassive_3_6.NodeExecFct=Menu_PassivePrint;

	// procéder dans cet ordre c'est plus logique :
	MenuNodePassive_3_7.UpNode=&MenuNodePassive_3_6;
	MenuNodePassive_3_7.NextNode=&MenuNodePassive_3_7;
	MenuNodePassive_3_7.DownNode=&MenuNodePassive_3_7;
	MenuNodePassive_3_7.PreviousNode=&MenuNodePassive_3;
	MenuNodePassive_3_7.MenuPosition=Last;
	MenuNodePassive_3_7.Title="Menu 3...           ";
	MenuNodePassive_3_7.ItemToPrint =" Nenu 3_7           ";
	MenuNodePassive_3_7.NodeExecFct=Menu_PassivePrint;


	/*      **********************
	 * 		*Volt Param [V]...   *
	 * 		* Min :				 *
	 * 		* Max :	             *
	 * 		* Actual :           *
	 * 		**********************
	 */

	// procéder dans cet ordre c'est plus logique :
	MenuNodeInput_VoltParam.UpNode=&MenuNodeInput_VoltParam;
	MenuNodeInput_VoltParam.NextNode=&MenuNodeInput_VoltParam;
	MenuNodeInput_VoltParam.DownNode=&MenuNodeInput_VoltParam;
	MenuNodeInput_VoltParam.PreviousNode=&MenuNodePassive_3_1;
	MenuNodeInput_VoltParam.MenuPosition=Last;
	MenuNodeInput_VoltParam.Title="Volt Param [V]...   ";
	MenuNodeInput_VoltParam.ItemToPrint =" Actual :           ";
	MenuNodeInput_VoltParam.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_VoltParam.Param=&VoltageParam;


	/*............................................
	 *		chargement des param
	 *		pour les menus saisie
	.............................................*/
	VoltageParam.Val=12.5;
	VoltageParam.MinVal=12.0;
	VoltageParam.MaxVal=13.0;
	VoltageParam.NewUserInputFlag=0;
	VoltageParam.DigitNb=4;
	VoltageParam.DecimalNb=2;

//===================================================================================================
// ========================== TESTS FORMATS FRACT ===================================================
//===================================================================================================

	/*      **********************
	 * 		*Volt Param 10...    *
	 * 		* Min :				 *
	 * 		* Max :	             *
	 * 		* Actual :           *
	 * 		**********************
	 */

	MenuNodeInput_10.UpNode=&MenuNodeInput_10;
	MenuNodeInput_10.NextNode=&MenuNodeInput_10;
	MenuNodeInput_10.DownNode=&MenuNodeInput_10;
	MenuNodeInput_10.PreviousNode=&MenuNodePassive_3_2;
	MenuNodeInput_10.MenuPosition=Last;
	MenuNodeInput_10.Title="Volt Param 10...    ";
	MenuNodeInput_10.ItemToPrint =" Actual :           ";
	MenuNodeInput_10.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_10.Param=&Param_10;

	Param_10.Val=1.5;
	Param_10.MinVal=1.0;
	Param_10.MaxVal=3.0;
	Param_10.NewUserInputFlag=0;
	Param_10.DigitNb=1;
	Param_10.DecimalNb=0;

	//===================20=====================================
	MenuNodeInput_20.UpNode=&MenuNodeInput_20;
	MenuNodeInput_20.NextNode=&MenuNodeInput_20;
	MenuNodeInput_20.DownNode=&MenuNodeInput_20;
	MenuNodeInput_20.PreviousNode=&MenuNodePassive_3_3;
	MenuNodeInput_20.MenuPosition=Last;
	MenuNodeInput_20.Title="Volt Param 20...    ";
	MenuNodeInput_20.ItemToPrint =" Actual :           ";
	MenuNodeInput_20.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_20.Param=&Param_20;

	Param_20.Val=10.0;
	Param_20.MinVal=5.0;
	Param_20.MaxVal=99.0;
	Param_20.NewUserInputFlag=0;
	Param_20.DigitNb=2;
	Param_20.DecimalNb=0;

	//=====================21===================================
	MenuNodeInput_21.UpNode=&MenuNodeInput_21;
	MenuNodeInput_21.NextNode=&MenuNodeInput_21;
	MenuNodeInput_21.DownNode=&MenuNodeInput_21;
	MenuNodeInput_21.PreviousNode=&MenuNodePassive_3_4;
	MenuNodeInput_21.MenuPosition=Last;
	MenuNodeInput_21.Title="Volt Param 21...    ";
	MenuNodeInput_21.ItemToPrint =" Actual :           ";
	MenuNodeInput_21.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_21.Param=&Param_21;

	Param_21.Val=1.5;
	Param_21.MinVal=0;
	Param_21.MaxVal=9.0;
	Param_21.NewUserInputFlag=0;
	Param_21.DigitNb=2;
	Param_21.DecimalNb=1;
}


/*==========================================================================================
 *
 * 		USER ZONE END
 *
 ==========================================================================================*/

















/*------------------------------------------------
*  LA VARIABLE D ETAT DU MODULE
*   c'est elle qui permet de savoir à quel Item (node)
*   nous en sommes
-------------------------------------------------*/

typedef enum  {Menu_Up, Menu_Down, Menu_Left, Menu_Right } Menu_Cmd;
typedef enum  {Entry, SetValue, Discard, Confirm,Retry} InputNodeSt;
struct {
	Menu_Node *  ActualNode;
	Menu_Cmd Cmde;

	InputNodeSt State;
	Menu_Node * LinkPreviousNode;
	char StringInputValue[8]; // max 6 digit + '.' + Null
	int CursorPos; // 0 sur le premier digit , 6 maxi.
} Menu_Status;





/******************************************************************************
*  PRIVATE declaration

******************************************************************************/


void Menu_NodeUpdate(void);


/******************************************************************************
*  PUBLIC FUNCTION

******************************************************************************/


void Menu_Init(UART_HandleTypeDef * UsedUSART)
{
	/*************************************
	 * init attribut MenuStatus
	 *
	 *************************************/
	Menu_Status.ActualNode=&MenuNodePassive_1;




	//Initialisation des maillons de la châine
	Menu_Node_Init();
	// Initialisation ComUART en IT callback
	ComUART_Init_IT(UsedUSART, Menu_NodeUpdate);
	// Init du ParamNull
	ParamNull.MaxVal=0.0;
	ParamNull.MinVal=0.0;
	ParamNull.Val=0.0;
	ParamNull.NewUserInputFlag=0;
	ParamNull.DigitNb=0;


	Menu_Status.State=Entry;

	// test $$$$$$$$$$$$$$$$$$$$$$$$$$
	if (StringFct_Float2Str(12.36,Tab, 5, 3)==0) MenuInput_PrintPlantageConv();

	res=StringFct_Str2Float(Tab, 5, 3);
	if (res>1000000.0) MenuInput_PrintPlantageConv();
	// fin test $$$$$$$$$$$$$$$$$$$$$$$$$$
}


void Menu_LoadParam(int Param,  Menu_ParamValTypedef * ParamValues)
{
	//Menu_ParamTable[Param]=ParamValues;
}




/******************************************************************************
*  PRIVATE functions : FONCTION DE MISE A JOUR DES ITEM
*  appelée en IT ou timer
******************************************************************************/

void Menu_NodeUpdate(void)
{
	char Touch;
	Touch=ComUART_GetCmd();
	switch(Touch)
	{
	case Uart_Up_R:
		{
		Menu_Status.Cmde=Menu_Up;
		Menu_Status.ActualNode=Menu_Status.ActualNode->UpNode;
		break;
		}
	case Uart_Up_L:
		{
		Menu_Status.Cmde=Menu_Up;
		Menu_Status.ActualNode=Menu_Status.ActualNode->UpNode;
		break;
		}
	case Uart_Down_R:
		{
		Menu_Status.Cmde=Menu_Down;
		Menu_Status.ActualNode=Menu_Status.ActualNode->DownNode;
		break;
		}
	case Uart_Down_L:
		{
		Menu_Status.Cmde=Menu_Down;
		Menu_Status.ActualNode=Menu_Status.ActualNode->DownNode;
		break;
		}
	case Uart_Left_R:
		{
		Menu_Status.Cmde=Menu_Left;
		Menu_Status.ActualNode=Menu_Status.ActualNode->PreviousNode;
		break;
		}
	case Uart_Left_L:
		{
		Menu_Status.Cmde=Menu_Left;
		Menu_Status.ActualNode=Menu_Status.ActualNode->PreviousNode;
		break;
		}
	case Uart_Right_R:
		{
		Menu_Status.Cmde=Menu_Right;
		Menu_Status.ActualNode=Menu_Status.ActualNode->NextNode;
		break;
		}
	case Uart_Right_L:
		{
		Menu_Status.Cmde=Menu_Right;
		Menu_Status.ActualNode=Menu_Status.ActualNode->NextNode;
		break;
		}
	}

	Menu_Status.ActualNode->NodeExecFct();



}




/******************************************************************************
*  PRIVATE functions : FONCTIONS D'AFFICHAGE PASSIF

******************************************************************************/
void Menu_PassivePrint(void)
{

	ComUART_Print(Menu_Status.ActualNode->Title, 20);
	ComUART_Print("\r\n", 2);
	switch (Menu_Status.ActualNode->MenuPosition)
	{
		case First:
		{
			//Ligne 2 Actual
			ComUART_Print(">", 1);
			ComUART_Print(Menu_Status.ActualNode->ItemToPrint,19);
			ComUART_Print("\r\n", 2);
			//Ligne 3 si...
			if (Menu_Status.ActualNode->DownNode!=Menu_Status.ActualNode) // existe un item en dessous
			{
				ComUART_Print(" ", 1);
				ComUART_Print(Menu_Status.ActualNode->DownNode->ItemToPrint,20);
				ComUART_Print("\r\n", 2);
			}
			else
			{
				ComUART_Print("                    ",20); //clr
				ComUART_Print("\r\n", 2);
			}
			//Ligne 4 si...
			if (Menu_Status.ActualNode->DownNode->DownNode!=Menu_Status.ActualNode->DownNode) // existe un item en dessous
			{
				ComUART_Print(" ", 1);
				ComUART_Print(Menu_Status.ActualNode->DownNode->DownNode->ItemToPrint,20);
				ComUART_Print("\r\n", 2);
			}
			else
			{
				ComUART_Print("                    ",20); //clr
				ComUART_Print("\r\n", 2);
			}
			break;
		}

		case Middle:
		{
			// il existe forcément un item au dessus et au dessous sinon il
			// serait First ou Last
			// ligne 2
			ComUART_Print(" ", 1);
			ComUART_Print(Menu_Status.ActualNode->UpNode->ItemToPrint,20);
			ComUART_Print("\r\n", 2);
			// ligne 3 Acutal
			ComUART_Print(">", 1);
			ComUART_Print(Menu_Status.ActualNode->ItemToPrint,19);
			ComUART_Print("\r\n", 2);
			// ligne 4
			ComUART_Print(" ", 1);
			ComUART_Print(Menu_Status.ActualNode->DownNode->ItemToPrint,20);
			ComUART_Print("\r\n", 2);
			break;
		}
		case Last:
		{
			// l'item précédent est soit Middle soit First.
			// La ligne 2 peut être écrite par le node n-2; la ligne 3 par le node n-1, la ligne 3 atuel
			// ou la ligne 2 est écrite par le node n-1, la ligne 2 par l'actuel, ligne 3 vide

			if (Menu_Status.ActualNode->UpNode->UpNode!=Menu_Status.ActualNode->UpNode)
			// l'item n-2 n'est pas un first
			{
				//ligne 2
				ComUART_Print(" ", 1);
				ComUART_Print(Menu_Status.ActualNode->UpNode->UpNode->ItemToPrint,20);
				ComUART_Print("\r\n", 2);
				// ligne 3
				ComUART_Print(" ", 1);
				ComUART_Print(Menu_Status.ActualNode->UpNode->ItemToPrint,20);
				ComUART_Print("\r\n", 2);
				// ligne 4
				ComUART_Print(">", 1);
				ComUART_Print(Menu_Status.ActualNode->ItemToPrint,19);
				ComUART_Print("\r\n", 2);

			}
			else // l'item n-2 n'existe pas
			{
				//ligne 2
				ComUART_Print(" ", 1);
				ComUART_Print(Menu_Status.ActualNode->UpNode->ItemToPrint,20);
				ComUART_Print("\r\n", 2);
				// ligne 3
				ComUART_Print(">", 1);
				ComUART_Print(Menu_Status.ActualNode->ItemToPrint,19);
				ComUART_Print("\r\n", 2);
				// ligne 4
				ComUART_Print(" ", 1);
				ComUART_Print("                    ",20); //clr
				ComUART_Print("\r\n", 2);
			}
			break;
		}

	}
}

/******************************************************************************
*  PRIVATE functions : FONCTIONS D'AFFICHAGE TYPE SAISIE

******************************************************************************/

/*............................................
 *		Les 3 affichages ...
.............................................*/
void MenuInput_PrintParam(void)  // var dynamique dans Menu_Status ...
{
	char MyString[8];
	// Line 1
	ComUART_Print(Menu_Status.ActualNode->Title, 20);
	ComUART_Print("\r\n", 2);
	// Line 2
	ComUART_Print(" Min : ", 7);
	if (StringFct_Float2Str(Menu_Status.ActualNode->Param->MinVal,MyString, Menu_Status.ActualNode->Param->DigitNb, Menu_Status.ActualNode->Param->DecimalNb)==0) MenuInput_PrintPlantageConv() ;
	ComUART_Print(MyString, Menu_Status.ActualNode->Param->DigitNb +1);
	ComUART_Print("\r\n", 2);
	// Line 3
	ComUART_Print(" Max : ", 7);
	if (StringFct_Float2Str(Menu_Status.ActualNode->Param->MaxVal,MyString, Menu_Status.ActualNode->Param->DigitNb, Menu_Status.ActualNode->Param->DecimalNb)==0) MenuInput_PrintPlantageConv() ;
	ComUART_Print(MyString, Menu_Status.ActualNode->Param->DigitNb +1);
	ComUART_Print("\r\n", 2);
	// line 4
	ComUART_Print(" Actual: ", 9);
	ComUART_Print(Menu_Status.StringInputValue, Menu_Status.ActualNode->Param->DigitNb +1);
	ComUART_Print("\r\n", 2);
}

void MenuInput_PrintDiscard(void)
{
	ComUART_Print("Discard Changes ?   ",20);
	ComUART_Print("\r\n", 2);
	ComUART_Print("Back for yes...     ",20);
	ComUART_Print("\r\n", 2);
	ComUART_Print("Forward for no...   ",20);
	ComUART_Print("\r\n", 2);
	ComUART_Print("                    ",20); //clr
	ComUART_Print("\r\n", 2);
}

void MenuInput_PrintConfirm(void)
{
	ComUART_Print("Confirm Changes ?   ",20);
	ComUART_Print("\r\n", 2);
	ComUART_Print("Forward for yes...  ",20);
	ComUART_Print("\r\n", 2);
	ComUART_Print("Back for no...      ",20);
	ComUART_Print("\r\n", 2);
	ComUART_Print("                    ",20); //clr
	ComUART_Print("\r\n", 2);
}

void MenuInput_PrintRetry(void)
{
	ComUART_Print("Value out of range, ",20);
	ComUART_Print("\r\n", 2);
	ComUART_Print("Please retry        ",20);
	ComUART_Print("\r\n", 2);
	ComUART_Print("Strike any key ...  ",20);
	ComUART_Print("\r\n", 2);
	ComUART_Print("                    ",20); //clr
	ComUART_Print("\r\n", 2);
}

void MenuInput_PrintPlantageConv(void)
{
	ComUART_Print("Plantage sys        ",20);
	ComUART_Print("\r\n", 2);
	ComUART_Print("Pb conv float / str ",20);
	ComUART_Print("\r\n", 2);
	ComUART_Print("Revoir les datas des ",20);
	ComUART_Print("\r\n", 2);
	ComUART_Print("param MenuNode       ",20); //clr
	ComUART_Print("\r\n", 2);
	while(1);
}
/*............................................
 *		La machine à états...
.............................................*/

void Menu_InputStateMachine(void)
{
	//char MyString[8];
	float Valeur;
	char DigitNb, DecNb;
	char Tamp;
	int PosVirgule;
	int PosMax;

	// chargement local de la valeur à traiter
	Valeur=Menu_Status.ActualNode->Param->Val;
	DigitNb=Menu_Status.ActualNode->Param->DigitNb;
	DecNb=Menu_Status.ActualNode->Param->DecimalNb;

	switch(Menu_Status.State)
	{

		case  Entry:
		{
			// prochaine étape, SetValue forcément
			Menu_Status.State=SetValue;
			// link back
			Menu_Status.LinkPreviousNode=Menu_Status.ActualNode->PreviousNode; // pour pouvoir revenir
            // blocage dans cet item
			Menu_Status.ActualNode->PreviousNode=Menu_Status.ActualNode;
			// prépa premier digit pour réglage
			Menu_Status.CursorPos=0;
			if (StringFct_Float2Str(Valeur,Menu_Status.StringInputValue, DigitNb, DecNb)==0) MenuInput_PrintPlantageConv();
			// affichage
			MenuInput_PrintParam();
			break;
		}

		case  SetValue:
		{
			// calcule position Virgule
			//|xx.xxx00| DigitNb=5, DecimalNb=2
			//|x.xx0000| DigitNb=3, DecimalNb=2
			//|x.000000| DigitNb=1, DecimalNb=0
			PosVirgule= Menu_Status.ActualNode->Param->DigitNb-Menu_Status.ActualNode->Param->DecimalNb;
			PosMax=Menu_Status.ActualNode->Param->DigitNb;

			if (Menu_Status.Cmde==Menu_Up)
			{
				// update digit highlihted (curseur)
				if (Menu_Status.CursorPos==PosVirgule) Menu_Status.CursorPos--; // si on tombe sur le '.' ..
				Tamp=Menu_Status.StringInputValue[Menu_Status.CursorPos];
				Tamp++;
				if (Tamp==0x3A) Tamp=0x30;
				Menu_Status.StringInputValue[Menu_Status.CursorPos]=Tamp;
				// affichage
				MenuInput_PrintParam();
			}
			else if (Menu_Status.Cmde==Menu_Down)
			{
				if (Menu_Status.CursorPos==PosVirgule) Menu_Status.CursorPos--; // si on tombe sur le '.' ..
				Tamp=Menu_Status.StringInputValue[Menu_Status.CursorPos];
				Tamp--;
				if (Tamp==0x2F) Tamp=0x39;
				Menu_Status.StringInputValue[Menu_Status.CursorPos]=Tamp;
				// affichage
				MenuInput_PrintParam();
			}

			else if (Menu_Status.Cmde==Menu_Left) // Discard ? or Cursor --
			{
				if (Menu_Status.CursorPos==0) // Discard ?
				{
					Menu_Status.State=Discard;
					MenuInput_PrintDiscard();
				}
				else // cursor --
				{
					Menu_Status.CursorPos--;
					// gestion '.' que l'on saute si nécessaire
					if (Menu_Status.CursorPos==PosVirgule) Menu_Status.CursorPos--;
					// affichage
					MenuInput_PrintParam();
				}
			}
			else if (Menu_Status.Cmde==Menu_Right) // Confirm ? or Cursor ++
			{
				if (Menu_Status.CursorPos==PosMax) // Confirm ?
				{
					Menu_Status.State=Confirm;
					MenuInput_PrintConfirm();
				}
				else // cursor ++
				{
					Menu_Status.CursorPos++;
					// gestion '.' que l'on saute si nécessaire (uniquement si fract>0)
					if ((Menu_Status.CursorPos==PosVirgule)&&(DecNb>0)) Menu_Status.CursorPos++;
					// affichage
					MenuInput_PrintParam();
				}

			}
			break;
		}
		case Discard:
		{
			if (Menu_Status.Cmde==Menu_Left) // confirmation du Discard
			{
				// restitution pointeur back du menu actuel
				Menu_Status.ActualNode->PreviousNode=Menu_Status.LinkPreviousNode;
				Menu_Status.State=Entry;
				// basculement node père
				Menu_Status.ActualNode=Menu_Status.ActualNode->PreviousNode;
				// Affichage menu passif
				Menu_PassivePrint();
			}
			else if (Menu_Status.Cmde==Menu_Right) // abandon du Discard
			{
				Menu_Status.State=SetValue;
				// affichage
				MenuInput_PrintParam();
			}
			break;
		}
		case Confirm:
		{
			if (Menu_Status.Cmde==Menu_Right) // confirmation de la validation
			{
				// écriture valeur dans param
				Valeur=StringFct_Str2Float(Menu_Status.StringInputValue,Menu_Status.ActualNode->Param->DigitNb,Menu_Status.ActualNode->Param->DecimalNb);
				if (Valeur>1000000.0) MenuInput_PrintPlantageConv();
				if ((Valeur<=Menu_Status.ActualNode->Param->MaxVal)&&(Valeur>=Menu_Status.ActualNode->Param->MinVal))
				{
					// la valeur est OK
					Menu_Status.ActualNode->Param->Val=Valeur;
					Menu_Status.ActualNode->Param->NewUserInputFlag=1;
					// restitution pointeur back du menu actuel
					Menu_Status.ActualNode->PreviousNode=Menu_Status.LinkPreviousNode;
					Menu_Status.State=Entry;
					// basculement node père
					Menu_Status.ActualNode=Menu_Status.ActualNode->PreviousNode;
					Menu_PassivePrint();
				}
				else // valeur KO
				{
					Menu_Status.State=Retry;
					MenuInput_PrintRetry();
				}

			}
			else if (Menu_Status.Cmde==Menu_Left) // abandon du Discard
			{
				Menu_Status.State=SetValue;
				// affichage
				MenuInput_PrintParam();
			}
			break;
		}
		case Retry:
		{
			// prochaine étape, SetValue forcément
			Menu_Status.State=SetValue;
			Menu_Status.CursorPos=0;
			MenuInput_PrintParam();
			break;
		}
	}






}





/**
  * @brief
  */
