/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: Gabriel
 *
 * Created on 4 octobre 2016, 22:16
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct num { int negatif; struct cell *chiffres; };
struct cell { char chiffre; struct cell *suivant; int marked;};

struct variable { char lettre; int valeur;}; 
/*utilisable doit etre entre 0 et 1 et est initialisee a 0. Elle devient 1
 * lorsque la variable est utilisable.*/

/*
 * Procedure principale
 */
//struct cell* cellTete = NULL;

struct cell* selectNumber(struct cell*);
struct cell* addition(struct cell*, struct cell*);

int main(int argc, char** argv) 
{
    struct variable varArray[26]; //array where you set variables
    struct cell* cellTete = NULL; //premiere cellule de la linked list
    struct cell* cellActuelle = NULL; // cellule actuelle lors du parcours
    char checkfirst = '0'; //remplacement de bool
    char c; //variable du get char
	
    cellTete = malloc(sizeof(struct cell));
    cellActuelle = malloc(sizeof(struct cell));
    cellActuelle = cellTete;
   
    makeArray(varArray);
    
    inputEquation:
    printf("> ");
    fflush (stdout);
	
    do 
    {
        //get next char in the buffer
        c = getchar();
		
        if(c != '\n')
        {
            if(checkfirst == '0')
            {
                //set cellTete
                checkfirst = '1';
                cellTete->chiffre = c;
                cellTete->marked = 0;
            }
            else
            {
                struct cell* temp = malloc(sizeof(struct cell));
		
                temp->chiffre = c;
                temp->marked = 0;
                cellActuelle->suivant = temp;
                cellActuelle = temp;				                
            }
        }
    } while ( c != '\n' );
	
    //printString(cellTete);
    //verifyArray(varArray);
    calculer(cellTete, varArray);
    printf("\n");
    goto inputEquation;
    return 0;
}

/* Debug purposes
void printString(struct cell* firstCell){
    do {
        printf("%c", firstCell->chiffre);
        firstCell = firstCell->suivant;
    } while(firstCell != NULL);
}*/

void makeArray(struct variable varArray[]){
    //initialise le array de a à z
    int compteur = 0;
    for (int i = 97; i<=122; i++){
        struct variable var = {i, 0};
        varArray[compteur] = var;
        //printf("%c", varArray[compteur].lettre); debug purpose
        compteur++;
    }
}

/* Debug purposes 
void verifyArray(struct variable varArray[]){
    printf("%c", varArray[4].lettre);
}
*/

void calculer(struct cell* cellTete, struct variable varArray[]){
    struct cell* cellTeteCopie;
    char operateur;
    
    struct num* var1;
    struct num* var2;
    
    var1 = malloc(sizeof(struct num));
    var2 = malloc(sizeof(struct num));
    cellTeteCopie = malloc(sizeof(struct cell));
    
    cellTeteCopie = cellTete;
    
    var1->chiffres = cellTeteCopie;
    
    printf("%c", cellTeteCopie->chiffre);
    
    cellTeteCopie = selectNumber(cellTeteCopie);
    cellTeteCopie = cellTeteCopie->suivant;
 
    var2->chiffres = cellTeteCopie;
    
    printf("%c", cellTeteCopie->chiffre);
    
    cellTeteCopie = selectNumber(cellTeteCopie);
    operateur = cellTeteCopie->suivant->chiffre;
    
    selectOperation(operateur, var1->chiffres, var2->chiffres);
}

struct cell* selectNumber(struct cell* currentCell){
    while (currentCell->chiffre != ' '){
        currentCell = currentCell->suivant;
        printf("%c", currentCell->chiffre);
    }
    return currentCell;
}

void selectOperation(char operateur, struct cell* a, struct cell* b){
    struct cell* temporaireCell1;
    temporaireCell1 = malloc(sizeof(struct cell));
    
    switch(operateur){
        case '+':
            temporaireCell1 = addition(a, b);
            printf("%d", temporaireCell1->chiffre);
            break;
        case '-':
            printf("-");
            break;
        case '*':
            printf("*");
            break;
        case '?':
            printf("?");
            break;
        case '=':
            printf("=");
            break;
        default:
            printf("default");
            break;
    }
}

struct cell* addition(struct cell* a, struct cell* b){
    // Additionne les decimales une par une pour avoir une precision infinie.
    struct cell* add;
    
    struct cell* aa; //initialisation du pointeur buggé de la ligne 200
    
    int valeurAjoutee = 0;
    while (a->suivant->chiffre != ' '){
        a = a->suivant;
    }
    while (b->suivant->chiffre != ' '){
        b = b->suivant;
    }
    
    int z = (int)(a->chiffre-'0') + (int)(b->chiffre-'0');
    a->marked = 1;
    b->marked = 1;
    if(z >= 10){
        valeurAjoutee = 1;
        z = z-10;
    }
    
    /* Cette ligne (200) cause un problème 0 [main] tp1 2100 
     * cygwin_exception::open_stackdumpfile: dumping stack trace 
     * to tp1.exe.stackdump*/
    aa->chiffre = z;

    add->chiffre = z;
    add->suivant = NULL;
    
    //while (a->suivant->marked != 1)
    
    //printf("%d", add->chiffre);
    
    return add;
}
