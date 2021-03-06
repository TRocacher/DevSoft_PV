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
	 char NewValueToPrint[10];
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
enum {M_Demarrage, M_DemarrageMPPT, M_DemarrageBattChg, M_Param};
void MenuFct_Demarrage(void);
void MenuFct_DemarrageMPPT(void);
void MenuFct_DemarrageBattChg(void);
void MenuFct_Param(void);



/******************************************************************************
*  PUBLIC FUNCTION

******************************************************************************/

void Init_Menu(void)
{
	/*************************************
	 * init attribut MenuStatus
	 *************************************/
	MenuStatus.ActualMenu=M_Demarrage;
	MenuStatus.ActualMenuState=Entry;
	MenuStatus.InputValue=12.5;
	MenuStatus.NewInputValueFlag=0;
	// chargement du tableau de pointeur de fonction
	MenuStatus.MenuPtrFct[M_Demarrage]=MenuFct_Demarrage;
	MenuStatus.MenuPtrFct[M_DemarrageMPPT]=MenuFct_DemarrageMPPT;
	MenuStatus.MenuPtrFct[M_DemarrageBattChg]=MenuFct_DemarrageBattChg;
	MenuStatus.MenuPtrFct[M_Param]=MenuFct_Param;
	// valeurs à imprimer à 0.0
	for (int i=0 ;i<=9 ;i++)
	{
		MenuStatus.ValueToPrint[i]=0.0;
		MenuStatus.NewValueToPrint[i]=0;
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
					if (MenuScreenData.ActiveItem==MenuScreenData.FirstItemToPrint) MenuScreenData.FirstItemToPrint--;
					MenuScreenData.ActiveItem--;
				}
				break;
			case Down:
				if 	(MenuScreenData.ActiveItem!=MenuScreenData.NbItemsToPrint)
				{
					// besoin de scroller vers le bas ?
					if ((MenuScreenData.ActiveItem-MenuScreenData.FirstItemToPrint)==3)	MenuScreenData.FirstItemToPrint++;
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


/*==========================================================================================
 *
 * 		LES MENNU DECRITS A TRAVERS LES "MENU FUNCTIONS"
 *
 *
 ==========================================================================================*/

/*      **********************
 * 		*Systeme eteint      *
 * 		*1> Demarrage MPPT	 *
 * 		*2> Demarrage Bat Ch *
 * 		*3> Parametres >	 *
 * 		**********************
 */

void MenuFct_Demarrage(void)
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
			MenuScreenData.PtrLine[1]="1> Demarrage...     ";
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
					MenuStatus.ActualMenu=M_DemarrageMPPT;
					break;
				}
			case 2:
				{
					MenuStatus.ActualMenu=M_DemarrageBattChg;
				}
			case 3:
				{
					MenuStatus.ActualMenu=M_Param;
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



/*      **********************
 * 		*Demarrage	         *
 * 		*1> MPPT  			 *
 * 		*2> Batt Charger	 *
 * 		*					 *
 * 		**********************
 */

void MenuFct_DemarrageMPPT(void)
{


}
void MenuFct_DemarrageBattChg(void)
{

}
void MenuFct_Param(void)
{

}


void MenuProgress(void)
{
	MenuStatus.MenuPtrFct[MenuStatus.ActualMenu]();
}



/**
  * @brief
  */
