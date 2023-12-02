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
    initHunter(name, &h1, house.rooms.head->data, 0, &house.evList);
    hunterNameInput(name);
    initHunter(name, &h2, house.rooms.head->data, 1, &house.evList);
    hunterNameInput(name);
    initHunter(name, &h3, house.rooms.head->data, 2, &house.evList);
    hunterNameInput(name);
    initHunter(name, &h4, house.rooms.head->data, 3, &house.evList);



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
    // for(int i = 0; i < 4; i++){
    //     pthread_join(threads[i], NULL);
    // }
    

    //ghost thread
    /*
    addEvidence(&house.rooms.head->data->evList, 0);
    printf("EVIDENCE IN THE ROOM: %d", house.rooms.head->data->evList.head->evidence);
    HunterType h1;
    initHunter("joel", &h1, house.rooms.head->data, 0, &house.evList);
    HunterType h2;
    initHunter("jeff", &h2, house.rooms.head->data, 1, &house.evList);
    HunterType h3;
    initHunter("john", &h3, house.rooms.head->data, 2, &house.evList);
    HunterType h4;
    initHunter("joe", &h4, house.rooms.head->data, 3, &house.evList);
    ghostAction(&ghost);
    hunterAction(&h1);
    hunterAction(&h2);
    hunterAction(&h3);
    hunterAction(&h4);
    */

    //removeHunter(&h1.currRoom->hunterList, &h1);
    // removeHunter(&h2.currRoom->hunterList, &h2);
    // removeHunter(&h3.currRoom->hunterList, &h3);
    // removeHunter(&h4.currRoom->hunterList, &h4);
    //printRoomList(&house.rooms);

    printf("\n\n\n\n\n\n D O N E   P R O G R A M");
    
    cleanupHouse(&house);
    return 0;
}

