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
    printf("RANDOMLY GENERATED ROOM: %s \n", hunter->currRoom->name);
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

    if(prevNode == NULL){//hunter was at the head of the list
        list->head = currNode->next;
    }
    else{
        prevNode->next = currNode->next;
    }

    free(currNode);//free the hunter node
    
}

void* hunterAction(void *arg){
    HunterType *hunter = (HunterType*) arg;

    //check if the ghost is in the same room as the hunter
    //while(hunter->fear <= FEAR_MAX && hunter->boredom <= BOREDOM_MAX){
    while(1){
        if(hunter->currRoom->ghost == NULL){//current room's ghost pointer is null, no ghost
            hunter->fear++;
            hunter->boredom = 0;
        }
        else{
            hunter->boredom++;
        }

        int rng;
        if (hunter->currRoom->evList.head != NULL) {//check if evidence exists in room
            rng = randInt(0,3);//min = 0, max-1 = 2
        }
        else {
            rng = randInt(0,2);//min = 0, 1 = 1
        }

        if(hunter->fear >= FEAR_MAX){
            l_hunterExit(hunter->name, 0);
            // sem_wait(&(hunter->currRoom->mutex));
            // printf("            LOCKED IN EXIT\n");
            // printf("START REMOVING HUNTER FROM CURRENT ROOM IN HUNTERACTION\n");
            
            // printf("FINISHED REMOVING HUNTER FROM CURRENT ROOM IN HUNTERACTION\n");
            // printf("HUNTER's ROOM BEFORE EXITING: %s\n",hunter->currRoom->name);

            // sem_post(&(hunter->currRoom->mutex));
            printf("            UNLOCKED IN EXIT\n");
            break;
        }
        if (hunter->boredom >= BOREDOM_MAX) {
            removeHunter(&hunter->currRoom->hunterList, hunter);//remove the hunter from the room
            l_hunterExit(hunter->name, 1);
            break;
        }
        

        // sem_wait(&(hunter->currRoom->mutex));
        // printf("                LOCKED IN MAIN\n");
        if(rng == 0){//move to a random connected room
            hunterMove(hunter);
        }
        else if(rng == 1){//review evidence that is shared
            hunterReview(hunter);
        }
        else if(rng == 2){//collect evidence in a room if evidence is in it & equipment matches
            // sem_wait(&hunter->evList->mutex);//lock the shared list, since this hunter is adding to the evidence list
            //hunterCollect(hunter);
            // sem_post(&hunter->evList->mutex);//unlock the shared evidence list
        }
        // sem_post(&(hunter->currRoom->mutex));
        // printf("                UNLOCKED IN MAIN\n");
        usleep(HUNTER_WAIT);
    }
removeHunter(&hunter->currRoom->hunterList, hunter);//remove the hunter from the room
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
    
    l_hunterMove(hunter->name, selectRoom->name);//log the move
    
    
    
    
    //sem_wait(&(selectRoom->mutex));
    printf("START REMOVING HUNTER FROM CURRENT ROOM\n");
    removeHunter(&hunter->currRoom->hunterList, hunter);//remove hunter from the current room's hunterlist (and free its memory)
    printf("FINISHED REMOVING HUNTER FROM CURRENT ROOM\n");
    
    hunter->currRoom = selectRoom;//set the hunter's current room pointer to the randomly chosen room
    printf("RANDOMLY GENERATED ROOM: %s \n", hunter->currRoom->name);
    //sem_wait(&(selectRoom->mutex));
    // printf("    LOCKED THE NEXT ROOM MUTEX FOR HUNTER: %s\n", hunter->name);

    addHunter(&selectRoom->hunterList, hunter);//add hunter to the room it moved to
    printf("FINISHED ADDING HUNTER TO CHOSEN ROOM\n");
    //sem_post(&(currentRoom->mutex));
    //sem_post(&(selectRoom->mutex));
    // printf("    UNLOCKED THE NEXT ROOM MUTEX FOR HUNTER: %s\n", hunter->name);
    
    
    
}

void hunterCollect(HunterType *hunter){
    //something with the mutex/threading happens in this function??
    
    EvidenceNodeType *currNode;
    EvidenceNodeType *nextNode;
    currNode = hunter->currRoom->evList.head;
    while(currNode != NULL) {
        nextNode = currNode->next;
        if (currNode->evidence == hunter->evType) {//if evidence matches the hunters collection type
            l_hunterCollect(hunter->name, currNode->evidence, hunter->currRoom->name);//logs the evidence collection
            if (hunter->evList->head == NULL) {
                hunter->evList->head = currNode; //if the list is empty, the new evidence is the head
            }
            else {
                hunter->evList->tail->next = currNode; //update the tail node
            }
            hunter->evList->tail = currNode; // new evidence is set as the tail
            hunter->evList->size++;
            return;
        }
        currNode = nextNode;
    }
    //hunter evidence did not match
}

void hunterReview(HunterType *hunter){
    int result = evidenceCheck(hunter->evList);// helper function in utils.c

    if (result == 1) {// review successful
        l_hunterReview(hunter->name, 3);// review sufficient
        l_hunterExit(hunter->name, 2);// exit because sufficient evidence
    }
    else {
        l_hunterReview(hunter->name, 4);// review insufficient
    }
}