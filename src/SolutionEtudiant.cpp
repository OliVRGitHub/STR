// INCLUDES
#include "SolutionEtudiant.hpp"
#include "Header_Prof.h"
#include <time.h>
#include <ctime>

#define SOLUTION_1
//#undef SOLUTION_1
#ifdef SOLUTION_1
// Variables
int nbMangeurs;
int threadsReady = 0;
int nMangeursActuels = 0;
int index = 0;
int autorisation = 1;

clock_t start_time;
long double back_time;

// Fonctions
int Modulo(int dividende, int diviseur){
	int resultatModulo = dividende%diviseur;
	if(resultatModulo<0){
		resultatModulo=diviseur+resultatModulo;
	}
	return resultatModulo;
}

void initMangeursAnciens(){
	mangeursAnciens = (int*)malloc(nbMangeurs * sizeof(int));
	for(int i = 0; i < nbMangeurs; i++){
		mangeursAnciens[i] = ((2*i)-1) % NB_PHILOSOPHES;
	}
}

void initMangeursActuels(){
	mangeursNouveaux = (int*)malloc(nbMangeurs * sizeof(int));
}

void initEtatsPhilosophes(){
	etatsPhilosophes = (char*)malloc(NB_PHILOSOPHES * sizeof(char));
	for (int i = 0; i < NB_PHILOSOPHES; ++i) {
		etatsPhilosophes[i] = P_FAIM;
	}
}


void initSemaphoresTurnToEat(){
	// Allouer de l'espace pour les sémaphores ready
	semTurnToEat = (sem_t*)malloc(sizeof(sem_t));
	// Initialiser les sémaphores des fourchettes
	sem_init((sem_t*)semTurnToEat, 0, 1);

}

void initSemaphoresReady(){
// Allouer de l'espace pour les sémaphores ready
semReady = (sem_t**)malloc(NB_PHILOSOPHES * sizeof(sem_t*));
/*
if (semReady == nullptr) {
	std::cerr << "Erreur d'allocation mémoire pour les sémaphores des fourchettes." << std::endl;
	exit(EXIT_FAILURE);
}
*/

// Initialiser les sémaphores des fourchettes
for (int i = 0; i < NB_PHILOSOPHES; ++i) {
	semReady[i] = (sem_t*)malloc(sizeof(sem_t));
	sem_init(semReady[i], 0, 0);
	/*
	if (sem_init(semReady[i], 0, 0) == -1) {
		std::cerr << "Erreur d'initialisation du sémaphore des fourchettes." << std::endl;
		exit(EXIT_FAILURE);
	}
	*/
}
}

