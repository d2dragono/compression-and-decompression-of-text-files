/*

    Programme réalisé par François PICARD (22003512)
    compression.c fonctionne sur tous les .txt et peut les compresser avec l'option -c et les décompresser avec l'option -d

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Structure d'un noeud de l'arbre de Huffman
typedef struct Arbre
{
    char nom[257]; //Nom du noeud. Comme il y a 256 caractère dans le code ascii, le noeud peut au maximum avoir un nom de taille 256. On ajoute une place pour le \0 de fin de chaine
    struct Arbre *SAG; //Sous arbre gauche
    struct Arbre *SAD; //Sous arbre droit
    double probabilite; //Probabilité du noeud
    char CODE[32]; //Codage du noeud
} Arbre;



//FONCTIONS POUR LA CRÉATION DE L'ARBRE DE HUFFMANN

    //Fonction qui retourne la taille d'un tableau de noeud
    //Utilisé pour savoir quand l'ensemble est de taille 1 et que donc l'arbre de Huffman est terminé et pour l'insertion décroissante
    int sizeOfTab(Arbre *tabNoeud[])
    {
        int size = 0;
        while(tabNoeud[size] != NULL && size < 256) //256 est la taille maximal du tableau (si les 256 caractères sont présents)
        {
            size++;
        }

        return size;
    }



    //Fonction pour ajouter un élément dans le tableau de manière décroissante
    void ajouterDecroissant(Arbre *noeud, Arbre *tabNoeud[])
    {
        int i = 0; //Compteur
        Arbre *noeudTemp = NULL; //Noeud temporaire

        //Si le tableau est vide
        if(tabNoeud[0] == NULL)
        {
            //Alors le premier emplacement du tableau prend le noeud
            tabNoeud[0] = noeud;
        }
        else
        {
            //Sinon, tant que l'on atteind pas la fin de tableau
            while(i < sizeOfTab(tabNoeud))
            {
                //On regarde si la probabilité de l'élément est inférieur à la probabilité du noeud à ajouter
                if(tabNoeud[i]->probabilite < noeud->probabilite)
                {
                    //Si oui, on décalle tout le tableau de 1 vers la droite de la fin jusqu'à i et on insère le noeud à l'indice i
                    for(int j = sizeOfTab(tabNoeud); j > i; j--)
                    {
                        tabNoeud[j] = tabNoeud[j-1];
                    }
                    tabNoeud[i] = noeud;
                    break;
                }
                else
                {
                    i++;
                }  
            }

            //Si on a atteint le bout du tableau
            if(i == sizeOfTab(tabNoeud))
            {
                //Alors on ajoute l'élément à la fin du tableau, il a la plus petite probabilité
                tabNoeud[i] = noeud;
            }
        }
    }



    //Fonction pour créer le noeud d'un arbre
    Arbre* createNode(char *nomNoeud, double probabilite)
    {
        //On alloue la mémoire pour le noeud
        Arbre *noeud = (Arbre*)malloc(sizeof(Arbre));
        strcpy(noeud->nom, nomNoeud);
        noeud->SAD = NULL;
        noeud->SAG = NULL;
        noeud->probabilite = probabilite;
        return noeud;
    }



    //Fonction qui ajoute le codage de nos caractères dans les noeuds respectifs
    Arbre* codage(Arbre *noeud, char CODE[])
    {
        strcpy(noeud->CODE, CODE); //Le noeud prend le code atteint
        //Comme chaque noeud possède forcément un fils droit et un fils gauche (sauf feuille) on peut vérifier si un des fils est vide pour déterminé si le noeud est une feuille
        if(noeud->SAD != NULL)
        {
            //Comme le passage de tableau est par adresse, il est important de créer une version temporaire pour pouvoir manipuler le code à ce niveau de la récursivité
            char CODETemp[32]; //Code temporaire
            strcpy(CODETemp, noeud->CODE); //On donne au code temporaire le code du noeud actuel
            //Sinon, c'est un noeud. La probabilité la plus haute étant toujours à gauche, on va a gauche avant la droite pour avoir un affichage en probabilité décroissante
            //Chaine temporaire qui stock le code a envoyer (pour ne pas modifier directement le code associé au noeud)
            noeud->SAG = codage(noeud->SAG, strcat(CODETemp, "1")); //A droite, on ajoute 1 à la fin du code
            strcpy(CODETemp, noeud->CODE); //On donne au code temporaire le code du noeud actuel en remontant de la récursivité
            noeud->SAD = codage(noeud->SAD, strcat(CODETemp, "0")); //A gauche, on ajoute 0 à la fin du code
        }

        return noeud;
    }



    //À partir de la source S des probabilités des chaques caractères, on construit l'arbre de Huffman 
    Arbre* arbreHuffman(Arbre *S[])
    {

        //Un noeud temporaire (celui qui résulte de la fusion)
        Arbre *noeudTemp = NULL;
        Arbre *stockNoeud = NULL; //Pour stocker des noeuds
        char nomNoeud[257]; //Nom du noeud temporaire
        double probaNoeud; //Probabilité du noeud

        //S est l'ensemble des caractères triés dans l'ordre décroissant en fonction de la probabilité
        //Ainsi, tant que la taille de S est supérieur à 1, on construit l'arbre de Huffman. Lorsque S sera de taille 1, le seul noeud que contiendra S sera la racine de l'arbre de Huffman
        while(sizeOfTab(S) > 1)
        {
            //Etape 1 : on fusionne les deux derniers éléments, qui iront dans un noeud temporaire
            strcpy(nomNoeud, S[sizeOfTab(S)-1]->nom); //Création du nom du noeud
            strcat(nomNoeud, S[sizeOfTab(S)-2]->nom);
            probaNoeud = S[sizeOfTab(S)-1]->probabilite + S[sizeOfTab(S)-2]->probabilite; //Création de la probabilité du noeud
            noeudTemp = createNode(nomNoeud, probaNoeud); //Création du noeud

            //Etape 2 : le nouveau noeud prend comme fils gauche l'avant dernier élément de la liste et comme fils droit le dernier élément de la liste
            noeudTemp->SAD = S[sizeOfTab(S)-1]; //La probabilité la plus faible ira toujours à droite
            noeudTemp->SAG = S[sizeOfTab(S)-2];

            //Etape 3 : suppression des deux derniers éléments du tableau
            S[sizeOfTab(S)-1] = NULL; //La taille perd un, ainsi, pour supprimer le deuxième élément, on refait -1
            S[sizeOfTab(S)-1] = NULL; 

            //Etape 4 : Ajout du nouveau noeud résultant de la fision dans le tableau (insertion de manière à garder la probabilité décroissante des noeuds)
            ajouterDecroissant(noeudTemp, S);
        }
        
        //Quand on arrive là, on possède l'arbre car il n'y a plus qu'un élément dans le tableau. On associe ensuite le codage à chaque élément
        return codage(S[0], ""); //Le premier noeud ne possède pas de code (nous n'avons ni été à droite, ni à gauche).
    }

//FIN FONCTIONS POUR LA CRÉATION DE L'ARBRE DE HUFFMANN



//FONCTIONS POUR ÉCRIRE LA VERSION COMPRESSÉ

    //Fonction pour écrire la signature dans le fichier (uniquement la codification des caractères)
    void ecritureSignature(FILE *COMPRESSION, Arbre *racine)
    {
        //Comme chaque caractère est une feuille, si il n'y a pas de fils droit, alors c'est une feuille
        if(racine->SAG == NULL)
        {
            //Ajout de la ligne dans la signature
            fputs(racine->nom, COMPRESSION); //Écriture du caractère
            fputs(":", COMPRESSION); //Obligatoire de mettre un séparateur (ici ':') car sinon la lecture de certains caractères serait impossible
            fputs(racine->CODE, COMPRESSION); //Ajout du code
            fputs("\n", COMPRESSION); //Ajout d'un retour à la ligne
        }
        else
        {
            //Si ce n'est pas une feuille, on va vers le fils gauche puis le fils droit
            ecritureSignature(COMPRESSION, racine->SAG);
            ecritureSignature(COMPRESSION, racine->SAD);
        }
    }



    //Fonction qui écrit la version compressé du fichier
    void ecritureCompression(FILE* ORIGINAL, FILE* COMPRESSION, char nomFichierCompression[])
    {
        /* PRINCIPE UTILISÉ POUR LA COMPRESSION :

            -> On va lire caractère par caractère
            -> On écrira le code associé au caractère
            -> On aura une variable qui ira de 1 à 8, nous indiquant à quel bit nous sommes dans l'octet.
            -> Ainsi, quand on dépasse 8, on écrira la suite sur l'octet suivant.

            -> Un caractère pouvant avoir un code valant 0 ou composé exclusivement de 1 ainsi que la possibilité dans le dernier octet de n'utiliser que 3 bits nous oblige à indique en tête de signature le nombre de bits inutiles dans le dernier octet. On écrira en tête de signature :

                -> Le nombre d'octet de la version compressé
                -> Le nombre de bits utilisé dans le dernier octet

            Ainsi, on saura quand on aura atteint le dernier octet et on saura également le nombre de bits qu'il ne faudra pas lire dans ce dernier octet
        */

        //VARIABLES
        //Les deux variables suivantes seront ajoutés avant la signature
        int bit = 1; //Indique à quel bit nous sommes dans l'octet actuel
        int nbrOctets = 1; //Indique à quel octet nous sommes actuellement

        //Variables utilisées pour la compression
        char ligneLu[128]; //Ligne lu dans la signature
        char CODE[32]; //Code lu
        char intToStr[16]; //Conversion entier vers chaine

        //L'octet dans lequel écrire le code
        unsigned char octet = 0; //Un octet

        //Un bit (00000001)
        unsigned char unBit = 1;

        int i; //Compteur
        char caractereLu; //Caractère qui sera lu dans le fichier d'origine
        int flag; //Tant que flag est vrai, on lira la signature.

        //Début
        while((caractereLu = fgetc(ORIGINAL)) != EOF)
        {
            //ÉTAPE 1 : On recherche la ligne dans la signature à laquel correspond le caractère lu
            rewind(COMPRESSION); //On retourne au début du fichier
            //On est obligé de lire caractère par caractère et non pas ligne par ligne à cause des caractères pouvant être à l'origine d'un retour à la ligne (code ascii 10 et 13)
            flag = 1;

            //On réinitialise CODE
            for(int j = 0; j < 32; j++)
            {
                CODE[j] = '\0';
            }

            while(flag)
            {
                //On lit une ligne de la signature
                fgets(ligneLu, 128, COMPRESSION);
                //On regarde si le premier élément correspond au caractère lu dans le fichier original
                if(ligneLu[0] == caractereLu)
                {
                    //Si oui, on lit le code
                    //On vérifie si le caractère suivant est différent de : (pour le cas où c'est un caractère qui fait un retour à la ligne)
                    if(ligneLu[1] != ':')
                    {
                        //Le code est sur la ligne suivante, on lit donc une ligne de plus
                        fgets(ligneLu, 128, COMPRESSION);
                        //Lecture du code jusqu'à atteindre \n
                        i = 1;
                        while(ligneLu[i] != '\n')
                        {
                            CODE[i-1] = ligneLu[i];
                            i++;
                        }
                        CODE[i-1] = '\0';
                    }
                    else
                    {
                        //Sinon, lecture du code jusqu'à atteindre \n
                        i = 2;
                        while(ligneLu[i] != '\n')
                        {
                            CODE[i-2] = ligneLu[i];
                            i++;
                        }
                        CODE[i-2] = '\0';
                    }

                    //On veut quitter la boucle
                    flag = 0;
                }
            }

            //ÉTAPE 2 : Maintenant que l'on a récupéré le code associé, on l'ajoute dans la version compressé
            //On retourne à l'emplacement initial
            fseek(COMPRESSION, 0, SEEK_END); //On se remet au dernier octet du fichier

            //Maintenant que l'on est à la bonne place, on ajoute le code
            /* Le principe pour ajouter le code est le suivant :

                Imaginons que nous ayons 00000000 de base.
                Si on lit 1, on ajoute 1 (on a donc 00000001).
                Si on lit 0, on ne fait rien.
                Lorsque l'on ajoute un nouveau chiffre du code, on fait un décalage de 1 bit vers la gauche sauf lorsque c'est le premier bit de l'octet dans lequel on écrit
                Quand on à écrit 8 bits, on ajoute l'octet dans le fichier puis on travail sur un nouvel octet

            */

            i = 0;
            while(CODE[i] != '\0')
            {
                //Si on est au premier bit, on ne multiplie pas par 2
                if(bit == 1)
                {
                    //On ne multiplie pas par 2
                    bit++;
                }
                else
                {
                    //Multiplication par 2
                    octet = octet << 1;
                    bit++;
                }

                //Si CODE[i] vaut 1, on l'ajoute. Sinon, on ne fait rien (comme un octet a par défaut ses bits à 0)
                if(CODE[i] == '1')
                {
                    octet = octet | unBit; //OU LOGIQUE avec l'octet actuel et 00000001
                }

                //Si bit vaut 9, cela veut dire que l'on a écrit 8 bits. Ainsi, on ajoute l'octet dans le fichier
                if(bit == 9)
                {
                    //Écriture de l'octet dans le fichier
                    fwrite(&octet, sizeof(octet), 1, COMPRESSION);
                    bit = 1; //On repasse au premier bit

                    //Octet reprend 00000000
                    octet = 0;
                    nbrOctets++;
                }

                i++;
            }
        }

        //On décale le dernier octet du nombre de bits restant (sauf si on tombe juste)
        if(bit != 1)
        {
            octet = octet << (9 - bit); //9 fois et non pas 8 fois car on incrémente toujours le bit de 1, on vérifie si il vaut 9 PUIS on fait le décalage éventuel
            fwrite(&octet, sizeof(octet), 1, COMPRESSION); //Écriture du dernier octet
        }

        //On ajoute le nombre d'octet (pour savoir quand on atteint le dernier octet) ainsi que le nombre de bits dans le dernier octet (ainsi, on est capable de correctement lire le dernier octet)
        //Pour faire cela, il est impossible d'ajouter un ligne en plein milieu du fichier, on doit le réécrire complètement mais avec les nouvelles informations
        //La première ligne du fichier sera donc le nombre d'octets
        //La seconde ligne du fichier sera le nombre de bits inutiles dans le dernier octet
        //On recopie l'intégralité du fichier dans un fichier temporaire, mais on y ajoute les nouvelles informations avant
        FILE* TEMP = fopen("COMPRESSION_TEMP", "w+");
        rewind(COMPRESSION); //On retourne au début du fichier pour le réécrire
        sprintf(intToStr, "%d", nbrOctets); //Nombre d'octet
        strcpy(ligneLu, intToStr);
        strcat(ligneLu, "\n");
        sprintf(intToStr, "%d", (9 - bit)); //Nombre de bits inutiles dans le dernier octet
        strcat(ligneLu, intToStr);
        fputs(ligneLu, TEMP);
        fputc('\n', TEMP);

        //On commence par réécrire la signature
        flag = 1; //La signature et le fichier compressé sont délimité par les deux caractère '-\n', tant qu'on ne lit pas cette ligne, on continue.
        while(flag)
        {
            fgets(ligneLu, 127, COMPRESSION);
            if(strcmp(ligneLu, "-\n") == 0)
            {
                //On écrit le délimiteur, puis on sais qu'après se sont les octets du fichier compressé
                flag = 0;
            }
            fputs(ligneLu, TEMP);
        }

        //On connais le nombre d'octet à lire et à mettre dans le fichier
        for(int i = 0; i < nbrOctets; i++)
        {
            octet = fgetc(COMPRESSION);
            fwrite(&octet, sizeof(octet), 1, TEMP);
        }

        //On supprime le .huff et on renomme le fichier temporaire
        fclose(COMPRESSION);
        remove(nomFichierCompression);
        rename("COMPRESSION_TEMP", nomFichierCompression);

        //Fermeture du fichier temporaire (qui est maintenant le vrai fichier)
        fclose(TEMP);
    }



    //Fonction qui assure la compression du fichier
    void compression(char nomFichierOrigine[], char nomFichierCompression[], Arbre *racine)
    {
        /* La première étape va être d'écrire le codage dans le fichier compressé pour pouvoir le décoder par la suite. On écrire le codage de la manière suivante :

            -> Une ligne par caractère, avec le caractère , ':' et le code juste derrière
            -> Une fois tout les caractères indiqué, un - et un retour à la ligne directement, indiquant le début du fichier compressé (obligé de faire tenir ça sur deux caractères pour éviter des problèmes)

        */

        //On commence par ouvrir le fichier dans lequel écrire la "signature"
        FILE *COMPRESSION = fopen(nomFichierCompression, "wb+"); //Lecture écriture pour pouvoir écrire le fichier compressé et lire la signature

        //On va maintenant écrire la signature dans le fichier
        ecritureSignature(COMPRESSION, racine);
        //On ajoute la ligne séparatrice de la signature et du fichier compressé
        fputs("-\n", COMPRESSION); //Si on lit le caractère - et que derrière il n'y a pas ':' (pour indiquer que le code se trouve juste derrière) c'est que c'est la fin de la signature et que la suite correspond au fichier compressé

        //Maintenant que la signature est écrite dans le fichier, il n'y a plus qu'a écrire la version compressé du fichier
        FILE *ORIGINAL = fopen(nomFichierOrigine, "r");

        ecritureCompression(ORIGINAL, COMPRESSION, nomFichierCompression);

        //Fermeture des deux fichiers
        fclose(COMPRESSION);
        fclose(ORIGINAL);
    }

