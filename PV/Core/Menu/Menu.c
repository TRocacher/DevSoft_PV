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
void Menu_PassiveScreenPrint(void);
void Menu_InputScreenPrint(void);


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

Menu_Node Item_Menu1, Item_Menu2, Item_Menu3, Item_Menu4;
Menu_Node Item_Menu1_1,Item_Menu1_2;
Menu_Node Item_Menu2_1;
Menu_Node Item_Menu3_1,Item_Menu3_2,Item_Menu3_3,Item_Menu3_4,Item_Menu3_5,Item_Menu3_6,Item_Menu3_7;
Menu_Node ItemInput_VoltParam;

/*............................................
 *
 * 		Définitions des structures param
 * 		privée au module
 *
.............................................*/
// Param nul
Menu_ParamValTypedef ParamNull;
// Voltage
Menu_ParamValTypedef  VoltageParam;


/*............................................
 *
 * 		Chargement des maillons MenuNodes
 *      = CONSTRUCTION DES DIVERS MENUS
 *
 *		chargement des param
 *		pour les menus saisie
.............................................*/

// test
char Tab[8];
void Menu_Node_Init(void)
{

	// test
	StringFct_Float2Str(12.36,Tab, 5, 3);

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
	Item_Menu1.DownNode=&Item_Menu2;
	Item_Menu1.NextNode=&Item_Menu1_1;
	Item_Menu1.UpNode=&Item_Menu1;  // lui même
	Item_Menu1.PreviousNode=&Item_Menu1;  // lui même
	Item_Menu1.MenuPosition=First;
	Item_Menu1.Title="Menu principal      ";
	Item_Menu1.ItemToPrint =" Nenu 1             ";
	Item_Menu1.NodeExecFct=Menu_PassiveScreenPrint;
	Item_Menu1.Param=&ParamNull; //------------------------------------------------------------------

	Item_Menu2.DownNode=&Item_Menu3;
	Item_Menu2.NextNode=&Item_Menu2_1;
	Item_Menu2.UpNode=&Item_Menu1;
	Item_Menu2.PreviousNode=&Item_Menu2;  // lui même
	Item_Menu2.MenuPosition=Middle;
	Item_Menu2.Title="Menu principal      ";
	Item_Menu2.ItemToPrint =" Nenu 2             ";
	Item_Menu2.NodeExecFct=Menu_PassiveScreenPrint;

	Item_Menu3.DownNode=&Item_Menu4;
	Item_Menu3.NextNode=&Item_Menu3_1;
	Item_Menu3.UpNode=&Item_Menu2;
	Item_Menu3.PreviousNode=&Item_Menu3;  // lui même
	Item_Menu3.MenuPosition=Middle;
	Item_Menu3.Title="Menu principal      ";
	Item_Menu3.ItemToPrint =" Nenu 3             ";
	Item_Menu3.NodeExecFct=Menu_PassiveScreenPrint;

	Item_Menu4.DownNode=&Item_Menu4; // lui même
	Item_Menu4.NextNode=&Item_Menu4;
	Item_Menu4.UpNode=&Item_Menu3;
	Item_Menu4.PreviousNode=&Item_Menu4;  // lui même
	Item_Menu4.MenuPosition=Last;
	Item_Menu4.Title="Menu principal      ";
	Item_Menu4.ItemToPrint =" Nenu 4             ";
	Item_Menu4.NodeExecFct=Menu_PassiveScreenPrint;

	/*      **********************
	 * 		*Menu 1...           *
	 * 		* Nenu 1_1           *
	 * 		* Nenu 1_2           *
	 * 		**********************
	 */

	Item_Menu1_1.DownNode=&Item_Menu1_2;
	Item_Menu1_1.NextNode=&Item_Menu1_1;
	Item_Menu1_1.UpNode=&Item_Menu1_1;
	Item_Menu1_1.PreviousNode=&Item_Menu1;
	Item_Menu1_1.MenuPosition=First;
	Item_Menu1_1.Title="Menu 1...           ";
	Item_Menu1_1.ItemToPrint =" Nenu 1_1           ";
	Item_Menu1_1.NodeExecFct=Menu_PassiveScreenPrint;

	Item_Menu1_2.DownNode=&Item_Menu1_2;
	Item_Menu1_2.NextNode=&Item_Menu1_2;
	Item_Menu1_2.UpNode=&Item_Menu1_1;
	Item_Menu1_2.PreviousNode=&Item_Menu1;
	Item_Menu1_2.MenuPosition=Last;
	Item_Menu1_2.Title="Menu 1...           ";
	Item_Menu1_2.ItemToPrint =" Nenu 1_2           ";
	Item_Menu1_2.NodeExecFct=Menu_PassiveScreenPrint;



	/*      **********************
	 * 		*Menu 2...           *
	 * 		* Nenu 2_1           *
	 * 		**********************
	 */
	Item_Menu2_1.DownNode=&Item_Menu2_1;
	Item_Menu2_1.NextNode=&Item_Menu2_1;
	Item_Menu2_1.UpNode=&Item_Menu2_1;
	Item_Menu2_1.PreviousNode=&Item_Menu2;
	Item_Menu2_1.MenuPosition=First;
	Item_Menu2_1.Title="Menu 2...           ";
	Item_Menu2_1.ItemToPrint =" Nenu 2_1           ";
	Item_Menu2_1.NodeExecFct=Menu_PassiveScreenPrint;



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
	Item_Menu3_1.DownNode=&Item_Menu3_2;
	Item_Menu3_1.NextNode=&ItemInput_VoltParam;
	Item_Menu3_1.UpNode=&Item_Menu3_1;
	Item_Menu3_1.PreviousNode=&Item_Menu3;
	Item_Menu3_1.MenuPosition=First;
	Item_Menu3_1.Title="Menu 3...           ";
	Item_Menu3_1.ItemToPrint =" Nenu 3_1           ";
	Item_Menu3_1.NodeExecFct=Menu_PassiveScreenPrint;

	Item_Menu3_2.DownNode=&Item_Menu3_3;
	Item_Menu3_2.NextNode=&Item_Menu3_2;
	Item_Menu3_2.UpNode=&Item_Menu3_1;
	Item_Menu3_2.PreviousNode=&Item_Menu3;
	Item_Menu3_2.MenuPosition=Middle;
	Item_Menu3_2.Title="Menu 3...           ";
	Item_Menu3_2.ItemToPrint =" Nenu 3_2           ";
	Item_Menu3_2.NodeExecFct=Menu_PassiveScreenPrint;

	Item_Menu3_3.DownNode=&Item_Menu3_4;
	Item_Menu3_3.NextNode=&Item_Menu3_2;
	Item_Menu3_3.UpNode=&Item_Menu3_2;
	Item_Menu3_3.PreviousNode=&Item_Menu3;
	Item_Menu3_3.MenuPosition=Middle;
	Item_Menu3_3.Title="Menu 3...           ";
	Item_Menu3_3.ItemToPrint =" Nenu 3_3           ";
	Item_Menu3_3.NodeExecFct=Menu_PassiveScreenPrint;

	Item_Menu3_4.DownNode=&Item_Menu3_5;
	Item_Menu3_4.NextNode=&Item_Menu3_4;
	Item_Menu3_4.UpNode=&Item_Menu3_3;
	Item_Menu3_4.PreviousNode=&Item_Menu3;
	Item_Menu3_4.MenuPosition=Middle;
	Item_Menu3_4.Title="Menu 3...           ";
	Item_Menu3_4.ItemToPrint =" Nenu 3_4           ";
	Item_Menu3_4.NodeExecFct=Menu_PassiveScreenPrint;

	Item_Menu3_5.DownNode=&Item_Menu3_6;
	Item_Menu3_5.NextNode=&Item_Menu3_5;
	Item_Menu3_5.UpNode=&Item_Menu3_4;
	Item_Menu3_5.PreviousNode=&Item_Menu3;
	Item_Menu3_5.MenuPosition=Middle;
	Item_Menu3_5.Title="Menu 3...           ";
	Item_Menu3_5.ItemToPrint =" Nenu 3_5           ";
	Item_Menu3_5.NodeExecFct=Menu_PassiveScreenPrint;

	Item_Menu3_6.DownNode=&Item_Menu3_7;
	Item_Menu3_6.NextNode=&Item_Menu3_6;
	Item_Menu3_6.UpNode=&Item_Menu3_5;
	Item_Menu3_6.PreviousNode=&Item_Menu3;
	Item_Menu3_6.MenuPosition=Middle;
	Item_Menu3_6.Title="Menu 3...           ";
	Item_Menu3_6.ItemToPrint =" Nenu 3_6           ";
	Item_Menu3_6.NodeExecFct=Menu_PassiveScreenPrint;

	// procéder dans cet ordre c'est plus logique :
	Item_Menu3_7.UpNode=&Item_Menu3_6;
	Item_Menu3_7.NextNode=&Item_Menu3_7;
	Item_Menu3_7.DownNode=&Item_Menu3_7;
	Item_Menu3_7.PreviousNode=&Item_Menu3;
	Item_Menu3_7.MenuPosition=Last;
	Item_Menu3_7.Title="Menu 3...           ";
	Item_Menu3_7.ItemToPrint =" Nenu 3_7           ";
	Item_Menu3_7.NodeExecFct=Menu_PassiveScreenPrint;


	/*      **********************
	 * 		*Volt Param [V]...   *
	 * 		* Min :				 *
	 * 		* Max :	             *
	 * 		* Actual :           *
	 * 		**********************
	 */

	// procéder dans cet ordre c'est plus logique :
	ItemInput_VoltParam.UpNode=&ItemInput_VoltParam;
	ItemInput_VoltParam.NextNode=&ItemInput_VoltParam;
	ItemInput_VoltParam.DownNode=&ItemInput_VoltParam;
	ItemInput_VoltParam.PreviousNode=&Item_Menu3_1;
	ItemInput_VoltParam.MenuPosition=Last;
	ItemInput_VoltParam.Title="Volt Param [V]...   ";
	ItemInput_VoltParam.ItemToPrint =" Actual :           ";
	ItemInput_VoltParam.NodeExecFct=Menu_InputScreenPrint;
	ItemInput_VoltParam.Param=&VoltageParam;




	/*............................................
	 *		chargement des param
	 *		pour les menus saisie
	.............................................*/
	VoltageParam.Val=12.5;
	VoltageParam.MaxVal=13.0;
	VoltageParam.MinVal=12.0;
	VoltageParam.NewUserInputFlag=0;
	VoltageParam.DigitNb=4;
	VoltageParam.DecimalNb=2;
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
struct {
	Menu_Node *  ActualNode;
	Menu_Cmd Cmde;
	char DigitUserInput[10];
} Menu_Status;


/*------------------------------------------------
*  VARIABLE STRUCTUREE User_Input
*  elle contient les champs nécessaire à la saisie
-------------------------------------------------*/
typedef enum  {MenuOnEntry, MenuDo, MenuExit} Menu_ItemState;
struct {
	Menu_ItemState State;
	Menu_Node *  LinkActualNode;
	float BuiltInValue;
	char StringInputValue[8]; // max 6 digit + '.' + Null
	int DigitPos; // 0 sur le premier digit , 6 maxi.
} Menu_UserInput;




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
	Menu_Status.ActualNode=&Item_Menu1;


	for (int i=0 ;i<=9 ;i++)
	{
		Menu_Status.DigitUserInput[i]=0;
	}

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

	Menu_UserInput.BuiltInValue=0.0;
	Menu_UserInput.State=MenuOnEntry;
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
		Menu_Status.ActualNode->NodeExecFct();
		break;
		}
	case Uart_Up_L:
		{
		Menu_Status.Cmde=Menu_Up;
		Menu_Status.ActualNode=Menu_Status.ActualNode->UpNode;
		Menu_Status.ActualNode->NodeExecFct();
		break;
		}
	case Uart_Down_R:
		{
		Menu_Status.Cmde=Menu_Down;
		Menu_Status.ActualNode=Menu_Status.ActualNode->DownNode;
		Menu_Status.ActualNode->NodeExecFct();
		break;
		}
	case Uart_Down_L:
		{
		Menu_Status.Cmde=Menu_Down;
		Menu_Status.ActualNode=Menu_Status.ActualNode->DownNode;
		Menu_Status.ActualNode->NodeExecFct();
		break;
		}
	case Uart_Left_R:
		{
		Menu_Status.Cmde=Menu_Left;
		Menu_Status.ActualNode=Menu_Status.ActualNode->PreviousNode;
		Menu_Status.ActualNode->NodeExecFct();
		break;
		}
	case Uart_Left_L:
		{
		Menu_Status.Cmde=Menu_Left;
		Menu_Status.ActualNode=Menu_Status.ActualNode->PreviousNode;
		Menu_Status.ActualNode->NodeExecFct();
		break;
		}
	case Uart_Right_R:
		{
		Menu_Status.Cmde=Menu_Right;
		Menu_Status.ActualNode=Menu_Status.ActualNode->NextNode;
		Menu_Status.ActualNode->NodeExecFct();
		break;
		}
	case Uart_Right_L:
		{
		Menu_Status.Cmde=Menu_Right;
		Menu_Status.ActualNode=Menu_Status.ActualNode->NextNode;
		Menu_Status.ActualNode->NodeExecFct();
		break;
		}
	}


}




