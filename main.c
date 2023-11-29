#include "defs.h"

int main(){
    srand(time(NULL));// Initialize the random number generator

    // Create the house: You may change this, but it's here for demonstration purposes
    // Note: This code will not compile until you have implemented the house functions and structures
    HouseType house;
    initHouse(&house);
    populateRooms(&house);
    printRoomList(&house.rooms);
    
    GhostType ghost;

    initGhost(&ghost, chooseStartingRoom(&house.rooms));
    //printf("GHOST STARTING CLASS: %d ", ghost.class);

    cleanupHouse(&house);
    
    return 0;
}

