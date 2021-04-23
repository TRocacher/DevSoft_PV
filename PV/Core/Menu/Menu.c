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

Menu_Node Item_Menu1, Item_Menu2, Item_Menu3, Item_Menu4,  Item_Null;


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
		break;
		}
	case Uart_Up_L:
		{
		Menu_Status.ActualNode=Menu_Status.ActualNode->UpNode;
		break;
		}
	case Uart_Down_R:
		{
		Menu_Status.ActualNode=Menu_Status.ActualNode->DownNode;
		break;
		}
	case Uart_Down_L:
		{
		Menu_Status.ActualNode=Menu_Status.ActualNode->DownNode;
		break;
		}
	case Uart_Left_R:
		{
		Menu_Status.ActualNode=Menu_Status.ActualNode->PreviousNode;
		break;
		}
	case Uart_Left_L:
		{
		Menu_Status.ActualNode=Menu_Status.ActualNode->PreviousNode;
		break;
		}
	case Uart_Right_R:
		{
		Menu_Status.ActualNode=Menu_Status.ActualNode->NextNode;
		break;
		}
	case Uart_Right_L:
		{
		Menu_Status.ActualNode=Menu_Status.ActualNode->NextNode;
		break;
		}
	}


}




/******************************************************************************
*  PRIVATE functions

******************************************************************************/




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
	 * 		*1> Nenu 1			 *
	 * 		*2> Menu 2			 *
	 * 		*3> Menu 3			 *
	 * 		*4> Menu 4			 *
	 * 		**********************
	 */
	Item_Menu1.DownNode=&Item_Menu2;
	Item_Menu1.NextNode=& Item_Null;
	Item_Menu1.UpNode=&Item_Menu1;  // lui même
	Item_Menu1.PreviousNode=&Item_Menu1;  // lui même
	Item_Menu1.MenuPosition=First;
	Item_Menu1.Title="Menu principal      ";
	Item_Menu1.ItemToPrint ="1> Nenu 1			 ";
	Item_Menu1.NodeExecFct=NullFct;

	Item_Menu2.DownNode=&Item_Menu3;
	Item_Menu2.NextNode=& Item_Null;
	Item_Menu2.UpNode=&Item_Menu1;
	Item_Menu2.PreviousNode=&Item_Menu2;  // lui même
	Item_Menu2.MenuPosition=Middle;
	Item_Menu2.Title="Menu principal      ";
	Item_Menu2.ItemToPrint ="2> Menu 2			 ";
	Item_Menu2.NodeExecFct=NullFct;

	Item_Menu3.DownNode=&Item_Menu4;
	Item_Menu3.NextNode=& Item_Null;
	Item_Menu3.UpNode=&Item_Menu2;
	Item_Menu3.PreviousNode=&Item_Menu3;  // lui même
	Item_Menu3.MenuPosition=Middle;
	Item_Menu3.Title="Menu principal      ";
	Item_Menu3.ItemToPrint ="3> Menu 3			 ";
	Item_Menu3.NodeExecFct=NullFct;

	Item_Menu4.DownNode=&Item_Menu4; // lui même
	Item_Menu4.NextNode=& Item_Null;
	Item_Menu4.UpNode=&Item_Menu3;
	Item_Menu4.PreviousNode=&Item_Menu4;  // lui même
	Item_Menu4.MenuPosition=Last;
	Item_Menu4.Title="Menu principal      ";
	Item_Menu4.ItemToPrint ="3> Menu 4			 ";
	Item_Menu4.NodeExecFct=NullFct;

	/*      **********************
	 * 		*Menu 1			     *
	 * 		*1> Nenu 1_1		 *
	 * 		*2> Menu 1_2		 *
	 * 		*3> Menu 1_3		 *
	 * 		**********************
	 */

	/*      **********************
	 * 		*Menu 2			     *
	 * 		*1> Nenu 2_1		 *
	 * 		*2> Menu 2_2		 *
	 * 		**********************
	 */


	/*      **********************
	 * 		*Menu 2_1			 *
	 * 		*1> Nenu 2_1_1		 *
	 * 		*2> Menu 2_1_2		 *
	 * 		*3> Menu 2_1_3		 *
	 * 		*3> Menu 2_1_4		 *
	 * 		**********************
	 */

	/*      **********************
	 * 		*Menu 4			     *
	 * 		*1> Nenu 4_1		 *
	 * 		*2> Menu 4_2		 *
	 * 		**********************
	 */

	/*      **********************
	 * 		*Menu 5			     *
	 * 		*1> Nenu 5_1		 *
	 * 		*2> Menu 5_2		 *
	 * 		*3> Menu 5_3		 *
	 * 		**********************
	 */
}










/**
  * @brief
  */