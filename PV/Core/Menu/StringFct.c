/*
 * StringFct.c
 *
 *  Created on: 26 avr. 2021
 *      Author: trocache
 */

#include "StringFct.h"


int Val; // à mettre en local après debug
char Tampon[7];
void StringFct_Float2Str(float Value,char * DigitTab, char DigitNb, char Frac)
{

	int i;
	// erase Tab
	for (i=0;i<7;i++)
	{
		*(DigitTab+i)=0;
	}

	switch (Frac)
	{
	case 0:Val=Value;break;
	case 1:Val=10.0*Value;break;
	case 2:Val=100.0*Value;break;
	case 3:Val=1000.0*Value;break;
	default : while(1); // erreur fatale si Frac>3
	}

	switch (DigitNb)
	{
	case 6:*(Tampon+5)=Val/100000+0x30;Val=Val%100000;
	case 5:*(Tampon+4)=Val/10000+0x30;Val=Val%10000;
	case 4:*(Tampon+3)=Val/1000+0x30;Val=Val%1000;
	case 3:*(Tampon+2)=Val/100+0x30;Val=Val%100;
	case 2:*(Tampon+1)=Val/10+0x30;Val=Val%10;
	case 1:*(Tampon)=Val+0x30;break;
	// resultat dans la chaîne (7 car) :
	// |x 6 5 4 3 2 1| avec 6 les centtaines de mille, 5 les dizaines ...etc..

	default : while(1);// erreur fatale si NbDigit>6
	}


	for (i=DigitNb ; i>=(Frac+1); i-- )
	{
		*(Tampon+i)=*(Tampon+i-1);
	}
	*(Tampon+Frac)='.';

	// remise dans l'ordre digit de poids fort en 0 (début de châine)
	for (i=0 ; i<=DigitNb; i++ )
	{
		*(DigitTab+i)=*(Tampon+DigitNb-i);
	}
	*(DigitTab+DigitNb+1)=0; // caractère nul

}



//void StringFct_Digit2Str(char * DigitTab, char * Str)
