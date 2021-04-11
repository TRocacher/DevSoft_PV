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
git ref : 0db344a
Validation des 10 filtres d'ordre 2. Le Timer 6 pédale à 40µs, soit 25kHz de fréquence éch.
utilisation du DAC channel 1(conf HAL + use dans MyAnalog.c)
Test : Avec Cube, on définit scan sur 10 voies, la même. Les filtres sont testés les uns après les autres pour 10 Fc de 100 à 1000Hz
A chaque nouveau filtre, on garde l'ancien pour vérifier les tps d'exéc. Le test se fait à Fc, attente de -3dB

Résultat : Parfait. Tps d'exec total pour 10 filtres = 18µs, sans surprise

=================================================================================================================================
git ref : a0a2919 
Filtre d'ordre 1 validé pour le premier filtre, à faire pour les autres.
Validation : filtre rank 1, avec la sonde analog device mode Bode.
Tps d'execution : 1.4µs (400ns gagné sur le filtre d'ordre 2 soit 22% (pas mal de tps passé pour caster, pour mémoriser ... = tps fixe).
Compilation OK sans warning pour ordre 1 ou ordre 2

=================================================================================================================================
git ref : 15ea08a
Tous les filtres (les 10, ordre 1 et ordre 2) sont testés. Tout est validé.
Le projet est finalisé.
Resterait à faire un pdf...

=================================================================================================================================
git ref : V1.0
Ajout des fonction get

========================== MAIL COLLEGUES ====================
Je viens de finaliser un projet qui prend en main une grande partie de l'analogique sur STM32L476. Le développement c'est CubeIDE. J'ai pensé que cela pourrait vous intéresser et pourquoi pas vous être utile.

Ma motivation : jouer avec CubeIDE + Hal pour éventuellement mettre en suivant du FreeRTOS sur autre chose que le 103.

Déception : la partie analogique Hal est pourrie, prend 20 plombes pour faire un opération de base (start ADC...). Ce n'est pas acceptable dans plein d'applications de régulation rapide.

-> mon besoin est non seulement d'aller vite avec l'ADC mais aussi de mettre en place des filtres numériques en amont des régulations.

Mon idée a donc été de développer un module MyAnalog.c/.h qui vient dans un projet Cubeide et qui prend le relais de la conf cubemx, sur les aspects délicats ou Hal est à la rue (interruption, DMA). Le but est que le user ne s'occupe de RIEN au niveau analogique, il vient piocher à la volée jusqu'à dix valeurs (10 canaux) non filtrés, ou filtrés. Les interruptions DMA sont prioritaire sur tout.

On imagine alors un programme de haut niveau qui n'a même pas à lancer d'ADC : il suffit de venir lire la dernière valeur traitée. Tout est en masqué.


En gros je vous dis comment on y joue si ça vous intéresse :

Config HAL (vous avez une exemple dans le projet, mais si on part de 0) : Cubemx (lancement du .ioc)

- Préconfiguré pour la nucléo L476 (LED, BP user...)

- Horloge interne (elle est relativement précise et évite les configurations à la con des nucléo, bypass ou pas...), donc 80MHz au CPU et 20MHz pour l'adc (on pourrait aller moins vite).

- ADC triggé par un timer  dont on règle la périodicité à une valeur Te

-ADC en scan sur 10 voies maximum

- éventuellement DAC1 en conf de base (si on veut étudier les filtres à la mise au point)

Config MyAnalog.h

- choix de Te (période d'échantillonnage) = mettre la même que l'overlow timer !!!)

- filtrage 1 à 10 activé ou pas, calcul flottant (performant grâce à la FPU du M4)

-fréquence de coupure  réglable en Hz pour chaque filtre

- ordre 1 ou ordre 2 pour chacun des filtre (tps d'exécution 1.5µs pour ordre 1, 1.8µs pour ordre 2)

- possibilité d'associer ou non un callback à l'IT DMA

- une seule fonction d'init (deux en fait si on veut jouer avec un callback)

- un .h de config générale

- deux fonctions get (data filtrée ou non filtrée).


Voila pour la présentation. Si vous jouez avec, n'hésitez pas à me faire des retours si la mise en oeuvre est obscure.