//FIN FONCTIONS POUR LE COMPRESSION DU FICHIER



//FONCTIONS POUR LA DÉCOMPRESSION DU FICHIER

    //Décompression (à partir du fichier compressé uniquement)
    int decompression(char nomFichier[], char nomFichierCompression[])
    {
        //Cette fonction permet de décompresser un fichier compressé via la méthode de HUFFMAN
        /* Voici comment nous procéderons : 

            -> Lecture du nombre d'octet dans la compression et du nombre de bits inutiles dans le dernier octet (ces informations sont présentes dans les deux premières lignes de la signature)
            -> Retranscription des octets sous forme de caractère dans le fichier original 

        */

        //Variables
        unsigned char octet; //L'octet qui sera lu
        unsigned char octetLecture; //Un octet qui sert pour lire un bit
        char ligneLu[128]; //Une ligne qui sera lu
        int i, j, decalage; //Compteurs
        int flag = 1;
        long adresse; //Permet de revenir à l'octet où nous sommes après être aller chercher le code dans la signature
        char CODE[32] = "\0"; //Code lu
        char CODETemp[32]; //Un code temporaire
        int bit; //Indique à quel bit nous sommes dans l'octet
        char caractereDeLaLigne; //Caractère dont on lit le code dans la signature




        //On ouvre le fichier pour voir si il existe
        FILE *COMPRESSION = fopen(nomFichierCompression, "r");

        if(COMPRESSION == NULL)
        {
            //La version compressé du fichier passé en argument n'existe pas
            return EXIT_FAILURE;
        }

        //On ouvre le fichier qui stockera la décompression
        FILE *DECOMPRESSION = fopen(nomFichier, "w");




        //Lecture de la première et seconde ligne pour initialisation des variables
        //Nombre d'octet
        fgets(ligneLu, 127, COMPRESSION);
        int nbrOctet = atoi(ligneLu);

        //Nombre de bits inutiles
        fgets(ligneLu, 127, COMPRESSION);
        int bitsInutiles = atoi(ligneLu);

        //On cherche maintenant à se placer au début de la compression, on passe donc la signature
        while(flag)
        {
            fgets(ligneLu, 127, COMPRESSION);
            if(strcmp(ligneLu, "-\n") == 0)
            {
                //On a atteint la délimitation entre la signature et la compression du fichier
                flag = 0;
            }
        }

        //Il va maintenant être question de lire les octets et de chercher un code semblable
        /* MÉTHODE :

            -> Lorsque l'on lit un bit dans l'octet, on lit un chiffre du code de chaque caractère.
            -> On prend le premier caractère tel que les bits lu correspondent au code lu
            -> On écrit le caractère dans le fichier décompressé puis on continue la lecture du fichier compressé 

        */

        //Début de la conversion
        for(i = 0; i < nbrOctet; i++)
        {
            //On regarde si on est au dernier octet
            if(i == nbrOctet - 1)
            {
                //Si oui, on ne lira que le nombre de bits utiles
                bit = bitsInutiles + 1; //Passe directement le nombre de bits inutiles
            }
            else
            {
                bit = 1; //On passe au premier bit de l'octet
            }

            //Lecture de l'octet suivant
            octet = fgetc(COMPRESSION);

            //On ajoute bit par bit jusqu'a avoir un code correspondant
            while(bit < 9)
            {
                //On lit le bit correspondant au numéro du bit
                octetLecture = octet;
                octet = octet << 1; //Décalage de 1 vers la gauche
                octetLecture = octetLecture >> 7; //Décalage de 7 bits vers la droite (vaut 00000001 OU 00000000)
                if(octetLecture == 1)
                {
                    //On ajoute 1 au code
                    strcat(CODE, "1");
                }
                else
                {
                    //On ajoute 0 au code
                    strcat(CODE, "0");
                }

                
                //On regarde si le code actuel et égal au code d'un des caractère
                adresse = ftell(COMPRESSION); //On sauvegarde la position dans le fichier
                rewind(COMPRESSION); //On retourne au début du fichier
                fgets(ligneLu, 128, COMPRESSION); //On passe la première ligne (nombre d'octet)
                fgets(ligneLu, 128, COMPRESSION); //On passe la deuxième ligne (nombre de bits inutiles)

                //On récupère maintenant ligne par ligne le code associé à chaque caractère et on regarde si il vaut le code actuellement lu
                flag = 1;
                while(flag)
                {
                    fgets(ligneLu, 128, COMPRESSION); //Lecture de la ligne
                    if(strcmp(ligneLu, "-\n") == 0)
                    {
                        //Alors on a atteint le bout de la signature, aucun code ne correspond au code actuellement présent dans CODE
                        flag = 0;
                    }
                    else
                    {
                        caractereDeLaLigne = ligneLu[0]; //Caractère associé au code qui va suivre
                        //On regarde si le code lu dans la ligne correspond au code présent dans CODE
                        //On commence par lire le caractère (au cas ou c'est un caractère spécial)
                        if(ligneLu[1] != ':')
                        {
                            //C'est un caractère qui nous ramène à la ligne suivante, le code est donc présent à la ligne juste après
                            fgets(ligneLu, 128, COMPRESSION);
                            j = 1; //Début du code
                            decalage = 1;
                        }
                        else
                        {
                            j = 2; 
                            decalage = 2;
                        }

                        //On commence par écrire le code de la ligne dans une variable temporaire
                        while(ligneLu[j] != '\n')
                        {
                            CODETemp[j - decalage] = ligneLu[j];
                            j++;
                        }

                        //On compare le code temporaire au code actuellement lu dans la compression
                        if(strcmp(CODE,CODETemp) == 0)
                        {
                            //Alors c'est le bon code, on écrit le caractère associé au code qui viens d'être lu dans le fichier
                            fputc(caractereDeLaLigne, DECOMPRESSION);

                            //On réinitialise CODE
                            for(j = 0; j < 32; j++)
                            {
                                CODE[j] = '\0';
                            }
                        }

                        //On réinitialise CODETemp pour la prochaine lecture
                        for(j = 0; j < 32; j++)
                        {
                            CODETemp[j] = '\0';
                        }
                    }
                }

                //On retourne à l'emplacement où l'on était
                fseek(COMPRESSION, adresse, SEEK_SET);
                //On passe au bit suivant
                bit++;
            }
        }

        fclose(DECOMPRESSION);
        fclose(COMPRESSION);

        //Suppression du fichier compressé
        remove(nomFichierCompression);  

        return EXIT_SUCCESS;  
    }

