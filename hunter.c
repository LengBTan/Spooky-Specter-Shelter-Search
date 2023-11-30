#include "defs.h"

void initHunter(char *name, HunterType *hunter, RoomType *startingRoom, EvidenceType evType, EvidenceListType *evList) {
    strcpy(hunter->name, name);

    hunter->currRoom = startingRoom;
    hunter->evType = evType;
    hunter->evList = evList;
    hunter->fear = 0;
    hunter->boredom = 0;

    addHunter(&startingRoom->hunterList, hunter);//aadd the hunter to the starting room list
    l_hunterInit(name, evType);
}

void initHunterList(HunterListType *list) {
    list->head = NULL;
    list->tail = NULL;
}

void addHunter(HunterListType *list, HunterType *hunter) {
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

void hunterAction(HunterType *hunter, GhostType *ghost){
    //check if the ghost is in the same room as the hunter
    if(hunter->currRoom == ghost->currRoom){
        hunter->fear++;
        hunter->boredom = 0;
    }
    else{
        hunter->boredom++;
    }

    if(hunter->fear >= FEAR_MAX || hunter->boredom >= BOREDOM_MAX){
        //exit the thread
        //pthread_exit()
        //log the reason, either LOG_BORED, LOG_FEAR, or LOG_EVIDENCE
        //l_hunterExit(char* name, enum LoggerDetails reason);
        return;
    }

    int rng;
    rng = randInt(0,3);//min = 0, max-1 = 2

    if(rng == 0){//move to a random connected room
        hunterMove(hunter);
    }
    else if(rng == 1){//collect evidence in a room if evidence is in it & equipment matches
        hunterCollect(hunter);
    }
    else if(rng == 2){//review evidence that is shared
        hunterReview(hunter);
    }
}

void hunterMove(HunterType *hunter){
    //log the move
    //l_hunterMove(char* name, char* room);
}

void hunterCollect(HunterType *hunter){
    //log the collect
    //l_hunterCollect(char* name, enum EvidenceType evidence, char* room);
}

void hunterReview(HunterType *hunter){
    //log the review
    //l_hunterReview(char* name, enum LoggerDetails reviewResult);
}