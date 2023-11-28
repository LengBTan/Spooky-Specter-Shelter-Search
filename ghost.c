#include "defs.h"

void initGhost(GhostType *ghost, RoomType *startingRoom){
    ghost->class = randomGhost();//randomly select a ghost type upon initialization
    ghost->currRoom = startingRoom;
    ghost->boredom = 0;
}

//random ghost actions