void initSemaphoresFourchettes(){
	// Allouer de l'espace pour les sémaphores des fourchettes
	semFourchettes = (sem_t**)malloc(NB_PHILOSOPHES * sizeof(sem_t*));
	/*
	if (semFourchettes == nullptr) {
		std::cerr << "Erreur d'allocation mémoire pour les sémaphores des fourchettes." << std::endl;
		exit(EXIT_FAILURE);
	}
	*/

	// Initialiser les sémaphores des fourchettes
	for (int i = 0; i < NB_PHILOSOPHES; ++i) {
		semFourchettes[i] = (sem_t*)malloc(sizeof(sem_t));
		sem_init(semFourchettes[i], 0, 1);
	}
}
void initSemaphoresOrdonnancement(){
	// Allouer de l'espace pour les sémaphores d'ordonnancement
	semOrdonnanceur = (sem_t**)malloc(NB_PHILOSOPHES * sizeof(sem_t*));
	/*
	if (semOrdonnanceur == nullptr) {
		std::cerr << "Erreur d'allocation mémoire pour les sémaphores des fourchettes." << std::endl;
		exit(EXIT_FAILURE);
	}
	*/

	 // Initialiser les sémaphores d'ordonnancement
	 for (int i = 0; i < NB_PHILOSOPHES; ++i) {
		 semOrdonnanceur[i] = (sem_t*)malloc(sizeof(sem_t));
		 sem_init(semOrdonnanceur[i], 0, 1);
		 /*
		 if (sem_init(semOrdonnanceur[i], 0, 1) == -1) {
		 std::cerr << "Erreur d'initialisation du sémaphore des fourchettes." << std::endl;
		 exit(EXIT_FAILURE);
		 }
		 */
		 sem_wait(semOrdonnanceur[i]);
	 }
	 /*
	for (int i = 0; i < NB_PHILOSOPHES; i++) {
		sem_wait(semOrdonnanceur[i]);
	}
	*/
}
void initThreadOrdonnanceur(){
	pthread_create(&threadOrdonnanceur, nullptr, &ordonnanceur, nullptr);
	/*
	if (pthread_create(&threadOrdonnanceur, nullptr, &ordonnanceur, nullptr) != 0) {
		std::cerr << "Erreur lors de la création du thread ordonnanceur "<< std::endl;
		exit(EXIT_FAILURE);
	}
	*/

}
void initThreadsPhilosophes(){
	// Création des threads pour les philosophes
	threadsPhilosophes = (pthread_t*)malloc(NB_PHILOSOPHES * sizeof(pthread_t));
	/*
	if (threadsPhilosophes == nullptr) {
		std::cerr << "Erreur d'allocation mémoire pour les threads des philosophes." << std::endl;
		exit(EXIT_FAILURE);
	}
	*/

	 for (int i = 0; i < NB_PHILOSOPHES; ++i) {

		 int* idPhilosophe = (int*)malloc(sizeof(int));
		 /*
		 if (idPhilosophe == nullptr) {
		 std::cerr << "Erreur d'allocation mémoire pour l'ID du philosophe." << std::endl;
		 exit(EXIT_FAILURE);
		 }
		 */
		 *idPhilosophe = i;
		 pthread_create(&(threadsPhilosophes[i]), NULL, &vieDuPhilosophe, idPhilosophe);
		 /*
		 if (pthread_create(&threadsPhilosophes[i], nullptr, &vieDuPhilosophe, idPhilosophe) != 0) {
			 std::cerr << "Erreur lors de la création du thread du philosophe " << i << std::endl;
			 exit(EXIT_FAILURE);
		 }
		 */


	 }
}
void initialisation(){
	time(&instantDebut);
	start_time=clock();
	pthread_mutex_lock(&mutexInitialisation);
	nbMangeurs=(NB_PHILOSOPHES-(NB_PHILOSOPHES%2))/2;
	initMangeursAnciens();
	initMangeursActuels();
	initEtatsPhilosophes();
	initSemaphoresTurnToEat();
	initSemaphoresReady();
	initSemaphoresFourchettes();
	initSemaphoresOrdonnancement();
	initThreadOrdonnanceur();
	initThreadsPhilosophes();
	pthread_mutex_unlock(&mutexInitialisation);
}


bool mangeursAnciensFiniManger(){
	pthread_mutex_lock(&mutexEtats);
	for(int i = 0; i < nbMangeurs; i++){
		if(etatsPhilosophes[mangeursAnciens[i]] == P_MANGE){
			pthread_mutex_unlock(&mutexEtats);
			return false;
		}
	}
	pthread_mutex_unlock(&mutexEtats);
	return true;
}


bool mangeursNouveauxOntFaim(){
	pthread_mutex_lock(&mutexEtats);
	for(int i = 0; i < nbMangeurs; i++){
		if(etatsPhilosophes[(mangeursAnciens[i] + 1) % NB_PHILOSOPHES ] != P_FAIM){
		pthread_mutex_unlock(&mutexEtats);
		return false;
		}
	}
	pthread_mutex_unlock(&mutexEtats);
	return true;
}


 void* ordonnanceur(void* args){
	//pthread_mutex_lock(&mutexInitialisation);
	while(1){
		if(mangeursAnciensFiniManger() && mangeursNouveauxOntFaim()){
			sem_wait((sem_t*)semTurnToEat);
			for(int i = 0; i<nbMangeurs; i++){
				mangeursNouveaux[i] = (mangeursAnciens[i] + 1) % NB_PHILOSOPHES;
				sem_post((sem_t*)(semOrdonnanceur[mangeursNouveaux[i]]));
				mangeursAnciens[i] = mangeursNouveaux[i];
			}
		}
	}
	return NULL;
}

/*
void* ordonnanceur(void* args){
	pthread_mutex_lock(&mutexInitialisation);
	bool changeOK=false;
	while(1){
		if(mangeursAnciensFiniManger() && mangeursNouveauxOntFaim()&& (changeOK==false)){
			changeOK=true;
		}
		pthread_mutex_unlock(&mutexIndex);
		int aut = autorisation;
		pthread_mutex_unlock(&mutexIndex);
		if(changeOK && (aut==1)){
			changeOK=false;
			//sem_wait(semTurnToEat);

			for(int i = 0; i<nbMangeurs; i++){
				mangeursNouveaux[i] = (mangeursAnciens[i] + 1) % NB_PHILOSOPHES;
				sem_post(semOrdonnanceur[mangeursNouveaux[i]]);
				mangeursAnciens[i] = mangeursNouveaux[i];
			}
		}
	}
	return NULL;
}
*/
int tempsAleatoireUS(float maxSec) {
    srand((unsigned int)time(NULL));
    int randomValue = rand();
    float randomS = (float)randomValue / RAND_MAX * maxSec;
    float randomUS = abs(1000000*randomS);
    return (int)randomUS;
}

