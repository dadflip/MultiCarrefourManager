#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "intersection.h"

#define MAX_VEHICLES 5

void* drawIntersections(void* arg);

// Ajout d'un indicateur global pour terminer le thread
int stopDrawing = 0;

// Ajout d'un indicateur global pour vérifier s'il y a encore des véhicules
int vehiclesInIntersection = 1;  // Initialisé à 1 car il y a des véhicules au début

// Déclaration d'un compteur statique pour les identifiants de véhicules
static int nextVehicleId = 0;

// Fonction pour générer un identifiant unique pour chaque véhicule
int generateVehicleId() {
    return nextVehicleId++;
}

int main() {

    // Supprimer le fichier log.txt s'il existe déjà
    remove("log.txt");

    pthread_t vehicles[MAX_VEHICLES];
    int vehicleIds[MAX_VEHICLES];

    // Initialisation du fichier de log
    initLogFile();

    // Initialisation des carrefours
    for (int i = 0; i < 4; i++) {
        initIntersection(&intersections[i]);
    }

    // Création de véhicules
    for (int i = 0; i < MAX_VEHICLES; i++) {
        vehicleIds[i] = generateVehicleId();
        pthread_create(&vehicles[i], NULL, vehicleFunction, &vehicleIds[i]);
    }

    // Création du thread pour afficher les carrefours en temps réel
    pthread_t drawThread;
    pthread_create(&drawThread, NULL, drawIntersections, NULL);

    // Attente d'une courte période pour permettre au thread d'affichage de faire son travail
    sleep(5); // Attend 5 secondes (ajustez selon vos besoins)

    // Indiquer au thread d'arrêter
    stopDrawing = 1;

    // Attente de la fin du thread d'affichage
    pthread_join(drawThread, NULL);

    // Vérifier si des véhicules sont encore dans le multicarrefour
    for (int i = 0; i < 4; i++) {
        if (intersections[i].vehicleCount > 0) {
            vehiclesInIntersection = 1;
            break;
        }
    }

    // Attente de la fin de tous les threads de véhicules
    for (int i = 0; i < MAX_VEHICLES; i++) {
        pthread_join(vehicles[i], NULL);
    }

    // Libération des ressources des carrefours
    for (int i = 0; i < 4; i++) {
        destroyIntersection(&intersections[i]);
    }

    // Fermeture du fichier de log avant de quitter
    closeLogFile();

    // Pause supplémentaire pour permettre à l'utilisateur de lire le contenu du terminal
    sleep(5);  // Ajustez selon vos besoins

    // Quitter uniquement s'il n'y a plus de véhicules dans le multicarrefour
    if (vehiclesInIntersection == 0) {
        return 0;
    } else {
        printf("Attendez que tous les véhicules sortent du multicarrefour...\n");
        return 1;
    }
}

void* drawIntersections(void* arg) {
    while (!stopDrawing) {
        //system("clear");  // Efface l'écran du terminal (UNIX)

        // Dessine l'état actuel des carrefours
        for (int i = 0; i < 4; i++) {
            printf("Intersection %d: [", i + 1);
            for (int j = 0; j < intersections[i].vehicleCount; j++) {
                if (j == 0) {
                    printf("o");
                } else {
                    printf(" <- o");
                }
            }
            printf("]\n");
        }

        // Pause pour permettre la mise à jour périodique
        sleep(1);
    }

    return NULL;
}
