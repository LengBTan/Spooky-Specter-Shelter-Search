#include "defs.h"

void initHunter(char *name, HunterType *hunter, RoomType *startingRoom, EvidenceType evType, EvidenceListType *evList) {
    strcpy(hunter->name, name);

    hunter->currRoom = startingRoom;
    hunter->evType = evType;
    hunter->evList = evList;
    hunter->fear = 0;
    hunter->boredom = 0;
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

    while (currNode != NULL) {
        prevNode = currNode;
        currNode = currNode->next;
        //free(prevNode->data); //freeing the data? idk
        free(prevNode);
    }
}