//FIN FONCTIONS POUR LA DÉCOMPRESSION DU FICHIER



//Fonction principale
int main(int argc, char* argv[])
{

    //VARIABLES
    //Variable du fichier à ouvrir
    char nomFichier[128];
    char nomFichierCompression[128]; //Nom du fichier qui contiendra la version compressé
    FILE *ID = NULL;

    //Données sur le fichier
    int occChar[256] = {0}; //Occurence de chaque caractère
    int nbrChar = 0; //Nombre de caractère dans le fichier

    //Pour la lecture du fichier
    char caractereLu;

    //Noeuds
    Arbre *noeud = NULL;
    char nomNoeud[257];
    Arbre *tabNoeud[256] = {NULL}; //Maximum 256 caractères différents (au cas où il sont tous présents)

    //Compteurs
    int i;

    //Si l'utilisateur demande une compression
    if(strcmp(argv[1], "-c") == 0)
    {
        //Ouverture du fichier (en lecture uniquement)
        ID = fopen(argv[2], "r");

        if (ID == NULL)
        {
            //Le fichier ne s'est pas ouvert correctement
            system("clear");
            printf("\n    Le fichier %s n'existe pas, fin du programme\n\n", nomFichier);
            return EXIT_FAILURE;
        }

        //Lecture de l'occurence de chaque caractère
        while((caractereLu = fgetc(ID)) != EOF)
        {
            occChar[caractereLu]++;
            nbrChar++;
        }

        //Fermeture du fichier une fois l'occurence des caractères obtenue
        fclose(ID);

        //Obtention de la probabilité de chaque caractère et ajout dans une liste dans l'ordre décroissant
        for(int i = 0; i<256; i++)
        {
            //Si l'occurence est à 0, on ne l'ajoute pas dans la liste
            if(occChar[i] == 0)
            {
                continue;
            }
            else
            {
                //Sinon, on l'ajoute dans la liste à la bonne place (en fonction de sa probabilité, de manière à avoir une liste de probabilité décroissante)
                //On utilise pour ça une fonction car on va devoir trier la liste tout le long (lors de la fusion des deux dernières probabilités les plus improbables, on doit ensuite replacer le nouveau noeud dans l'ensemble)

                //On commence par initialiser le noeud
                nomNoeud[0] = i; //Le nom du noeud sera le caractère correspondant au code ASCII de i
                noeud = createNode(nomNoeud, ((double)occChar[i])/nbrChar);

                //Ajout dans le tableau (de manière décroissante)
                ajouterDecroissant(noeud, tabNoeud);
            }
        }

        //Création de l'arbre de HUFFMAN
        noeud = arbreHuffman(tabNoeud); //Le noeud prend la racine de l'arbre de Huffman

        //Maintenant que nous avons l'arbre de huffman avec le code de chaque caractère, on va créer le fichier résultant de la compression
        strcpy(nomFichierCompression, argv[2]);
        strcat(nomFichierCompression, ".huff");

        compression(argv[2], nomFichierCompression, noeud);
        //Suppression de la version originale non compressé
        remove(argv[2]);
    }
    else
    {
        //L'utilisateur demande une décompression
        if(strcmp(argv[1], "-d") == 0)
        {
            strcpy(nomFichierCompression, argv[2]);
            strcat(nomFichierCompression, ".huff");

            if(decompression(argv[2], nomFichierCompression))
            {
                //Si le fichier correspondant à nomFichierCompression n'existe pas
                printf("\n    Le fichier %s n'existe pas, fin du programme\n\n", nomFichierCompression);
                return EXIT_FAILURE;
            }

        }
        else
        {
            //Si l'option n'est pas -c ou -d
            system("clear");
            printf("\n    Option invalide : -c pour la compression et -d pour la décompression\n\n");
            return EXIT_FAILURE;
        }
    }

    //Si on arrive ici, tout s'est bien déroulé
    return EXIT_SUCCESS;
}