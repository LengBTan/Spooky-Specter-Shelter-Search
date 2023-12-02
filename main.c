#include "defs.h"

int main(){
    srand(time(NULL));// Initialize the random number generator
    pthread_t threads[NUM_THREADS];//create 5 threads, 4 hunters and 1 ghost

    //initialize a house
    HouseType house;
    initHouse(&house);
    populateRooms(&house);
    
    HunterType h1,h2,h3,h4;
    GhostType ghost;
    initGhost(&ghost, chooseStartingRoom(&house.rooms, &ghost));

    // initialize hunters:
    char name[MAX_STR];
    hunterNameInput(name);
    initHunter(name, &h1, house.rooms.head->data, 0, &house.evList, &house);
    hunterNameInput(name);
    initHunter(name, &h2, house.rooms.head->data, 1, &house.evList, &house);
    hunterNameInput(name);
    initHunter(name, &h3, house.rooms.head->data, 2, &house.evList, &house);
    hunterNameInput(name);
    initHunter(name, &h4, house.rooms.head->data, 3, &house.evList, &house);

    pthread_create(threads+0, NULL, hunterAction, &h1);
    pthread_create(threads+1, NULL, hunterAction, &h2);
    pthread_create(threads+2, NULL, hunterAction, &h3);
    pthread_create(threads+3, NULL, hunterAction, &h4);
    pthread_create(threads+4, NULL, ghostAction, &ghost);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);
    pthread_join(threads[3], NULL);
    pthread_join(threads[4], NULL);

    finalResult(&house, &ghost);

    cleanupHouse(&house);
    return 0;
}