void* vieDuPhilosophe(void* idPtr){
	int id = *((int*)idPtr);
	 // Configuration du thread : il sera annulable à partir de n'importe quel point de préemption
	 // (appel bloquant, appel système, etc...)
	 pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	 pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);


	 while (1) {

		sem_wait(semOrdonnanceur[id]);
		// Acquisition de la fourchette de gauche
		sem_wait(semFourchettes[id]);
		// Acquisition de la fourchette de droite
		sem_wait(semFourchettes[(id + 1) % NB_PHILOSOPHES]);

		actualiserEtAfficherEtatsPhilosophes(id, P_MANGE);
		usleep(tempsAleatoireUS(DUREE_MANGE_MAX_S));
		index++;
		if(index == nbMangeurs){
			index = 0;
			sem_post((sem_t*)semTurnToEat);
		}
		// Relâchement de la fourchette de gauche
		sem_post(semFourchettes[id]);
		// Relâchement de la fourchette de droite
		sem_post(semFourchettes[(id + 1) % NB_PHILOSOPHES]);
		actualiserEtAfficherEtatsPhilosophes(id, P_PENSE);
		usleep(tempsAleatoireUS(DUREE_PENSE_MAX_S));
		actualiserEtAfficherEtatsPhilosophes(id, P_FAIM);


		pthread_testcancel(); // Point où l'annulation du thread est permise
	 }
	 return NULL;
}


void ecritureConsole(char Message[], int info){
	pthread_mutex_lock(&mutexCout);
	std::cout << Message << info << std::endl;
	pthread_mutex_unlock(&mutexCout);
}
/*
void* vieDuPhilosophe(void* idPtr){
	int id = *((int*)idPtr);
	 // Configuration du thread : il sera annulable à partir de n'importe quel point de préemption
	 // (appel bloquant, appel système, etc...)
	 pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	 pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	 while (1) {
		sem_wait(semOrdonnanceur[id]);
		// Acquisition de la fourchette de gauche
		sem_wait(semFourchettes[id]);
		// Acquisition de la fourchette de droite
		sem_wait(semFourchettes[(id + 1) % NB_PHILOSOPHES]);
		actualiserEtAfficherEtatsPhilosophes(id, P_MANGE);
		usleep(tempsAleatoireUS(DUREE_MANGE_MAX_S));

			index++;
			if(index == nbMangeurs){//
				index = 0;
				//sem_post(semTurnToEat);if(changeOK && (aut==1)){
				pthread_mutex_lock(&mutexIndex);
				autorisation = 1;
				pthread_mutex_unlock(&mutexIndex);
			}


		// Relâchement de la fourchette de gauche
		sem_post(semFourchettes[id]);
		// Relâchement de la fourchette de droite
		sem_post(semFourchettes[(id + 1) % NB_PHILOSOPHES]);
		actualiserEtAfficherEtatsPhilosophes(id, P_PENSE);
		usleep(tempsAleatoireUS(DUREE_PENSE_MAX_S));
		actualiserEtAfficherEtatsPhilosophes(id, P_FAIM);
		pthread_testcancel(); // Point où l'annulation du thread est permise
	 }
	 return NULL;
}
*/

void actualiserEtAfficherEtatsPhilosophes(int idPhilosopheChangeant, char nouvelEtat)
{
    // ***** À IMPLÉMENTER : PROTECTION ÉTATS *****
    // ***** À IMPLÉMENTER : PROTECTION CONSOLE *****
	pthread_mutex_lock(&mutexCout);
	pthread_mutex_lock(&mutexEtats);
    etatsPhilosophes[idPhilosopheChangeant] = nouvelEtat;
    pthread_mutex_unlock(&mutexEtats);
    std::string id;
    if(idPhilosopheChangeant<10){
    	id=std::to_string(idPhilosopheChangeant)+" ";
    } else {
    	id=std::to_string(idPhilosopheChangeant);
    }
    std::cout << id << " " << determinerCouleur(nouvelEtat) << nouvelEtat << RESET_COLOR << " | ";
    for (int i=0;i<NB_PHILOSOPHES;i++) {
		char etatsPhilosophesEnCours=etatsPhilosophes[i];
		std::cout << " ";
		if(i==idPhilosopheChangeant){
			std::cout << determinerCouleur(nouvelEtat) << etatsPhilosophesEnCours << RESET_COLOR;
        } else {
        	std::cout << etatsPhilosophesEnCours;
        }
        std::cout << " ";
    }
    //std::cout << "          (t=" << difftime(time(NULL), instantDebut) << ")" << std::endl;
    long double elapsed_time=((long double)(clock()-start_time))* 1000.0 / CLOCKS_PER_SEC;
	std::cout << "  (t=" << elapsed_time << ")" << std::endl;
	std::cout << "|" << elapsed_time-back_time << std::endl;
	back_time=elapsed_time;
    pthread_mutex_unlock(&mutexCout);
}

