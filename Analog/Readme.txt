Comment copier un projet proprement ?
- préparer un rep d'accueil 
- créer un nouveau proj à partir d'un .ioc (cube)
- choisir le rep d'accueil + nom du projet
- ajouter les SRC avec les lignes de codes personnelles.
- tester le clone pour voir si OK

=================================================================================================================================
DESCRIPTION GENERALE:
Développement du module MyAnalog.c/.h initié dans C:\Users\trocache\Documents\Thierry\2013_2016\Formation_Perso\Cube_IDE\Dev_ADC
et repris ici avec suivi ici avec git

Le principe visé est une configuration avec HAL pour l'essentiel, puis le module MyAnalog fait le reste.
Objectif : disposer d'un système autonome d'acquisition analogique jusqu'à 10 canaux, filtré ou pas.
Chaque filtre est paramétrable

De cette manière, un utilisateur pourra venir chercher à la volée des grandeurs analogiques, depuis un système d'IT totalement indépendant
ou même via du freeRTOS. 
MyAnalog doit permettre une utilisation minimale du CPU. Son système d'IT est au maximum (0). Prioritaire sur tout.
NB : un mode debug permet d'indiquer des pulses au moment des acquisitions analog (sur Led2 Nucléo = PA5). A n'utiliser qu'en débug


=================================================================================================================================
git ref : 6f66fc5 
Service fourni actuellement :
- DMA sur n voies en mode scan rafales
Exemple proposé :
HAL = TIM6 en TRGO Event overflow = 4µs; ADC en trigg sur TIM6, Scan, pas d'IT ni de DMA, 2 voies 5 et 6 (PA0 et PA1)

=================================================================================================================================
git ref : 9fbe058
DMA en IT OK, FPU validée !!
Test 1 : filtre 0 activé float: voie 0 filtrée connectée au 3.3V
Resultat : la sortie filtrée se fige à 4088, oscille très peu, alors que l'entrée oui.
Tps d'exec mesurée au scope (entrée / sortie IT DMA) = 1.8µs

Codage filtre 1 en entier (32bits * 32bits en 64bits)
Test 2 : idem, filtre comme la version float.
Tps d'exec très décevant = 2.2µs. Le code ASM est curieux, utilise 2 fois MUL au lieu d'une Mul longue.
-> Validation de la FPU. Pas de concurrence en langage C. Si on voulait améiorer les choses, il faudrait
coder les filtres en ASM format fractionnaire...ne vaut pas le coup du tout.

=================================================================================================================================
git ref : 28501ce
Validation du filtre 0 en float, à fc = 500Hz, fe = 250kHz.
utilisation du DAC channel 1(conf HAL + use dans MyAnalog.c)
Test : GBF avec Analog discovery + scope
Résultat : PARFAIT, Bode nickel, rep indicielle aussi, repliement de spectre ...

=================================================================================================================================
git ref : 
Validation des 10 filtres d'ordre 2. Le Timer 6 pédale à 40µs, soit 25kHz de fréquence éch.
utilisation du DAC channel 1(conf HAL + use dans MyAnalog.c)
Test : Avec Cube, on définit scan sur 10 voies, la même. Les filtres sont testés les uns après les autres pour 10 Fc de 100 à 1000Hz
A chaque nouveau filtre, on garde l'ancien pour vérifier les tps d'exéc. Le test se fait à Fc, attente de -3dB

Résultat : Parfait. Tps d'exec total pour 10 filtres = 18µs, sans surprise
