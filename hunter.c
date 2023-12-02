#include "defs.h"

void initHunter(char *name, HunterType *hunter, RoomType *startingRoom, EvidenceType evType, EvidenceListType *evList) {
    strcpy(hunter->name, name);

    hunter->currRoom = startingRoom;
    hunter->evType = evType;
    hunter->evList = evList;
    hunter->fear = 0;
    hunter->boredom = 0;

    addHunter(&startingRoom->hunterList, hunter);//add the hunter to the starting room list
    l_hunterInit(name, evType);
}

void initHunterList(HunterListType *list) {
    list->head = NULL;
    list->tail = NULL;
}

void addHunter(HunterListType *list, HunterType *hunter) {
    //printf("RANDOMLY GENERATED ROOM: %s \n", hunter->currRoom->name);
    sem_wait(&(hunter->currRoom->mutex));
    HunterNodeType *newNode;
    newNode = malloc(sizeof(HunterNodeType));

    newNode->data = hunter;
    newNode->next = NULL;
    newNode->prev = list->tail;

    
    if (list->head == NULL) {
        list->head = newNode; //if the list is empty, the newNode is the head
    }
    else {
        list->tail->next = newNode; //if not empty, update the tail
    }

    list->tail = newNode;

    sem_post(&(hunter->currRoom->mutex));
}

void cleanupHunterList(HunterListType *list) {
    HunterNodeType *currNode = list->head;
    HunterNodeType *prevNode;

    while (currNode != NULL) {//traverse through the list until it gets to the end of the list
        prevNode = currNode;
        currNode = currNode->next;
        //free(prevNode->data); //freeing the data? idk
        free(prevNode);
    }
}

//remove hunter from the current room
void removeHunter(HunterListType *list, HunterType *hunter){
    sem_wait(&(hunter->currRoom->mutex));

    HunterNodeType *currNode = list->head;
    HunterNodeType *prevNode = NULL;

    while(currNode != NULL){
        if(currNode->data == hunter){//if the addresses of the hunters match
            break;//we found the hunter to remove from the room
        }
        //traverse thru the list
        prevNode = currNode;
        currNode = currNode->next;
    }

    if(prevNode == NULL){//hunter was at the head of the list (only 1 hunter in the room)
        list->head = currNode->next;

        if (currNode->next == NULL) {//the current node is the tail of the list
            list->tail = NULL;//set the tail to null
        }
    }
    else{
        prevNode->next = currNode->next;

        if (currNode->next == NULL) {//the current node is the tail of the list
            list->tail = prevNode;
        }
    }

    free(currNode);//free the hunter node
    sem_post(&(hunter->currRoom->mutex));
}

void* hunterAction(void *arg){
    HunterType *hunter = (HunterType*) arg;
    char foundEv = 0;
    //check if the ghost is in the same room as the hunter
    //while(hunter->fear <= FEAR_MAX && hunter->boredom <= BOREDOM_MAX){
    while(1){
        usleep(HUNTER_WAIT);
        
        if(checkGhost(hunter)){//ghost in room, increase fear and set boredom to 0
            hunter->fear++;
            hunter->boredom = 0;
        }
        else{
            hunter->boredom++;
        }

        int rng;

        sem_wait(&hunter->currRoom->evList.mutex);
        if (hunter->currRoom->evList.head != NULL) {//check if evidence exists in room
            rng = randInt(0,3);//min = 0, max-1 = 2
        }
        else {
            rng = randInt(0,2);//min = 0, 1 = 1
        }
        sem_post(&hunter->currRoom->evList.mutex);
        if(foundEv == 1){
            l_hunterExit(hunter->name, 2);
            removeHunter(&hunter->currRoom->hunterList, hunter);
            break;
        }

        if(hunter->fear >= FEAR_MAX){
            l_hunterExit(hunter->name, 0);
            removeHunter(&hunter->currRoom->hunterList, hunter);//remove the hunter from the room
            break;
        }
        if (hunter->boredom >= BOREDOM_MAX) {
            removeHunter(&hunter->currRoom->hunterList, hunter);//remove the hunter from the room
            l_hunterExit(hunter->name, 1);
            break;
        }
        
        if(rng == 0){//move to a random connected room
            hunterMove(hunter);
        }
        
        else if(rng == 1){//review evidence that is shared
            //sem_wait(&(hunter->evList->mutex));
            foundEv = hunterReview(hunter);
            //sem_post(&(hunter->evList->mutex));
        }
        else if(rng == 2){//collect evidence in a room if evidence is in it & equipment matches
            hunterCollect(hunter);
        }
        
    }
    
    return NULL;
}

