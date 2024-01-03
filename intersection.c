// intersection.c
#include "intersection.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// Files de messages pour l'envoi et la réception
#define MAX_QUEUE_SIZE 200
struct Message messageQueue[MAX_QUEUE_SIZE];
int messageQueueSize = 0;

// Mutex pour protéger l'accès à la file de messages
pthread_mutex_t messageQueueMutex = PTHREAD_MUTEX_INITIALIZER;

// Condition pour notifier la réception d'un nouveau message
pthread_cond_t messageReceived = PTHREAD_COND_INITIALIZER;

// Variable globale pour l'intersection
Intersection intersections[4];

// Fichier de log
FILE* logFile;

void initIntersection(Intersection* intersection) {
    sem_init(&intersection->semaphore, 0, 1);
    intersection->vehicleCount = 0;
    pthread_mutex_init(&intersection->mutex, NULL);
}

void destroyIntersection(Intersection* intersection) {
    sem_destroy(&intersection->semaphore);
    pthread_mutex_destroy(&intersection->mutex);  // Ajout pour détruire le mutex
}

void initLogFile() {
    // Maintenant vide, car nous n'utilisons plus de fichier de log
}

void closeLogFile() {
    // Maintenant vide, car nous n'utilisons plus de fichier de log
}

void* vehicleFunction(void* arg) {
    int id = *((int*)arg);

    int currentIntersection = id % 4;

    sem_wait(&intersections[currentIntersection].semaphore);
    pthread_mutex_lock(&intersections[currentIntersection].mutex);

    printf("Vehicle %d: Requesting route from intersection %d...\n", id, currentIntersection + 1);
    usleep(500000);  // Pause de 500 millisecondes

    struct Message request;
    request.vehicleId = id;
    request.currentIntersection = currentIntersection;

    sendMessage(request);

    sleep(2);

    struct Message response = receiveMessage();

    printf("Vehicle %d: Received optimal route. Crossing the intersection %d.\n", response.vehicleId, currentIntersection + 1);
    usleep(500000);  // Pause de 500 millisecondes

    intersections[currentIntersection].vehicleCount--;

    int randomChoice = getRandomNumber(0, 2);

    if (randomChoice == 1) {
        int newDestination;
        do {
            newDestination = getRandomNumber(0, 3);
        } while (newDestination == currentIntersection);

        request.destination = newDestination;
        intersections[newDestination].vehicleCount++;
        sem_post(&intersections[newDestination].semaphore);

        printf("Vehicle %d: Moving to intersection %d.\n", id, newDestination + 1);
        usleep(500000);  // Pause de 500 millisecondes
    } else if (randomChoice == 2) {
        printf("Vehicle %d: Exiting the multicarrefour.\n", id);
        usleep(500000);  // Pause de 500 millisecondes
    } else {
        intersections[currentIntersection].vehicleCount++;
        sem_post(&intersections[currentIntersection].semaphore);

        printf("Vehicle %d: Staying at intersection %d.\n", id, currentIntersection + 1);
        usleep(500000);  // Pause de 500 millisecondes
    }

    pthread_mutex_unlock(&intersections[currentIntersection].mutex);
    sem_post(&intersections[currentIntersection].semaphore);

    pthread_exit(NULL);
}

void sendMessage(struct Message message) {
    pthread_mutex_lock(&messageQueueMutex);

    while (messageQueueSize == MAX_QUEUE_SIZE) {
        pthread_cond_wait(&messageReceived, &messageQueueMutex);
    }

    messageQueue[messageQueueSize++] = message;

    pthread_mutex_unlock(&messageQueueMutex);
    pthread_cond_signal(&messageReceived);

    printf("Sending message: Vehicle %d requesting route\n", message.vehicleId);
}

struct Message receiveMessage() {
    struct Message receivedMessage;

    pthread_mutex_lock(&messageQueueMutex);

    while (messageQueueSize == 0) {
        pthread_cond_wait(&messageReceived, &messageQueueMutex);
    }

    receivedMessage = messageQueue[--messageQueueSize];

    pthread_mutex_unlock(&messageQueueMutex);

    printf("Received message: Optimal route provided by server-controler\n");

    return receivedMessage;
}



// Fonction pour générer un nombre aléatoire entre min et max (inclus)
int getRandomNumber(int min, int max) {
    static int initialized = 0;

    if (!initialized) {
        srand(time(NULL));
        initialized = 1;
    }

    return rand() % (max - min + 1) + min;
}

// Fonction pour verrouiller l'intersection
void lockIntersection(Intersection* intersection) {
    pthread_mutex_lock(&intersection->mutex);
}

// Fonction pour déverrouiller l'intersection
void unlockIntersection(Intersection* intersection) {
    pthread_mutex_unlock(&intersection->mutex);
}

