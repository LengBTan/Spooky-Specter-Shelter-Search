#include "defs.h"

/*Function: initEvidenceList()
  Description: initializes all fields for a evidence list

  Output: EvidenceListType *list - pointer to the evidence list

  Returns: void
*/
void initEvidenceList(EvidenceListType *list){
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    sem_init(&(list->mutex), 0, 1); // initialize the semaphore
}

/*Function: leaveEvidence()
  Description: allows a ghost to leave a random piece of evidence in their current room depending on their ghost type

  Input: GhostType *ghost - pointer to the ghost

  Returns: void
*/
void leaveEvidence(GhostType *ghost) { // ghost leaves evidence behind
    int rng = randInt(0,3); // evidence depends on the ghost type and is random
    int evidence;
    switch (ghost->class){
    case POLTERGEIST: // EMF, TEMPERATURE, FINGERPRINTS
        if(rng == 0){
            evidence = 0; // EMF
        }
        else if(rng == 1){
            evidence = 1; // TEMP
        }
        else if(rng == 2){
            evidence = 2; // FINGERPRINTS
        }
        break;
    case BANSHEE: // EMF, TEMPERATURE, SOUND
        if(rng == 0){
            evidence = 0; // EMF
        }
        else if(rng == 1){
            evidence = 1; // TEMP
        }
        else if(rng == 2){
            evidence = 3; // SOUND
        }
        break;
    case BULLIES: // EMF, FINGERPRINTS, and SOUND
        if(rng == 0){
            evidence = 0; // EMF
        }
        else if(rng == 1){
            evidence = 2; // FINGERPRINTS
        }
        else if(rng == 2){
            evidence = 3; // SOUND
        }
        break;
    case PHANTOM: // TEMPERATURE, FINGERPRINTS, and SOUND
        if(rng == 0){
            evidence = 1; // TEMP
        }
        else if(rng == 1){
            evidence = 2; // FINGERPRINTS
        }
        else if(rng == 2){
            evidence = 3; // SOUND
        }
        break;

    default: // since GHOST_COUNT, GH_UNKNOWN arent ever cases
        break;
    }
    addEvidence(&ghost->currRoom->evList, evidence);
    l_ghostEvidence(evidence, ghost->currRoom->name);
}

/*Function: addEvidence()
  Description: adds a certain type of evidence into the evidence list 

  Input: EvidenceListType *list - pointer to the evidence list
  Input: EvidenceType ev - the type of evidence

  Returns: void
*/
void addEvidence(EvidenceListType *list, EvidenceType ev){
    sem_wait(&list->mutex);
    EvidenceNodeType *newNode;
    newNode = malloc(sizeof(EvidenceNodeType));
    newNode->evidence = ev;
    newNode->next = NULL;

    // add to the tail of the list
    if(list->head == NULL){ // list is empty
        list->head = newNode;
    }
    else{ // list isnt empty, just add to the tail
        list->tail->next = newNode;
    }
    list->tail = newNode; // point the tail to the newly added node
    sem_post(&list->mutex);
}

/*Function: cleanupEvidenceList()
  Description: deallocates all the evidence nodes in a evidence list

  Output: EvidenceListType *list - pointer to a evidence list

  Returns: void
*/
void cleanupEvidenceList(EvidenceListType *list){
    EvidenceNodeType *currNode = list->head;
    EvidenceNodeType *prevNode;

    while (currNode != NULL) { // traverse through the list until it gets to the end of the list
        prevNode = currNode; // set the previous node to the current node
        currNode = currNode->next; // set the current node to the next node
        free(prevNode); // free the prevNode
    }
}