std::string determinerCouleur(int Etat) {
    std::string couleurTexte;

    switch (Etat) {
        case P_MANGE:
            couleurTexte = GREEN_COLOR;
            break;
        case P_FAIM:
            couleurTexte = RED_COLOR;
            break;
        case P_PENSE:
            couleurTexte = BLUE_COLOR;
            break;
        default:
            couleurTexte = RESET_COLOR; // Couleur par défaut
            break;
    }

    return couleurTexte;
}

//
void terminerProgramme(){
	// Annulation des threads des philosophes
	for (int i = 0; i < NB_PHILOSOPHES; ++i) {
		pthread_cancel((pthread_t)threadsPhilosophes[i]);
	}
	// Attente de la terminaison des threads
	for (int i = 0; i < NB_PHILOSOPHES; ++i) {
		pthread_join((pthread_t)threadsPhilosophes[i], nullptr);
	}
	// Détruire les sémaphores des fourchettes
	if (semFourchettes != nullptr) {
		for (int i = 0; i < NB_PHILOSOPHES; ++i) {
			sem_destroy(semFourchettes[i]);
			free(semFourchettes[i]);
		}
	free(semFourchettes);
	}
	if (semOrdonnanceur != nullptr) {
	for (int i = 0; i < NB_PHILOSOPHES; ++i) {
		sem_destroy(semOrdonnanceur[i]);
		free(semOrdonnanceur[i]);
		}
		free(semOrdonnanceur);
	}
	if (semReady != nullptr) {
		for (int i = 0; i < NB_PHILOSOPHES; ++i) {
			sem_destroy(semReady[i]);
			free(semReady[i]);
		}
		free(semReady);
	}
	/*
	if (semTurnToEat != nullptr) {
		for (int i = 0; i < NB_PHILOSOPHES; ++i) {
			sem_destroy(semTurnToEat[i]);
			free(semTurnToEat[i]);
		}
		free(semTurnToEat);
	}
	*/
	sem_destroy((sem_t*)semTurnToEat);
	free((sem_t*)semTurnToEat);

	pthread_cancel((pthread_t)threadOrdonnanceur);
	// Libération de la mémoire
	free((pthread_t*)threadsPhilosophes);
	free((char*)etatsPhilosophes);
	free((int*)mangeursAnciens);
	free((int*)mangeursNouveaux);
}

#endif // #ifdef SOLUTION_1



//#define SOLUTION_2
#undef SOLUTION_2

#ifdef SOLUTION_2
// Variables
int nbMangeurs;
int threadsReady = 0;
int nMangeursActuels = 0;
int index = 0;
int autorisation = 1;
pthread_mutex_t mutexCompteur;
int* mangeursAutorises;
int* nFoisMangerPhilosophes;
// Fonctions
int Modulo(int dividende, int diviseur){
	int resultatModulo = dividende%diviseur;
	if(resultatModulo<0){
		resultatModulo=diviseur+resultatModulo;
	}
	return resultatModulo;
}

void initMangeursAnciens(){
	mangeursAnciens = (int*)malloc(nbMangeurs * sizeof(int));
	for(int i = 0; i < nbMangeurs; i++){
		mangeursAnciens[i] = ((2*i)-1) % NB_PHILOSOPHES;
	}
}

void initMangeursActuels(){
	mangeursNouveaux = (int*)malloc(nbMangeurs * sizeof(int));
	mangeursAutorises= (int*)malloc(nbMangeurs * sizeof(int));
	nFoisMangerPhilosophes= (int*)malloc(NB_PHILOSOPHES * sizeof(int));
	for(int i = 0; i < nbMangeurs; i++){
		mangeursAutorises[i] = (2*i) % NB_PHILOSOPHES;
	}
	for(int i = 0; i < NB_PHILOSOPHES; i++){
		nFoisMangerPhilosophes[i] = 0;
	}
}

