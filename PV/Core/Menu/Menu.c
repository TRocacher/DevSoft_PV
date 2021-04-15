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

/******************************************************************************
*  PRIVATE Variables
*
******************************************************************************/

enum {M_Demarrage, M_MPPT_main, M_BattCharg_main};
enum {Entry, Do, Exit};
#define NbMenu 3

char Space[]="                    ";

struct MenuStatusTypedef {
	 int  ActualMenu;
  	 void (*MenuPtrFct[NbMenu])(void);
	 int ActualMenuState; // {Entry, Do, Exit}
	 float InputValue;
	 char NewInputValueFlag;
	 float ValueToPrint[10];
} MenuStatus;


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


// pour tester
#define Up 24
#define Down 25
#define Right 26
#define Left 27
char Touch;



/******************************************************************************
*  PRIVATE declaration

******************************************************************************/
void Fct_Demarrage(void);
void Fct_MPPT_main(void);
void Fct_BattCharg_main(void);




/******************************************************************************
*  PUBLIC FUNCTION

******************************************************************************/

void Start_Menu(void)
{
	/*************************************
	 * init attribut MenuStatus
	 *************************************/
	MenuStatus.ActualMenu=M_Demarrage;
	MenuStatus.ActualMenuState=Entry;
	MenuStatus.InputValue=12.5;
	MenuStatus.NewInputValueFlag=0;
	// chargement du tableau de pointeur de fonction
	MenuStatus.MenuPtrFct[M_Demarrage]=Fct_Demarrage;
	MenuStatus.MenuPtrFct[M_MPPT_main]=Fct_MPPT_main;
	MenuStatus.MenuPtrFct[M_BattCharg_main]=Fct_BattCharg_main;
	// valeurs à imprimer à 0.0
	for (int i=0 ;i<=9 ;i++)
	{
		MenuStatus.ValueToPrint[i]=0.0;
	}


}





/******************************************************************************
*  PRIVATE functions

******************************************************************************/

void MenuPrintNavInfoScreen(void)
{
	switch(Touch)
			{
			case Up:
				if 	(MenuScreenData.ActiveItem!=1)
				{
					// besoin de scroller vers le haut ?
					if (MenuScreenData.ActiveItem==MenuScreenData.FirstItemToPrint)
						MenuScreenData.FirstItemToPrint--;
					MenuScreenData.ActiveItem--;
				}
				break;
			case Down:
				if 	(MenuScreenData.ActiveItem!=MenuScreenData.NbItemsToPrint)
				{
					// besoin de scroller vers le bas ?
					if ((MenuScreenData.ActiveItem-MenuScreenData.FirstItemToPrint)==3)
						MenuScreenData.FirstItemToPrint++;
					MenuScreenData.ActiveItem++;
				}
				break;
			case Right:
				MenuStatus.ActualMenuState=Exit;
				break;
			case Left:
				MenuStatus.ActualMenuState=Exit;
				MenuScreenData.ActiveItem=255;
				break;
			}

	// TO DO
	// - afficher les 4 lignes : la première et les 3 autres à partir de l'item actif
	// mettre en surbrillance la ligne indiquée


}




void Fct_Demarrage(void)
{
	switch(MenuStatus.ActualMenuState)
	{
	case Entry:
		{
			MenuStatus.ActualMenuState=Do;
			MenuScreenData.ActiveItem=1;
			MenuScreenData.NbItemsToPrint=2;
			MenuScreenData.FirstItemToPrint=1;
			MenuScreenData.PtrLine[0]="Systeme eteint      ";
			MenuScreenData.PtrLine[2]="1> Demarrage...     ";
			MenuScreenData.PtrLine[2]="2> Parametres ...   ";
			MenuScreenData.PtrLine[3]=Space;


			MenuPrintNavInfoScreen();
			break;
		}
	case Do:
		{   // Test et action ds 4 flèches, affichage
			MenuPrintNavInfoScreen();
			break;
		}
	case Exit:
		{
			switch(MenuScreenData.ActiveItem)
			{
			case 1:
				{
					MenuStatus.ActualMenu=M_MPPT_main;
					break;
				}
			case 2:
				{
						MenuStatus.ActualMenu=M_BattCharg_main;
				}
			case 255:
				{
					MenuStatus.ActualMenu=M_Demarrage; // on bouge pas
				}

			MenuStatus.ActualMenuState=Entry;
			break;
			}

		}
	}
}


void Fct_MPPT_main(void){

}

void Fct_BattCharg_main(void){

}


void MenuProgress(void)
{
	MenuStatus.MenuPtrFct[MenuStatus.ActualMenu]();
}



/**
  * @brief
  */
