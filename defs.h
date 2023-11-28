#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_STR         64
#define MAX_RUNS        50
#define BOREDOM_MAX     100
#define C_TRUE          1
#define C_FALSE         0
#define HUNTER_WAIT     5000
#define GHOST_WAIT      600
#define NUM_HUNTERS     4
#define FEAR_MAX        10
#define LOGGING         C_TRUE

typedef enum EvidenceType EvidenceType;
typedef enum GhostClass GhostClass;

enum EvidenceType { EMF, TEMPERATURE, FINGERPRINTS, SOUND, EV_COUNT, EV_UNKNOWN };
enum GhostClass { POLTERGEIST, BANSHEE, BULLIES, PHANTOM, GHOST_COUNT, GH_UNKNOWN };
enum LoggerDetails { LOG_FEAR, LOG_BORED, LOG_EVIDENCE, LOG_SUFFICIENT, LOG_INSUFFICIENT, LOG_UNKNOWN };

//structs
typedef struct Room{
    char name[MAX_STR];//name of the room
    RoomListType adjRooms;//linked list of adjacent rooms it is connected to
    //linked list of evidence that ghost has left
    //collection of hunters in the room (probably just use an array since # of hunters dont change during runtime?)

    GhostType *ghost;//pointer to the ghost that is in the room
    sem_t mutex;//semaphore to prevent the room from being modified by multiple threads
}RoomType;

typedef struct RoomNode{//roomNodes for the RoomList
    RoomType *data;//points to the room
    struct RoomNode *next;//points to the next node
    struct RoomNode *prev;//points to the previous node
}RoomNodeType;

typedef struct RoomList{//doubly linked list, store the head and tail
    int size;//stores the size of the room list
    RoomNodeType *head;
    RoomNodeType *tail;
}RoomListType;

typedef struct House{
    HunterListType hunterList;//collection of hunters, maybe just an array of hunters?
    RoomListType rooms;//linkedList of rooms
    //shared evidence list that hunters access
}HouseType;

typedef struct Ghost{
    GhostClass class;//the class of the ghost
    RoomType *currRoom;//pointer to the current room the ghost is in
    int boredom;//boredom counter, initialize it to 0
}GhostType;

typedef struct Hunter{
    RoomType *currRoom;//pointer to the current room the hunter is in
    EvidenceType evType;//type of evidence that the hunter is able to scan
    char name[MAX_STR];//name of the hunter
    EvidenceListType *sharedEvidence;//pointer to the shared evidence collection
    int fear;//fear counter
    int boredom;//boredom counter
}HunterType;

typedef struct{//??
    HunterType *elements;
    int size;
}HunterListType;

typedef struct EvidenceList{//Evidence collection
    int size;//stores the size of the EvidenceList
    sem_t mutex;//mutex for the collection
    EvidenceNodeType *head;//pointer to the head of the linked list
    EvidenceNodeType *tail;//pointer to the tail
}EvidenceListType;

typedef struct EvidenceNode{
    EvidenceType evidence;//data that the node is storing
    struct EvidenceNode *next;//point to the next node
    struct EvidenceNode *prev;//point to the previous node
}EvidenceNodeType;

// Helper Utilies
int randInt(int,int);        // Pseudo-random number generator function
float randFloat(float, float);  // Pseudo-random float generator function
enum GhostClass randomGhost();  // Return a randomly selected a ghost type
void ghostToString(enum GhostClass, char*); // Convert a ghost type to a string, stored in output paremeter
void evidenceToString(enum EvidenceType, char*); // Convert an evidence type to a string, stored in output parameter

// Logging Utilities
void l_hunterInit(char* name, enum EvidenceType equipment);
void l_hunterMove(char* name, char* room);
void l_hunterReview(char* name, enum LoggerDetails reviewResult);
void l_hunterCollect(char* name, enum EvidenceType evidence, char* room);
void l_hunterExit(char* name, enum LoggerDetails reason);
void l_ghostInit(enum GhostClass type, char* room);
void l_ghostMove(char* room);
void l_ghostEvidence(enum EvidenceType evidence, char* room);
void l_ghostExit(enum LoggerDetails reason);