void hunterMove(HunterType *hunter){
    int roomNum = randInt(0, hunter->currRoom->adjRooms.size); // randomly selects a room based on number of adjacent rooms
    int index = 0;
    RoomNodeType *currNode;
    RoomNodeType *nextNode;
    RoomType* selectRoom;
    //RoomType* currentRoom = hunter->currRoom;
    currNode = hunter->currRoom->adjRooms.head;

    
    while(currNode != NULL && index <= roomNum) {//traverse through the list adjacent rooms, stop when we get to the index of the room that was randomly chosen
        nextNode = currNode->next;
        selectRoom = currNode->data;
        currNode = nextNode;
        index++;
    }
    
    
    
    // printf("    LOCKED THE CURR ROOM MUTEX FOR HUNTER: %s\n", hunter->name);
    //printf("START REMOVING HUNTER FROM CURRENT ROOM\n");
    removeHunter(&hunter->currRoom->hunterList, hunter);//remove hunter from the current room's hunterlist (and free its memory)
    //printf("FINISHED REMOVING HUNTER FROM CURRENT ROOM\n");
    hunter->currRoom = selectRoom;//set the hunter's current room pointer to the randomly chosen room
    // printf("    UNLOCKED THE CURR ROOM MUTEX FOR HUNTER: %s\n", hunter->name);

    //printf("RANDOMLY GENERATED ROOM: %s \n", hunter->currRoom->name);

    // printf("    LOCKED THE NEXT ROOM MUTEX FOR HUNTER: %s\n", hunter->name);
    addHunter(&selectRoom->hunterList, hunter);//add hunter to the room it moved to
    //printf("FINISHED ADDING HUNTER TO CHOSEN ROOM\n");
    // printf("    UNLOCKED THE NEXT ROOM MUTEX FOR HUNTER: %s\n", hunter->name);
    
    l_hunterMove(hunter->name, selectRoom->name);//log the move
}

void hunterCollect(HunterType *hunter){
    sem_wait(&hunter->currRoom->mutex);//lock the shared list, since this hunter is adding to the evidence list
    // sem_wait(&hunter->currRoom->mutex);
    EvidenceNodeType *currNode;
    EvidenceNodeType *nextNode;
    currNode = hunter->currRoom->evList.head;
    while(currNode != NULL) {
        nextNode = currNode->next;
        if (currNode->evidence == hunter->evType) {//if evidence matches the hunters collection type
            break;
        }
        currNode = nextNode;
    }
    sem_post(&hunter->currRoom->mutex);//unlock the shared evidence list
    addEvidence(hunter->evList, hunter->evType);
    l_hunterCollect(hunter->name, hunter->evType, hunter->currRoom->name);//logs the evidence collection
    
    // sem_post(&hunter->currRoom->mutex);
    //hunter evidence did not match
}

char hunterReview(HunterType *hunter){
    //sem_wait(&hunter->evList->mutex);
    int result = evidenceCheck(hunter->evList);// helper function in utils.c
    // sem_post(&hunter->evList->mutex);
    if (result == 1) {// review successful
        l_hunterReview(hunter->name, 3);// review sufficient
        l_hunterExit(hunter->name, 2);// exit because sufficient evidence

        return 1;
    }
    else {
        l_hunterReview(hunter->name, 4);// review insufficient
        return 0;
    }
}

char checkGhost(HunterType *hunter){//check if ghost is in the room
    sem_wait(&(hunter->currRoom->mutex));
        if(hunter->currRoom->ghost == NULL){
            sem_post(&(hunter->currRoom->mutex));
            return 0;
        }
        else{
            sem_post(&(hunter->currRoom->mutex));
            return 1;
        }
        
}