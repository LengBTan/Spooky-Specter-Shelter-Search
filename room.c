#include "defs.h"

/*Function: createRoom()
  Description: dynamically allocates memory for a room, and initializes all it's fields

  Input: char *name - pointer to a string name that is read
  Returns: RoomType*, a pointer to the room that was dynamically allocated
*/
RoomType* createRoom(char *name) {
    RoomType *room; // dynamically allocate the room
    room = malloc(sizeof(RoomType)); // dynamically allocate the room

    strcpy(room->name, name); // initialize the name
    initRoomList(&room->adjRooms); // initialize the roomList
    initEvidenceList(&room->evList); // initialize the evidenceList
    initHunterList(&room->hunterList); // initialize the hunterList
    room->ghost = NULL; // initialize the ghost pointer
    sem_init(&room->mutex, 0, 1); // initializes the semaphore
    return room; // return the pointer of this room
}

/*Function: initRoomList()
  Description: initializes the roomList's fields

  Output: RoomListType *list - pointer to a RoomList
  Returns: void
*/
void initRoomList(RoomListType *list) {
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
}

/*Function: addRoom()
  Description: Adds a Room to the end of a RoomList

  Output: RoomListType *list - pointer to a RoomList
  Input: RoomType *room - pointer to a Room

  Returns: void
*/
void addRoom(RoomListType *list, RoomType *room) {
    RoomNodeType *newNode;
    newNode = malloc(sizeof(RoomNodeType));

    newNode->data = room;
    newNode->next = NULL;

    if (list->head == NULL) {
        list->head = newNode; // if the list is empty, the newNode is the head
    }
    else {
        list->tail->next = newNode; // if not empty, update the tail
    }

    list->tail = newNode; 
    list->size++;
}

/*Function: cleanupRoomData()
  Description: deallocates all the rooms in a roomlist

  Output: RoomListType *list - pointer to a RoomList

  Returns: void
*/
void cleanupRoomData(RoomListType *list){
    RoomNodeType *currNode = list->head;
    RoomNodeType *prevNode;

    while (currNode != NULL) { // traverse through the list until it gets to the end of the list
        prevNode = currNode; // set the previous node to the current node
        currNode = currNode->next; // set the current node to the next node
        cleanupRoomList(&prevNode->data->adjRooms); // free the adjacent rooms in the room
        cleanupEvidenceList(&prevNode->data->evList); // free the EvidenceList in the room 
        free(prevNode->data); // free the data
    }
}

/*Function: cleanupRoomList()
  Description: deallocates the nodes in a Roomlist

  Output: RoomListType *list - pointer to a RoomList

  Returns: void
*/
void cleanupRoomList(RoomListType *list) {
    RoomNodeType *currNode = list->head;
    RoomNodeType *prevNode;

    while (currNode != NULL) { // traverse through the list until it gets to the end of the list
        prevNode = currNode; // set the previous node to the current node
        currNode = currNode->next; // set the current node to the next node
        free(prevNode); // free the prevNode
    }
}

/*Function: connectRooms()
  Description: connects 2 rooms together in both directions by
               adding each other to their adjRoom list

  Output: RoomType *room1 - pointer to the first Room to be connected
  Output: RoomType *room2 - pointer to the second Room to be connected

  Returns: void
*/
void connectRooms(RoomType *room1, RoomType *room2) {
    // rooms are 2 way connections
    addRoom(&room1->adjRooms, room2); // add room2 to the roomList in room1
    addRoom(&room2->adjRooms, room1); // add room1 to the roomList in room2
}

void printRoomAdj(RoomType *room) {
    RoomNodeType *newNode = room->adjRooms.head;
	RoomNodeType *prevNode;
    printf("Adjacent Rooms:\n");
	while (newNode != NULL) {
		prevNode = newNode;
		printf("    %s ", prevNode->data->name);
        printf("ROOM POINTER: %p\n", prevNode->data); // just to check its address in memory, no duplicate rooms
		newNode = newNode->next;
	}
}

void printRoomEv(RoomType *room) {
    EvidenceNodeType *newNode = room->evList.head;
	EvidenceNodeType *prevNode;
    printf("Evidence in Room:\n");
	while (newNode != NULL){
		prevNode = newNode;
        enum EvidenceType evidence = prevNode->evidence;

        if (evidence == 0) {
            printf("EMF\n");
        }
        else if (evidence == 1) {
            printf("TEMPERATURE\n");
        }
        else if (evidence == 2) {
            printf("FINGERPRINTS\n");
        }
        else if (evidence == 3) {
            printf("SOUND\n");
        }
		
		newNode = newNode->next;
	}
}

void printRoomHunter(RoomType *room) {
    HunterNodeType *newNode = room->hunterList.head;
	HunterNodeType *prevNode;
    printf("Hunters in Room:\n");
	while (newNode != NULL) {
		prevNode = newNode;
		printf("%s\n", prevNode->data->name);
		newNode = newNode->next;
	}
}

void printRoom(RoomType *room) {
    printf("Room: %s\n", room->name);
    printf("    ROOM POINTER: %p\n", room);// just to check its address in memory, no duplicate rooms
    printRoomAdj(room);
    printRoomEv(room);
    printRoomHunter(room);
    // insert a print ghost function here
    printf("            ghost pointer: %p \n",room->ghost);
    printf("-----------------\n");
}

void printRoomList(RoomListType *list) {
    RoomNodeType *newNode = list->head;
	RoomNodeType *prevNode;
	while (newNode != NULL) {
		prevNode = newNode;
		printRoom(prevNode->data);
		newNode = newNode->next;
	}
}

/*Function: chooseStartingRoom()
  Description: selects a random room for the ghost to start in, not including the hunter's van

  Output: RoomListType *list - pointer to a RoomList
  Output: GhostType *ghost - pointer to the ghost

  Returns: RoomType*, pointer to a randomly chosen room
*/
RoomType* chooseStartingRoom(RoomListType *list, GhostType *ghost) {
    int rng = randInt(1,NUM_ROOMS+1); // first room (room 0) is the van, NUM_ROOMS+1 since max is max-1
    int index = 0;
    
    RoomNodeType *currNode;
    RoomNodeType *nextNode;
    RoomType* startingRoom;
    currNode = list->head;

    while(currNode != NULL && index <= rng) { // traverse through the list of houses, up to the randomly chosen room's index
        nextNode = currNode->next;
        startingRoom = currNode->data;
        currNode = nextNode;
        index++;
    }

    startingRoom->ghost = ghost; // set the starting room's ghost pointer to the ghost
    return startingRoom;
}