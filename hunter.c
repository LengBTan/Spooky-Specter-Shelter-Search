#include "defs.h"

/*Function: initHunter()
  Description: initializes all fields for a HunterType

  Input: char *name - string of the hunter's name
  Output: HunterType *hunter - pointer to the ghost
  Input: RoomType *startingRoom - pointer to the starting room
  Input: EvidenceType evType - evidenceType that the hunter is assigned
  Input: EvidenceListType *evList - pointer to the shared evidence list
  Input: HouseType *house - pointer to the House

  Returns: void
*/
void initHunter(char *name, HunterType *hunter, RoomType *startingRoom, EvidenceType evType, EvidenceListType *evList, HouseType *house) {
    strcpy(hunter->name, name);

    hunter->currRoom = startingRoom;
    hunter->evType = evType;
    hunter->evList = evList;
    hunter->fear = 0;
    hunter->boredom = 0;

    addHunter(&startingRoom->hunterList, hunter); // add the hunter to the starting room list
    addHunter(&house->hunterList, hunter); // add hunter to the house 
    
    l_hunterInit(name, evType);
}

/*Function: initHunterList()
  Description: initializes the hunterList's field

  Output: HunterListType *list - pointer to a hunterList
  Returns: void
*/
void initHunterList(HunterListType *list) {
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
}

/*Function: addHunter()
  Description: add a hunter to the end of the HunterList

  Output: HunterListType *list - pointer to a hunterList
  Input: HunterType *hunter - pointer to a hunter

  Returns: void
*/
void addHunter(HunterListType *list, HunterType *hunter) {
    sem_wait(&(hunter->currRoom->mutex));
    HunterNodeType *newNode;
    newNode = malloc(sizeof(HunterNodeType));

    newNode->data = hunter;
    newNode->next = NULL;

    if (list->head == NULL) {
        list->head = newNode; // if the list is empty, the newNode is the head
    }
    else {
        list->tail->next = newNode; // if not empty, update the tail
    }

    list->tail = newNode;
    list->size++;
    sem_post(&(hunter->currRoom->mutex));
}

/*Function: cleanupHunterList()
  Description: deallocates all the hunter nodes in a HunterList

  Output: HunterListType *list - pointer to a hunterList

  Returns: void
*/
void cleanupHunterList(HunterListType *list) {
    HunterNodeType *currNode = list->head;
    HunterNodeType *prevNode;

    while (currNode != NULL) { // traverse through the list until it gets to the end of the list
        prevNode = currNode;
        currNode = currNode->next;
        free(prevNode);
    }
}

/*Function: removeHunter()
  Description: deallocates a hunter node in a HunterList

  Output: HunterListType *list - pointer to a hunterList
  Input: HunterType *hunter - pointer to the hunter

  Returns: void
*/
void removeHunter(HunterListType *list, HunterType *hunter){
    sem_wait(&(hunter->currRoom->mutex));

    HunterNodeType *currNode = list->head;
    HunterNodeType *prevNode = NULL;

    while (currNode != NULL) {
        if(currNode->data == hunter){ // if the addresses of the hunters match
            break; // we found the hunter to remove from the room
        }
        // traverse thru the list
        prevNode = currNode;
        currNode = currNode->next;
    }

    if (prevNode == NULL) { // hunter was at the head of the list (only 1 hunter in the room)
        list->head = currNode->next;
        if (currNode->next == NULL) { // the current node is the tail of the list
            list->tail = NULL; // set the tail to null
        }
    }
    else {
        prevNode->next = currNode->next;
        if (currNode->next == NULL) { // the current node is the tail of the list
            list->tail = prevNode;
        }
    }
    
    free(currNode); // free the hunter node
    sem_post(&(hunter->currRoom->mutex));
}

