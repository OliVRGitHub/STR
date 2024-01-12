#ifndef SolutionEtudiant_hpp

#define SolutionEtudiant_hpp

// INCLUDES
#include <iostream>
#include <semaphore.h>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <unistd.h>

// Gestion des couleurs
#define RED_COLOR "\033[1;31m"
#define GREEN_COLOR "\033[1;32m"
#define BLUE_COLOR "\033[1;94m"
#define RESET_COLOR "\033[0m"


// Dans ce fichier, rajouter tout ce qui pourrait manquer pour programmer la solution
pthread_t threadOrdonnanceur;
sem_t** semOrdonnanceur;
sem_t** semReady;
sem_t* semTurnToEat;
pthread_mutex_t mutexInitialisation;
pthread_mutex_t mutexIndex;


bool ordonnanceurReady = false;
void* ordonnanceur(void* args);
int* mangeursAnciens;
int* mangeursNouveaux;
bool groupEndEating();
bool mangeursAnciensFiniManger();
bool mangeursNouveauxOntFaim();
std::string determinerCouleur(int Etat);
void ecritureConsole(char Message[], int info);
#endif /* SolutionEtudiant_hpp */
