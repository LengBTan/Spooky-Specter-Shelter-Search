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
    EvidenceListType evList;//linked list of evidence that ghost has left
    HunterListType hunterList;//collection of hunters in the room
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
    HunterListType HunterList;//collection of hunters, maybe just an array of hunters?
    RoomListType rooms;//linkedList of rooms
    EvidenceListType evList;//shared evidence list that hunters access
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
    EvidenceListType *evList;//pointer to the shared evidence collection
    int fear;//fear counter
    int boredom;//boredom counter
}HunterType;

typedef struct HunterNode{
    HunterType *data;
    struct HunterNode *next;
    struct HunterNode *prev;
}HunterNodeType;

typedef struct HunterList{
    HunterNodeType *head;
    HunterNodeType *tail;
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

//Room functions
void initRoom(char *name, RoomType *room);
void initRoomList(RoomListType *list);
void addRoom(RoomListType *list, RoomType *room);//add room to the room list
void cleanupRoomList(RoomListType *list);//used before the program quits
void connectRooms(RoomType *room1, RoomType *room2);//connect the 2nd room to the first room

//House functions
void initHouse(HouseType *house);
void cleanupHouse();//helper function to clean up the house before program ends?
void populateRooms(HouseType *house);//populate the rooms in the house

//Ghost functions
void initGhost(GhostType *ghost, RoomType *startingRoom);
void ghostAction();//helper function to choose a random action for the ghost to take
void leaveEvidence();//ghost action to leave evidence behind
void ghostMove();//ghost action to move to another room

//Hunter functions
void initHunter(char *name, HunterType *hunter, RoomType *startingRoom, EvidenceType evType, EvidenceListType *evList);
void initHunterList(HunterListType *list);
void addHunter(HunterListType *list, HunterType *hunter);//adds to the house's master list of hunters
void cleanupHunterList(HunterListType *list);
void hunterAction();

//Evidence functions
void initEvidence(EvidenceType evidence);//probably wont need?
void initEvidenceList(EvidenceListType *list);
void addEvidence(EvidenceListType *list, GhostType *ghost);
void cleanupEvidenceList(EvidenceListType *list);



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