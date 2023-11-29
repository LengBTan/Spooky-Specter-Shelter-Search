#include "defs.h"

void initRoom(char *name, RoomType **room) {
    strcpy((*room)->name, name); //sets the room name
    initRoomList(&(*room)->adjRooms);//initialize the roomList in room
    initEvidenceList(&(*room)->evList);//initialize the evidence list in the room
    initHunterList(&(*room)->hunterList);//collection of hunters in the room
    (*room)->ghost = NULL;

    sem_init(&((*room)->mutex), 0, 1); //initializes the semaphore
}

RoomType* createRoom(char *name){
    RoomType *room;//dynamically allocate the room
    room = malloc(sizeof(RoomType));//dynamically allocate the room

    //initialize its values
    strcpy(room->name, name);
    initRoomList(&room->adjRooms);
    initEvidenceList(&room->evList);
    initHunterList(&room->hunterList);

    return room;//return the pointer of this room
}

void initRoomList(RoomListType *list) {
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
}

void addRoom(RoomListType *list, RoomType *room) {
    RoomNodeType *newNode;
    newNode = malloc(sizeof(RoomNodeType));

    newNode->data = room;
    newNode->next = NULL;
    newNode->prev = list->tail;

    
    if (list->head == NULL) {
        list->head = newNode; //if the list is empty, the newNode is the head
    }
    else {
        list->tail->next = newNode; //if not empty, update the tail
    }

    list->tail = newNode; 
    list->size++;
}

void cleanupRoomData(RoomListType *list){
    RoomNodeType *currNode = list->head;
    RoomNodeType *prevNode;

    while (currNode != NULL) {//traverse through the list until it gets to the end of the list
        prevNode = currNode;//set the previous node to the current node
        currNode = currNode->next;//set the current node to the next node
        cleanupRoomAdj(prevNode->data);//free the adjacent rooms in the room
        cleanupEvidenceList(&prevNode->data->evList);//free the EvidenceList in the room 
        free(prevNode->data); //free the data
    }
}

void cleanupRoomAdj(RoomType *room){
    RoomNodeType *currNode;
    RoomNodeType *nextNode;

    currNode = room->adjRooms.head;
    while(currNode != NULL){
        nextNode = currNode->next;
        free(currNode);//free the node
        currNode = nextNode;
    }
}

void cleanupRoomList(RoomListType *list) {
    RoomNodeType *currNode = list->head;
    RoomNodeType *prevNode;

    while (currNode != NULL) {//traverse through the list until it gets to the end of the list
        prevNode = currNode;//set the previous node to the current node
        currNode = currNode->next;//set the current node to the next node
        free(prevNode);//free the prevNode
    }
}



void connectRooms(RoomType *room1, RoomType *room2){
    //rooms are 2 way connections
    addRoom(&room1->adjRooms, room2);//add room2 to the roomList in room1
    addRoom(&room2->adjRooms, room1);//add room1 to the roomList in room2
}

void printRoomAdj(RoomType *room) {
    RoomNodeType *newNode = room->adjRooms.head;
	RoomNodeType *prev;
    printf("Adjacent Rooms:\n");
	while (newNode != NULL){
		prev = newNode;
		printf("    %s ", prev->data->name);
        printf("ROOM POINTER: %p\n", prev->data);//just to check its address in memory, no duplicate rooms
		newNode = newNode->next;
	}
}

void printRoomEv(RoomType *room) {
    EvidenceNodeType *newNode = room->evList.head;
	EvidenceNodeType *prev;
    printf("Evidence in Room:\n");
	while (newNode != NULL){
		prev = newNode;
        enum EvidenceType evidence = prev->evidence;

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
	HunterNodeType *prev;
    printf("Hunters in Room:\n");
	while (newNode != NULL){
		prev = newNode;
		printf("%s\n", prev->data->name);
		newNode = newNode->next;
	}
}



void printRoom(RoomType *room){
    printf("    Room: %s\n", room->name);
    printf("ROOM POINTER: %p\n", room);//just to check its address in memory, no duplicate rooms
    printRoomAdj(room);
    printRoomEv(room);
    printRoomHunter(room);
    //insert a print ghost function here
    printf("-----------------\n");
}

void printRoomList(RoomListType *list){
    RoomNodeType *newNode = list->head;
	RoomNodeType *prev;
	while (newNode != NULL){
		prev = newNode;
		printRoom(prev->data);
		newNode = newNode->next;
	}
}

RoomType* chooseStartingRoom(RoomListType *list){
    int rng = randInt(1,NUM_ROOMS);//first room (room 0) is the van
    int index = 0;
    RoomNodeType *currNode;
    RoomNodeType *nextNode;
    RoomType* startingRoom;
    currNode = list->head;
    while(currNode != NULL && index <= rng){//traverse through the list of houses
        nextNode = currNode->next;
        startingRoom = currNode->data;
        currNode = nextNode;
        index++;
    }

    return startingRoom;
}