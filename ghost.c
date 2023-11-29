#include "defs.h"

void initGhost(GhostType *ghost, RoomType *startingRoom){
    ghost->class = randomGhost();//randomly select a ghost type upon initialization
    ghost->currRoom = startingRoom;
    ghost->boredom = 0;
}

void ghostAction(GhostType *ghost) {

    //call a helper function to check if the ghost is in a room with a hunter to increment its boredom counter
    int rng = randInt(0,2);
    
    if (rng == 0) {
        // ghost leaves evidence
        addEvidence(&(ghost->currRoom->evList), ghost);
    }
    else if (rng == 1) {
        // ghost moves to adjacent room only if hunter is not there
        int roomNum = randInt(0, ghost->currRoom->adjRooms.size - 1); // randomly selects a room based on number of adjacent rooms
        int index = 0;
        RoomNodeType *currNode;
        RoomNodeType *nextNode;
        RoomType* selectRoom;
        currNode = ghost->currRoom->adjRooms.head;
        while(currNode != NULL && index <= rng){//traverse through the list adjacent rooms
            nextNode = currNode->next;
            selectRoom = currNode->data;
            currNode = nextNode;
            index++;
        }

        // checks if there is a hunter present in room, if no hunter, move to new room
        if (selectRoom->hunterList.head == NULL) {
            ghost->currRoom = selectRoom;
        }
        // ghost does something else if there is a hunter in the selected room
        else {
            int rng1 = randInt(0, 1);
            if (rng1 == 0) {
                addEvidence(&(ghost->currRoom->evList), ghost); // ghost adds evidence 
            }
            else {
                // do nothing
            }
        }
    }
    else if (rng == 2) {
        // do nothing
    }
// int checkHunter(RoomType *room) {

// }

}


//random ghost actions