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
    

    //evidence depends on the ghost type
    //type of evidence left is random
    // switch (expression)
    // {
    // case /* constant-expression */:
    //     /* code */
    //     break;
    
    // default:
    //     break;
    // }

}