/*Function: hunterAction()
  Description: the actions the hunter takes throughout the game, performed by
               the thread

  Output: void *arg - *arg typecasted into a HunterType pointer

  Returns: void*, a void pointer
*/
void* hunterAction(void *arg) {
    HunterType *hunter = (HunterType*) arg;
    char foundEv = C_FALSE;
    // check if the ghost is in the same room as the hunter
    while(1) {
        usleep(HUNTER_WAIT);
        
        if (checkGhost(hunter)) { // ghost in room, increase fear and set boredom to 0
            hunter->fear++;
            hunter->boredom = 0;
        }
        else {
            hunter->boredom++;
        }

        if (hunter->fear >= FEAR_MAX) {
            l_hunterExit(hunter->name, 0);
            removeHunter(&hunter->currRoom->hunterList, hunter); // remove the hunter from the room
            break;
        }
        if (hunter->boredom >= BOREDOM_MAX) {
            removeHunter(&hunter->currRoom->hunterList, hunter); // remove the hunter from the room
            l_hunterExit(hunter->name, 1);
            break;
        }

        int rng;
        sem_wait(&hunter->currRoom->evList.mutex);
        if (hunter->currRoom->evList.head != NULL) { // check if evidence exists in room
            rng = randInt(0,3); // min = 0, max-1 = 2
        }
        else {
            rng = randInt(0,2); // min = 0, 1 = 1
        }
        sem_post(&hunter->currRoom->evList.mutex);

        if (foundEv == C_TRUE) {
            l_hunterExit(hunter->name, 2);
            removeHunter(&hunter->currRoom->hunterList, hunter);
            break;
        }
        
        if (rng == 0) { // move to a random connected room
            hunterMove(hunter);
        }
        else if (rng == 1){ // review evidence that is shared
            foundEv = hunterReview(hunter);
        }
        else if (rng == 2) { // collect evidence in a room if evidence is in it & equipment matches
            hunterCollect(hunter);
        }
    }
    return NULL;
}

/*Function: hunterMove()
  Description: moves the hunter to a randomly chosen room

  Output: HunterType *hunter - pointer to the hunter

  Returns: void
*/
void hunterMove(HunterType *hunter) {
    int roomNum = randInt(0, hunter->currRoom->adjRooms.size); // randomly selects a room based on number of adjacent rooms
    int index = 0;
    RoomNodeType *currNode;
    RoomNodeType *nextNode;
    RoomType* selectRoom;
    currNode = hunter->currRoom->adjRooms.head;
    
    while (currNode != NULL && index <= roomNum) { // traverse through the list adjacent rooms, stop when we get to the index of the room that was randomly chosen
        nextNode = currNode->next;
        selectRoom = currNode->data;
        currNode = nextNode;
        index++;
    }

    removeHunter(&hunter->currRoom->hunterList, hunter); // remove hunter from the current room's hunterlist (and free its memory)
    hunter->currRoom = selectRoom; // set the hunter's current room pointer to the randomly chosen room
    addHunter(&selectRoom->hunterList, hunter); // add hunter to the room it moved to
    l_hunterMove(hunter->name, selectRoom->name); // log the move
}

/*Function: hunterCollect()
  Description: handles the hunter's action of collecting evidence, by checking
               if the evidence in the room matches the evidence type that the
               hunter is able to collect, and adding it to its shared evidence
               list

  Output: HunterType *hunter - pointer to a hunterList

  Returns: void
*/
void hunterCollect(HunterType *hunter) {
    sem_wait(&hunter->currRoom->mutex); // wait/lock the current room
    sem_wait(&hunter->currRoom->evList.mutex); // wait/lock the evidence list since we are reading from it

    EvidenceNodeType *currNode;
    EvidenceNodeType *nextNode;
    currNode = hunter->currRoom->evList.head;
    while (currNode != NULL) {
        nextNode = currNode->next;
        if (currNode->evidence == hunter->evType) { // if evidence matches the hunters collection type
            break;
        }
        currNode = nextNode;
    }

    sem_post(&hunter->currRoom->evList.mutex); // finished reading the room's evidence list

    if (currNode == NULL) { // none of the evidence matches, since we traversed through the whole list 
        sem_post(&hunter->currRoom->mutex); // unlock the shared evidence list
        return;
    }
    sem_post(&hunter->currRoom->mutex); // unlock the shared evidence list
    addEvidence(hunter->evList, hunter->evType);
    l_hunterCollect(hunter->name, hunter->evType, hunter->currRoom->name); // logs the evidence collection
}

/*Function: hunterReview()
  Description: handles the hunter's action of reviewing evidence

  Input: HunterType *hunter - pointer to a hunterList

  Returns: void
*/
char hunterReview(HunterType *hunter) {
    int result = evidenceCheck(hunter->evList); // helper function in utils.c
    if (result == 0 || result == 1 || result == 2 || result == 3) { // evidence meets one of the ghost requirements
        l_hunterReview(hunter->name, 3); // review sufficient
        return C_TRUE;
    }
    else {
        l_hunterReview(hunter->name, 4); // review insufficient
        return C_FALSE;
    }
}

/*Function: checkGhost()
  Description: checks if the ghost is in the current room as the hunter

  Input: HunterType *hunter - pointer to a hunterList

  Returns: char, a byte if the ghost is or isn't in the room
*/
char checkGhost(HunterType *hunter) { // check if ghost is in the room
    sem_wait(&(hunter->currRoom->mutex));
        if (hunter->currRoom->ghost == NULL) {
            sem_post(&(hunter->currRoom->mutex));
            return C_FALSE;
        }
        else {
            sem_post(&(hunter->currRoom->mutex));
            return C_TRUE;
        }
}