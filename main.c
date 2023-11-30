#include "defs.h"

int main(){
    srand(time(NULL));// Initialize the random number generator

    // Create the house: You may change this, but it's here for demonstration purposes
    // Note: This code will not compile until you have implemented the house functions and structures
    HouseType house;
    initHouse(&house);
    populateRooms(&house);
    
    
    GhostType ghost;
    initGhost(&ghost, chooseStartingRoom(&house.rooms));
    //printf("GHOST STARTING CLASS: %d ", ghost.class);
    // addEvidence(&house.rooms.head->data->evList, 0);
    // printf("EVIDENCE IN THE ROOM: %d", house.rooms.head->data->evList.head->evidence);
    
    // HunterType h1;
    // initHunter("bob", &h1, house.rooms.head->data, 0, &house.evList);

    ghostAction(&ghost);
    
    //printRoomList(&house.rooms);
    cleanupHouse(&house);
    return 0;
}