void initEtatsPhilosophes(){
	etatsPhilosophes = (char*)malloc(NB_PHILOSOPHES * sizeof(char));
	for (int i = 0; i < NB_PHILOSOPHES; ++i) {
		etatsPhilosophes[i] = P_FAIM;
	}
}
void initSemaphoresTurnToEat(){
	// Allouer de l'espace pour les sémaphores ready
	semTurnToEat = (sem_t*)malloc(sizeof(sem_t));
	/*
	if (semTurnToEat == nullptr) {
		std::cerr << "Erreur d'allocation mémoire pour les sémaphores des fourchettes." << std::endl;
		exit(EXIT_FAILURE);
	}
	*/

	// Initialiser les sémaphores des fourchettes
	sem_init(semTurnToEat, 0, 1);
	/*
	for (int i = 0; i < NB_PHILOSOPHES; ++i) {
		semTurnToEat[i] = (sem_t*)malloc(sizeof(sem_t));
		sem_init(semTurnToEat[i], 0, 1);

		//if (sem_init(semTurnToEat[i], 0, 1) == -1) {
		//	std::cerr << "Erreur d'initialisation du sémaphore des fourchettes." << std::endl;
		//exit(EXIT_FAILURE);
		//}

	}*/
}

void initSemaphoresReady(){
// Allouer de l'espace pour les sémaphores ready
semReady = (sem_t**)malloc(NB_PHILOSOPHES * sizeof(sem_t*));
/*
if (semReady == nullptr) {
	std::cerr << "Erreur d'allocation mémoire pour les sémaphores des fourchettes." << std::endl;
	exit(EXIT_FAILURE);
}
*/

// Initialiser les sémaphores des fourchettes
for (int i = 0; i < NB_PHILOSOPHES; ++i) {
	semReady[i] = (sem_t*)malloc(sizeof(sem_t));
	sem_init(semReady[i], 0, 1);
	/*
	if (sem_init(semReady[i], 0, 0) == -1) {
		std::cerr << "Erreur d'initialisation du sémaphore des fourchettes." << std::endl;
		exit(EXIT_FAILURE);
	}
	*/
}
}

void initSemaphoresFourchettes(){
	// Allouer de l'espace pour les sémaphores des fourchettes
	semFourchettes = (sem_t**)malloc(NB_PHILOSOPHES * sizeof(sem_t*));
	/*
	if (semFourchettes == nullptr) {
		std::cerr << "Erreur d'allocation mémoire pour les sémaphores des fourchettes." << std::endl;
		exit(EXIT_FAILURE);
	}
	*/

	// Initialiser les sémaphores des fourchettes
	for (int i = 0; i < NB_PHILOSOPHES; ++i) {
		semFourchettes[i] = (sem_t*)malloc(sizeof(sem_t));
		sem_init(semFourchettes[i], 0, 1);
		/*
		if (sem_init(semFourchettes[i], 0, 1) == -1) {
			std::cerr << "Erreur d'initialisation du sémaphore des fourchettes." << std::endl;
			exit(EXIT_FAILURE);
		}
		*/
	}
}
void initSemaphoresOrdonnancement(){
	// Allouer de l'espace pour les sémaphores d'ordonnancement
	semOrdonnanceur = (sem_t**)malloc(NB_PHILOSOPHES * sizeof(sem_t*));
	/*
	if (semOrdonnanceur == nullptr) {
		std::cerr << "Erreur d'allocation mémoire pour les sémaphores des fourchettes." << std::endl;
		exit(EXIT_FAILURE);
	}
	*/

	 // Initialiser les sémaphores d'ordonnancement
	 for (int i = 0; i < NB_PHILOSOPHES; ++i) {
		 semOrdonnanceur[i] = (sem_t*)malloc(sizeof(sem_t));
		 sem_init(semOrdonnanceur[i], 0, 1);
		 /*
		 if (sem_init(semOrdonnanceur[i], 0, 1) == -1) {
		 std::cerr << "Erreur d'initialisation du sémaphore des fourchettes." << std::endl;
		 exit(EXIT_FAILURE);
		 }
		 */
		 int autor=0;
		for(int j = 0; j < nbMangeurs; j++){
			if(i==mangeursAutorises[j]){
				autor++;
			}
		}
		if(autor==0){
			sem_wait(semOrdonnanceur[i]);
		}

	 }
	 /*
	for (int i = 0; i < NB_PHILOSOPHES; i++) {
		sem_wait(semOrdonnanceur[i]);
	}
	*/
}
void initThreadOrdonnanceur(){
	pthread_create(&threadOrdonnanceur, nullptr, &ordonnanceur, nullptr);
	/*
	if (pthread_create(&threadOrdonnanceur, nullptr, &ordonnanceur, nullptr) != 0) {
		std::cerr << "Erreur lors de la création du thread ordonnanceur "<< std::endl;
		exit(EXIT_FAILURE);
	}
	*/

}
void initThreadsPhilosophes(){
	// Création des threads pour les philosophes
	threadsPhilosophes = (pthread_t*)malloc(NB_PHILOSOPHES * sizeof(pthread_t));
	/*
	if (threadsPhilosophes == nullptr) {
		std::cerr << "Erreur d'allocation mémoire pour les threads des philosophes." << std::endl;
		exit(EXIT_FAILURE);
	}
	*/

	 for (int i = 0; i < NB_PHILOSOPHES; ++i) {

		 int* idPhilosophe = (int*)malloc(sizeof(int));
		 /*
		 if (idPhilosophe == nullptr) {
		 std::cerr << "Erreur d'allocation mémoire pour l'ID du philosophe." << std::endl;
		 exit(EXIT_FAILURE);
		 }
		 */
		 *idPhilosophe = i;
		 pthread_create(&threadsPhilosophes[i], nullptr, &vieDuPhilosophe, idPhilosophe);
		 /*
		 if (pthread_create(&threadsPhilosophes[i], nullptr, &vieDuPhilosophe, idPhilosophe) != 0) {
			 std::cerr << "Erreur lors de la création du thread du philosophe " << i << std::endl;
			 exit(EXIT_FAILURE);
		 }
		 */


	 }
}
void initialisation(){
	pthread_mutex_lock(&mutexInitialisation);
	nbMangeurs=(NB_PHILOSOPHES-(NB_PHILOSOPHES%2))/2;
	initMangeursAnciens();
	initMangeursActuels();
	initEtatsPhilosophes();
	initSemaphoresTurnToEat();
	initSemaphoresReady();
	initSemaphoresFourchettes();
	initSemaphoresOrdonnancement();
	initThreadOrdonnanceur();
	initThreadsPhilosophes();
	pthread_mutex_unlock(&mutexInitialisation);
}


