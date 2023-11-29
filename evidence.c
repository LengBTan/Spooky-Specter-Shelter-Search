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

    //newNode->evidence;

    //evidence depends on the ghost type
    //type of evidence left is random
    int rng = randInt(0,1);
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
    
}