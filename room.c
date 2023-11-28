#include "defs.h"

void initRoom(char *name, RoomType *room) {
    strcpy(room->name, name); //sets the room name

    initRoomList(&room->adjRooms);//initialize the roomList in room
    initEvidenceList(&room->evList);//initialize the evidence list in the room
    initHunterList(&room->hunterList);//collection of hunters in the room
    room->ghost = NULL;

    sem_init(&(room->mutex), 0, 1); //initializes the semaphore
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

void cleanupRoomList(RoomListType *list) {
    RoomNodeType *currNode = list->head;
    RoomNodeType *prevNode;

    while (currNode != NULL) {
        prevNode = currNode;
        currNode = currNode->next;
        //free(prevNode->data); //freeing the data? idk
        free(prevNode);
    }
}

void connectRooms(RoomType *room1, RoomType *room2){
    addRoom(&room1->adjRooms, room2);//add room2 to the roomList in room1
}