bool mangeursAnciensFiniManger(){
	pthread_mutex_lock(&mutexEtats);
	for(int i = 0; i < nbMangeurs; i++){
		if(etatsPhilosophes[mangeursAnciens[i]] == P_MANGE){
			pthread_mutex_unlock(&mutexEtats);
			return false;
		}
	}
	pthread_mutex_unlock(&mutexEtats);
	return true;
}


bool mangeursNouveauxOntFaim(){
	pthread_mutex_lock(&mutexEtats);
	for(int i = 0; i < nbMangeurs; i++){
		if(etatsPhilosophes[(mangeursAnciens[i] + 1) % NB_PHILOSOPHES ] != P_FAIM){
		pthread_mutex_unlock(&mutexEtats);
		return false;
		}
	}
	pthread_mutex_unlock(&mutexEtats);
	return true;
}
/*
 void* ordonnanceur(void* args){
	pthread_mutex_lock(&mutexInitialisation);
	while(1){
		if(mangeursAnciensFiniManger() && mangeursNouveauxOntFaim()){
			sem_wait(semTurnToEat);
			for(int i = 0; i<nbMangeurs; i++){
				mangeursNouveaux[i] = (mangeursAnciens[i] + 1) % NB_PHILOSOPHES;
				sem_post(semOrdonnanceur[mangeursNouveaux[i]]);
				mangeursAnciens[i] = mangeursNouveaux[i];
			}
		}
	}
	return NULL;
}
*/

void* ordonnanceur(void* args){
	/*
	pthread_mutex_lock(&mutexInitialisation);
	bool changeOK=false;
	while(1){
		if(mangeursAnciensFiniManger() && mangeursNouveauxOntFaim()&& (changeOK==false)){
			changeOK=true;
		}
		pthread_mutex_unlock(&mutexIndex);
		int aut = autorisation;
		pthread_mutex_unlock(&mutexIndex);
		if(changeOK && (aut==1)){
			changeOK=false;
			//sem_wait(semTurnToEat);

			for(int i = 0; i<nbMangeurs; i++){
				mangeursNouveaux[i] = (mangeursAnciens[i] + 1) % NB_PHILOSOPHES;
				sem_post(semOrdonnanceur[mangeursNouveaux[i]]);
				mangeursAnciens[i] = mangeursNouveaux[i];
			}
		}
	}*/
	return NULL;

}

