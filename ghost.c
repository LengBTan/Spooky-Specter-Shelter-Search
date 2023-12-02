#include "defs.h"

void initGhost(GhostType *ghost, RoomType *startingRoom){
    ghost->class = randomGhost();//randomly select a ghost type upon initialization
    ghost->currRoom = startingRoom;
    ghost->boredom = 0;
    l_ghostInit(ghost->class, startingRoom->name);
}

void* ghostAction(void *arg) {//choose a random ghost action
    GhostType *ghost = (GhostType*) arg;
    int rng;

    
    while(ghost->boredom <= BOREDOM_MAX){
        usleep(GHOST_WAIT);
        if(checkHunter(ghost)){//1 for hunter in room, 0 for no hunter in room
            rng = randInt(0,2);//hunter is in the room, only either leave evidence or do nothing
            ghost->boredom = 0;//if hunter is present, ghost boredom reset
        }
        else{
            rng = randInt(0,3);//hunter isnt in the room, perform any of the 3 actions
            ghost->boredom++;
        }

        if (rng == 0) {// ghost leaves evidence         
            leaveEvidence(ghost);
        }
        //RNG = 1 does nothing
        else if (rng == 2) {//ghost moves to adjacent room only if hunter is not in the same room 
            ghostMove(ghost);
        }
        
    }

    if (ghost->boredom >= BOREDOM_MAX) {
        //log the reason for leaving, LOG_BORED every time
        l_ghostExit(LOG_BORED);
    }
    
    return NULL;
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

    sem_wait(&(ghost->currRoom->mutex));
    ghost->currRoom->ghost = NULL;//set the current room's ghost pointer to null
    sem_post(&(ghost->currRoom->mutex));

    sem_wait(&(selectRoom->mutex));
    selectRoom->ghost = ghost;//set the destination room's ghost pointer to the ghost
    sem_post(&(selectRoom->mutex));
    
    ghost->currRoom = selectRoom;//set the ghost's current room to the chosen room
    l_ghostMove(selectRoom->name);//log its movement
}

char checkHunter(GhostType *ghost){
    sem_wait(&(ghost->currRoom->mutex));
    if(ghost->currRoom->hunterList.head != NULL){//there is a hunter in the room
        sem_post(&(ghost->currRoom->mutex));
        return 1;
    }
    else{
        sem_post(&(ghost->currRoom->mutex));
        return 0;
    }
}