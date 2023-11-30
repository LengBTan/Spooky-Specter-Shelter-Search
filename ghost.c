#include "defs.h"

void initGhost(GhostType *ghost, RoomType *startingRoom){
    ghost->class = randomGhost();//randomly select a ghost type upon initialization
    ghost->currRoom = startingRoom;
    ghost->boredom = 0;
    l_ghostInit(ghost->class, startingRoom->name);
}

void ghostAction(GhostType *ghost) {//choose a random ghost action
    int rng;
    if(checkHunter(ghost)){//1 for hunter in room, 0 for no hunter in room
        rng = randInt(0,1);//hunter is in the room, only either leave evidence or do nothing
        ghost->boredom = 0;//if hunter is present, ghost boredom reset
    }
    else{
        rng = randInt(0,3);//hunter isnt in the room, perform any of the 3 actions
        ghost->boredom++;
    }

    if (ghost->boredom >= BOREDOM_MAX) {
        // something happens here with the thread
        //pthread_exit()
        //log the reason for leaving, LOG_BORED every time
        //l_ghostExit(enum LoggerDetails reason);
        return;
    }

    if (rng == 0) {// ghost leaves evidence
        leaveEvidence(&(ghost->currRoom->evList), ghost);
    }
    //RNG = 1 does nothing
    else if (rng == 2) {//ghost moves to adjacent room only if hunter is not in the same room 
        ghostMove(ghost);
    }
}

void ghostMove(GhostType *ghost){
    int roomNum = randInt(0, ghost->currRoom->adjRooms.size); // randomly selects a room based on number of adjacent rooms
    int index = 0;
    RoomNodeType *currNode;
    RoomNodeType *nextNode;
    RoomType* selectRoom;
    currNode = ghost->currRoom->adjRooms.head;
    while(currNode != NULL && index <= roomNum){//traverse through the list adjacent rooms, stop when we get to the index of the room that was randomly chosen
        nextNode = currNode->next;
        selectRoom = currNode->data;
        currNode = nextNode;
        index++;
    }

    ghost->currRoom = selectRoom;
    l_ghostMove(selectRoom->name);
}

char checkHunter(GhostType *ghost){
    if(ghost->currRoom->hunterList.head != NULL){//there is a hunter in the room
        return 1;
    }
    else{
        return 0;
    }
}