/******************************************************************************
*  PRIVATE functions : FONCTIONS D'AFFICHAGE

******************************************************************************/
void Menu_PassiveScreenPrint(void)
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





void Menu_InputScreenPrint(void)
{
	char MyString[8];
	float Valeur;
	char DigitNb, DecNb;
	char Tamp;


	// Line 1
	ComUART_Print(Menu_Status.ActualNode->Title, 20);
	ComUART_Print("\r\n", 2);
	// Line 2
	ComUART_Print(" Min : ", 7);
	StringFct_Float2Str(Menu_Status.ActualNode->Param->MinVal,MyString, Menu_Status.ActualNode->Param->DigitNb, Menu_Status.ActualNode->Param->DecimalNb);
	ComUART_Print(MyString, Menu_Status.ActualNode->Param->DigitNb +1);
	ComUART_Print("\r\n", 2);
	// Line 3
	ComUART_Print(" Max : ", 7);
	StringFct_Float2Str(Menu_Status.ActualNode->Param->MaxVal,MyString, Menu_Status.ActualNode->Param->DigitNb, Menu_Status.ActualNode->Param->DecimalNb);
	ComUART_Print(MyString, Menu_Status.ActualNode->Param->DigitNb +1);
	ComUART_Print("\r\n", 2);


	switch(Menu_UserInput.State)
	{
		case  MenuOnEntry:
		{
			Menu_UserInput.State=MenuDo;
			Menu_UserInput.LinkActualNode=Menu_Status.ActualNode->PreviousNode; // pour pouvoir revenir
			Menu_UserInput.DigitPos=0;
			// Line 4
			Valeur=Menu_Status.ActualNode->Param->Val;
			DigitNb=Menu_Status.ActualNode->Param->DigitNb;
			DecNb=Menu_Status.ActualNode->Param->DecimalNb;
			ComUART_Print(" Actual: ", 9);
			StringFct_Float2Str(Valeur,Menu_UserInput.StringInputValue, DigitNb, DecNb);
			ComUART_Print(Menu_UserInput.StringInputValue, Menu_Status.ActualNode->Param->DigitNb +1);
			ComUART_Print("\r\n", 2);
		}
		case  MenuDo:
		{
			switch(Menu_Status.Cmde)
			{
			case Menu_Up:
				{
					Tamp=Menu_UserInput.StringInputValue[Menu_UserInput.DigitPos];
					Tamp++;
					if (Tamp==0x3A) Tamp=0x30;
					Menu_UserInput.StringInputValue[Menu_UserInput.DigitPos]=Tamp;
					// Line 4
					ComUART_Print(" Actual: ", 9);
					ComUART_Print(Menu_UserInput.StringInputValue, Menu_Status.ActualNode->Param->DigitNb +1);
					ComUART_Print("\r\n", 2);
				}
				break;

			case Menu_Down:
				{
					Tamp=Menu_UserInput.StringInputValue[Menu_UserInput.DigitPos];
					Tamp--;
					if (Tamp==0x2F) Tamp=0x39;
					Menu_UserInput.StringInputValue[Menu_UserInput.DigitPos]=Tamp;
					// Line 4
					ComUART_Print(" Actual: ", 9);
					ComUART_Print(Menu_UserInput.StringInputValue, Menu_Status.ActualNode->Param->DigitNb +1);
					ComUART_Print("\r\n", 2);
				}
				break;

			}
			break;


		}
		case MenuExit:
		{
			Menu_Status.ActualNode->PreviousNode=Menu_UserInput.LinkActualNode;
			Menu_UserInput.State=MenuOnEntry;
			break;

		}
	}






}





/**
  * @brief
  */
