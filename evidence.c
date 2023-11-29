#include "defs.h"

//each ghost leaves 3 out of the 4 types of evidence

void initEvidenceList(EvidenceListType *list){
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    sem_init(&(list->mutex), 0, 1);//initialize the semaphore
}

void addEvidence(EvidenceListType *list, GhostType *ghost){
    EvidenceNodeType *newNode;
    newNode = malloc(sizeof(EvidenceNodeType));

    int rng = randInt(0,1);//evidence depends on the ghost type and is random
    switch (ghost->class){
    case POLTERGEIST://EMF, TEMPERATURE, FINGERPRINTS
        if(rng == 0){
            newNode->evidence = 0;//EMF
        }
        else if(rng == 1){
            newNode->evidence = 1;//TEMP
        }
        else if(rng == 2){
            newNode->evidence = 2;//FINGERPRINTS
        }
        break;
    case BANSHEE://EMF, TEMPERATURE, SOUND
        if(rng == 0){
            newNode->evidence = 0;//EMF
        }
        else if(rng == 1){
            newNode->evidence = 1;//TEMP
        }
        else if(rng == 2){
            newNode->evidence = 3;//SOUND
        }
        break;
    case BULLIES://EMF, FINGERPRINTS, and SOUND
        if(rng == 0){
            newNode->evidence = 0;//EMF
        }
        else if(rng == 1){
            newNode->evidence = 2;//FINGERPRINTS
        }
        else if(rng == 2){
            newNode->evidence = 3;//SOUND
        }
        break;
    case PHANTOM://TEMPERATURE, FINGERPRINTS, and SOUND
        if(rng == 0){
            newNode->evidence = 1;//TEMP
        }
        else if(rng == 1){
            newNode->evidence = 2;//FINGERPRINTS
        }
        else if(rng == 2){
            newNode->evidence = 3;//SOUND
        }
        break;
    }

    newNode->next = NULL;
    newNode->prev = list->tail;
    //add to the tail of the list
    if(list->head == NULL){//list is empty
        list->head = newNode;
    }
    else{//list isnt empty, just add to the tail
        list->tail->next = newNode;
    }
    list->tail = newNode;//point the tail to the newly added node
}

void cleanupEvidenceList(EvidenceListType *list){
    EvidenceNodeType *currNode = list->head;
    EvidenceNodeType *prevNode;

    while (currNode != NULL) {//traverse through the list until it gets to the end of the list
        prevNode = currNode;//set the previous node to the current node
        currNode = currNode->next;//set the current node to the next node
        free(prevNode);//free the prevNode
    }
}