int tempsAleatoireUS(float maxSec) {
    srand((unsigned int)time(NULL));
    int randomValue = rand();
    float randomS = (float)randomValue / RAND_MAX * maxSec;
    float randomUS = abs(1000000*randomS);
    return (int)randomUS;
}
/*
void* vieDuPhilosophe(void* idPtr){
	int id = *((int*)idPtr);
	 // Configuration du thread : il sera annulable à partir de n'importe quel point de préemption
	 // (appel bloquant, appel système, etc...)
	 pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	 pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	 while (1) {
		sem_wait(semOrdonnanceur[id]);
		// Acquisition de la fourchette de gauche
		sem_wait(semFourchettes[id]);
		// Acquisition de la fourchette de droite
		sem_wait(semFourchettes[(id + 1) % NB_PHILOSOPHES]);
		actualiserEtAfficherEtatsPhilosophes(id, P_MANGE);
		usleep(tempsAleatoireUS(DUREE_MANGE_MAX_S));
		index++;
		if(index == nbMangeurs){
			index = 0;
			sem_post(semTurnToEat);
		}
		// Relâchement de la fourchette de gauche
		sem_post(semFourchettes[id]);
		// Relâchement de la fourchette de droite
		sem_post(semFourchettes[(id + 1) % NB_PHILOSOPHES]);
		actualiserEtAfficherEtatsPhilosophes(id, P_PENSE);
		usleep(tempsAleatoireUS(DUREE_PENSE_MAX_S));
		actualiserEtAfficherEtatsPhilosophes(id, P_FAIM);
		pthread_testcancel(); // Point où l'annulation du thread est permise
	 }
	 return NULL;
}
*/

void ecritureConsole(char Message[], int info){
	pthread_mutex_lock(&mutexCout);
	std::cout << Message << info << std::endl;
	pthread_mutex_unlock(&mutexCout);
}

void* vieDuPhilosophe(void* idPtr){
	int id = *((int*)idPtr);
	 // Configuration du thread : il sera annulable à partir de n'importe quel point de préemption
	 // (appel bloquant, appel système, etc...)
	 pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	 pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	 while (1) {
		sem_wait(semOrdonnanceur[id]);
		// Acquisition de la fourchette de gauche
		sem_wait(semFourchettes[id]);
		// Acquisition de la fourchette de droite
		sem_wait(semFourchettes[(id + 1) % NB_PHILOSOPHES]);
		actualiserEtAfficherEtatsPhilosophes(id, P_MANGE);
		usleep(tempsAleatoireUS(DUREE_MANGE_MAX_S));

		// Relâchement de la fourchette de gauche
		sem_post(semFourchettes[id]);
		// Relâchement de la fourchette de droite
		sem_post(semFourchettes[(id + 1) % NB_PHILOSOPHES]);
		actualiserEtAfficherEtatsPhilosophes(id, P_PENSE);
		usleep(tempsAleatoireUS(DUREE_PENSE_MAX_S));
		actualiserEtAfficherEtatsPhilosophes(id, P_FAIM);
		pthread_testcancel(); // Point où l'annulation du thread est permise
	 }
	 return NULL;
}

char getEtatPhilosophe(int idPhilosophe){
	char etatPhilosopheEnCours;
	//pthread_mutex_lock(&mutexEtats);
	etatPhilosopheEnCours=etatsPhilosophes[idPhilosophe];
	//pthread_mutex_unlock(&mutexEtats);
	return etatPhilosopheEnCours;
}
void setEtatPhilosophe(int idPhilosophe, char nouvelEtat){
	//pthread_mutex_lock(&mutexEtats);
	etatsPhilosophes[idPhilosophe]=nouvelEtat;
	//pthread_mutex_unlock(&mutexEtats);
}


void OrdonnancerAvecSemaphoresFinal(int idPhilosophe,char Etat){
	int nbPhilosophesFaim=0;
	int nbPhilosophesFinisManger=0;
	int nbPhilosophesDejaManger=0;
	int nbAnciensPhilosophesMangeant=0;
	switch(Etat){
		case 'M':
			break;

		case 'F':

			for(int i=0;i<nbMangeurs;i++){
				pthread_mutex_lock(&mutexEtats);
				if((getEtatPhilosophe(mangeursAutorises[i])==P_FAIM)&&(nFoisMangerPhilosophes[mangeursAutorises[i]]==0)){
					nbPhilosophesFaim++;
				}
				pthread_mutex_unlock(&mutexEtats);
			}
			if(nbPhilosophesFaim==nbMangeurs){
				for(int i=0;i<nbMangeurs;i++){
					//sem_init(semOrdonnanceur[(mangeursAutorises[i]-1)%NB_PHILOSOPHES], 0, 1);
					sem_post(semOrdonnanceur[mangeursAutorises[i]]);
					nFoisMangerPhilosophes[mangeursAutorises[i]]=1;
				}
			}


			break;

		case 'P':


			for(int i=0;i<nbMangeurs;i++){
				pthread_mutex_lock(&mutexEtats);
				if(etatsPhilosophes[mangeursAutorises[i]]==P_PENSE){
					nbPhilosophesFinisManger++;
				}
				pthread_mutex_unlock(&mutexEtats);
			}

			if(nbPhilosophesFinisManger==nbMangeurs){
				for(int i=0;i<nbMangeurs;i++){
					mangeursAutorises[i]=(mangeursAutorises[i]+1)%NB_PHILOSOPHES;
					nFoisMangerPhilosophes[mangeursAutorises[i]]=0;
				}
			}

			break;
	}
}

