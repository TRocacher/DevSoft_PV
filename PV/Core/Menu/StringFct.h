/*
 * StrinFct.h
 *
 *  Created on: 26 avr. 2021
 *      Author: trocache
 */

#ifndef MENU_STRINGFCT_H_
#define MENU_STRINGFCT_H_


/**
  * @brief  Transforme un nombre float dans un format supposé en un suite
  * de digit avec la virgule au bon endroit
  * Le format final n'est pas un string, il tient dans un tableau de 7 bytes.
  *
  * @Note : Le user soit absulument faire attention au format 7 bytes afin de ne pas
  * produire des effets de bord.
  * Format final (exemple Frac = 2, DigitNumber = 5, valeur = 124.25 )
  *  |52.421x| du byte 0 au byte 6
  *
  * @Note 2: si DigitNbr > 6, plantage While 1
  * 		 si Frac > 3, plantage While(1)
  *
  * @param  float Value : la valeur à traiter
  * @param  char * DigitTab : l'adresse de la table de 7 bytes à écrire
  * @param  char DigitNbr : le nombre de digit à retranscrire (hors virgule)
  * @param  char Frac : le nombre de digit de la partie décimale
  * @retval None
  **/
char StringFct_Float2Str(float Value,char * DigitTab, char DigitNb, char Frac);

float StringFct_Str2Float(char * DigitTab, char DigitNb, char Frac);

/**
  * @brief  Convertit un float en un châine de caractères
  *
  * A REFAIRE.....LES COMMENTATRIES
  *
  * @Note : la châine de retour doit avoir au moins 8 bytes (pour caser le caractère Null)
  *
  * @param  float Value : la valeur à traiter
  * @param  char * DigitTab : l'adresse de la table de 7 bytes à écrire
  * @param  char DigitNbr : le nombre de digit à retranscrire (hors virgule)
  * @param  char Frac : le nombre de digit de la partie décimale
  * @retval None
  *
  * @Note : Exemple  avec DigitNumber = 5
  * |52.421x| du byte 0 au byte 6 donne |'1' 2' '4' '.' '2' '5' 0| du byte 0 au byte 6
  * (le dernier n'est pas utilisé)
  *
  **/
void StringFct_Digit2Str(char * DigitTab, char * Str, char DigitNb);

#endif /* MENU_STRINGFCT_H_ */
