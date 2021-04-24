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

// type Menu_Node
enum {First, Middle, Last};
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
};

Menu_Node Item_Menu1, Item_Menu2, Item_Menu3, Item_Menu4;
Menu_Node Item_Menu1_1,Item_Menu1_2;
Menu_Node Item_Menu2_1;
Menu_Node Item_Menu3_1,Item_Menu3_2,Item_Menu3_3,Item_Menu3_4,Item_Menu3_5,Item_Menu3_6,Item_Menu3_7;




//Variable Menu_Status

struct {
	Menu_Node *  ActualNode;
	float UserInput;
	char NewUserInputFlag;
	char DigitUserInput[10];
} Menu_Status;







struct MenuScreenTypedef {
	// Menu Input Menu
	char NbItemsToPrint;
	char InputValueDigits[10]; // pour input menu, c'est les digits de l'input Value (non utilisé 			    // directement pour l'affichage)
	char LineColToHighLight[2];// contient la coordonnée du caractère actif
	// Menu Navigation et Information
	char * PtrLine[10]; // ce sont 10 pointeurs vers 10 strings maximum
	char FirstItemToPrint; // utile pour le scrolling, on affiche la première ligne puis 3 suivantes en 		                 //partant de  FirstLineToPrint
	char ActiveItem;         // l'item à mettre en surbrillance, -1 pour retour en arrière
} MenuScreenData;








/******************************************************************************
*  PRIVATE declaration

******************************************************************************/

void Menu_Node_Init(void);
void Menu_NodeUpdate(void);


/******************************************************************************
*  PUBLIC FUNCTION

******************************************************************************/


void Init_Menu(UART_HandleTypeDef * UsedUSART)
{
	/*************************************
	 * init attribut MenuStatus
	 *
	 *************************************/
	Menu_Status.ActualNode=&Item_Menu1;
	Menu_Status.NewUserInputFlag=0;
	Menu_Status.UserInput=0.0;

	for (int i=0 ;i<=9 ;i++)
	{
		Menu_Status.DigitUserInput[i]=0;
	}

	//Initialisation des maillons de la châine
	Menu_Node_Init();
	// Initialisation ComUART en IT callback
	ComUART_Init_IT(UsedUSART, Menu_NodeUpdate);
}


#define Uart_Up_R 'o'
#define Uart_Down_R 'l'
#define Uart_Left_R 'k'
#define Uart_Right_R 'm'

void Menu_NodeUpdate(void)
{
	char Touch;
	Touch=ComUART_GetCmd();
	switch(Touch)
	{
	case Uart_Up_R:
		{
		Menu_Status.ActualNode=Menu_Status.ActualNode->UpNode;
		Menu_Status.ActualNode->NodeExecFct();
		break;
		}
	case Uart_Up_L:
		{
		Menu_Status.ActualNode=Menu_Status.ActualNode->UpNode;
		Menu_Status.ActualNode->NodeExecFct();
		break;
		}
	case Uart_Down_R:
		{
		Menu_Status.ActualNode=Menu_Status.ActualNode->DownNode;
		Menu_Status.ActualNode->NodeExecFct();
		break;
		}
	case Uart_Down_L:
		{
		Menu_Status.ActualNode=Menu_Status.ActualNode->DownNode;
		Menu_Status.ActualNode->NodeExecFct();
		break;
		}
	case Uart_Left_R:
		{
		Menu_Status.ActualNode=Menu_Status.ActualNode->PreviousNode;
		Menu_Status.ActualNode->NodeExecFct();
		break;
		}
	case Uart_Left_L:
		{
		Menu_Status.ActualNode=Menu_Status.ActualNode->PreviousNode;
		Menu_Status.ActualNode->NodeExecFct();
		break;
		}
	case Uart_Right_R:
		{
		Menu_Status.ActualNode=Menu_Status.ActualNode->NextNode;
		Menu_Status.ActualNode->NodeExecFct();
		break;
		}
	case Uart_Right_L:
		{
		Menu_Status.ActualNode=Menu_Status.ActualNode->NextNode;
		Menu_Status.ActualNode->NodeExecFct();
		break;
		}
	}


}




/******************************************************************************
*  PRIVATE functions

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
			ComUART_Print(Menu_Status.ActualNode->ItemToPrint,18);
			ComUART_Print(" >\r\n", 4);
			//Ligne 3 si...
			if (Menu_Status.ActualNode->DownNode!=Menu_Status.ActualNode) // existe un item en dessous
			{
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
			ComUART_Print(Menu_Status.ActualNode->UpNode->ItemToPrint,20);
			ComUART_Print("\r\n", 2);
			// ligne 3 Acutal
			ComUART_Print(Menu_Status.ActualNode->ItemToPrint,18);
			ComUART_Print(" >\r\n", 4);
			// ligne 4
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
				ComUART_Print(Menu_Status.ActualNode->UpNode->UpNode->ItemToPrint,20);
				ComUART_Print("\r\n", 2);
				// ligne 3
				ComUART_Print(Menu_Status.ActualNode->UpNode->ItemToPrint,20);
				ComUART_Print("\r\n", 2);
				// ligne 4
				ComUART_Print(Menu_Status.ActualNode->ItemToPrint,18);
				ComUART_Print(" >\r\n", 4);

			}
			else // l'item n-2 n'existe pas
			{
				//ligne 2
				ComUART_Print(Menu_Status.ActualNode->UpNode->ItemToPrint,20);
				ComUART_Print("\r\n", 2);
				// ligne 3
				ComUART_Print(Menu_Status.ActualNode->ItemToPrint,18);
				ComUART_Print(" >\r\n", 4);
				// ligne 4
				ComUART_Print("                    ",20); //clr
				ComUART_Print("\r\n", 2);
			}
			break;
		}

	}
}





/*==========================================================================================
 *
 * 		Chargement des maillons MenuNodes
 *
 *
 ==========================================================================================*/


void NullFct(void) {}

void Menu_Node_Init(void)
{

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
	 * 		* Nenu 3_1           *
	 * 		* Nenu 3_2           *
	 * 		* Nenu 3_3           *
	 * 		* Nenu 3_4           *
	 * 		* Nenu 3_5           *
	 * 		* Nenu 3_6           *
	 * 		* Nenu 3_7           *
	 * 		**********************
	 */
	Item_Menu3_1.DownNode=&Item_Menu3_2;
	Item_Menu3_1.NextNode=&Item_Menu3_1;
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

}










/**
  * @brief
  */
