// intersection.h
#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <semaphore.h>
#include <pthread.h>

// Structure représentant un carrefour
typedef struct {
    sem_t semaphore;  // Sémaphore pour la gestion de l'intersection
    int vehicleCount; // Compteur de véhicules traversant l'intersection
    pthread_mutex_t mutex;  // Mutex pour protéger l'accès aux ressources
} Intersection;

// Structure pour la simulation de la communication
struct Message {
    int vehicleId;
    int vehicleType;  // Type de véhicule (0: normal, 1: ambulance, 2: pompier, 3: police)
    int speed;        // Vitesse du véhicule
    int currentIntersection; // Intersection actuelle du véhicule
    int destination;  // Intersection de destination du véhicule
};

void initLogFile();
void closeLogFile();

// Initialisation d'un carrefour
void initIntersection(Intersection* intersection);

// Libération des ressources d'un carrefour
void destroyIntersection(Intersection* intersection);

// Fonction exécutée par les véhicules
void* vehicleFunction(void* arg);

// Fonction pour envoyer un message
void sendMessage(struct Message message);

// Fonction pour recevoir un message
struct Message receiveMessage();

// Fonction pour générer un nombre aléatoire entre min et max (inclus)
int getRandomNumber(int min, int max);

// Déclaration de la variable intersections
extern Intersection intersections[4];

#endif