void actualiserEtAfficherEtatsPhilosophes(int idPhilosopheChangeant, char nouvelEtat)
{
    // ***** À IMPLÉMENTER : PROTECTION ÉTATS *****
    // ***** À IMPLÉMENTER : PROTECTION CONSOLE *****
	pthread_mutex_lock(&mutexCout);
	setEtatPhilosophe(idPhilosopheChangeant, nouvelEtat);

    OrdonnancerAvecSemaphoresFinal(idPhilosopheChangeant,nouvelEtat);


    std::string id;
    if(idPhilosopheChangeant<10){
    	id=std::to_string(idPhilosopheChangeant)+" ";
    } else {
    	id=std::to_string(idPhilosopheChangeant);
    }
    std::cout << id << " " << determinerCouleur(nouvelEtat) << nouvelEtat << RESET_COLOR << " | ";
    for (int i=0;i<NB_PHILOSOPHES;i++) {
		char etatsPhilosophesEnCours=etatsPhilosophes[i];
		std::cout << " ";
		if(i==idPhilosopheChangeant){
			std::cout << determinerCouleur(nouvelEtat) << etatsPhilosophesEnCours << RESET_COLOR;
        } else {
        	std::cout << etatsPhilosophesEnCours;
        }
        std::cout << " ";
    }
    std::cout << "          (t=" << difftime(time(NULL), instantDebut) << ")" << std::endl;
    pthread_mutex_unlock(&mutexCout);
}

std::string determinerCouleur(int Etat) {
    std::string couleurTexte;

    switch (Etat) {
        case P_MANGE:
            couleurTexte = GREEN_COLOR;
            break;
        case P_FAIM:
            couleurTexte = RED_COLOR;
            break;
        case P_PENSE:
            couleurTexte = BLUE_COLOR;
            break;
        default:
            couleurTexte = RESET_COLOR; // Couleur par défaut
            break;
    }

    return couleurTexte;
}


void terminerProgramme(){
	// Annulation des threads des philosophes
	for (int i = 0; i < NB_PHILOSOPHES; ++i) {
		pthread_cancel(threadsPhilosophes[i]);
	}
	// Attente de la terminaison des threads
	for (int i = 0; i < NB_PHILOSOPHES; ++i) {
		pthread_join(threadsPhilosophes[i], nullptr);
	}
	// Détruire les sémaphores des fourchettes
	if (semFourchettes != nullptr) {
		for (int i = 0; i < NB_PHILOSOPHES; ++i) {
			sem_destroy(semFourchettes[i]);
			free(semFourchettes[i]);
		}
	free(semFourchettes);
	}
	if (semOrdonnanceur != nullptr) {
	for (int i = 0; i < NB_PHILOSOPHES; ++i) {
		sem_destroy(semOrdonnanceur[i]);
		free(semOrdonnanceur[i]);
		}
		free(semOrdonnanceur);
	}
	if (semReady != nullptr) {
		for (int i = 0; i < NB_PHILOSOPHES; ++i) {
			sem_destroy(semReady[i]);
			free(semReady[i]);
		}
		free(semReady);
	}
	/*
	if (semTurnToEat != nullptr) {
		for (int i = 0; i < NB_PHILOSOPHES; ++i) {
			sem_destroy(semTurnToEat[i]);
			free(semTurnToEat[i]);
		}
		free(semTurnToEat);
	}
	*/
	sem_destroy(semTurnToEat);
	free(semTurnToEat);

	pthread_cancel(threadOrdonnanceur);
	// Libération de la mémoire
	free(threadsPhilosophes);
	free(etatsPhilosophes);
	free(mangeursAnciens);
	free(mangeursNouveaux);
}
#endif // #ifdef SOLUTION_2
