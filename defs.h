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
#define GHOST_WAIT      800
#define NUM_HUNTERS     4
#define FEAR_MAX        10
#define LOGGING         C_TRUE
#define NUM_ROOMS       12
#define NUM_THREADS     5

typedef enum EvidenceType EvidenceType;
typedef enum GhostClass GhostClass;

enum EvidenceType { EMF, TEMPERATURE, FINGERPRINTS, SOUND, EV_COUNT, EV_UNKNOWN };
enum GhostClass { POLTERGEIST, BANSHEE, BULLIES, PHANTOM, GHOST_COUNT, GH_UNKNOWN };
enum LoggerDetails { LOG_FEAR, LOG_BORED, LOG_EVIDENCE, LOG_SUFFICIENT, LOG_INSUFFICIENT, LOG_UNKNOWN };

typedef struct Room RoomType;
typedef struct RoomNode RoomNodeType;
typedef struct RoomList RoomListType;
typedef struct House HouseType;
typedef struct Ghost GhostType;
typedef struct Hunter HunterType;
typedef struct HunterNode HunterNodeType;
typedef struct HunterList HunterListType;
typedef struct EvidenceList EvidenceListType;
typedef struct EvidenceNode EvidenceNodeType;

// structs
struct EvidenceList{ // Evidence collection
    int size; // stores the size of the EvidenceList
    sem_t mutex; // mutex for the collection
    EvidenceNodeType *head; // pointer to the head of the linked list
    EvidenceNodeType *tail; // pointer to the tail
};

struct EvidenceNode{
    EvidenceType evidence; // data that the node is storing
    struct EvidenceNode *next; // point to the next node
};

struct Hunter{
    RoomType *currRoom; // pointer to the current room the hunter is in
    EvidenceType evType; // type of evidence that the hunter is able to scan
    char name[MAX_STR]; // name of the hunter
    EvidenceListType *evList; // pointer to the shared evidence collection
    int fear; // fear counter
    int boredom; // boredom counter
};

struct HunterNode{
    HunterType *data;
    struct HunterNode *next;
};

struct HunterList{
    int size;
    HunterNodeType *head;
    HunterNodeType *tail;
};

struct RoomNode{ // roomNodes for the RoomList
    RoomType *data; // points to the room
    RoomNodeType *next; // points to the next node
};

struct RoomList{ // doubly linked list, store the head and tail
    int size; // stores the size of the room list
    RoomNodeType *head;
    RoomNodeType *tail;
};

struct Room{
    char name[MAX_STR]; // name of the room
    RoomListType adjRooms; // linked list of adjacent rooms it is connected to
    EvidenceListType evList; // linked list of evidence that ghost has left
    HunterListType hunterList; // collection of hunters in the room
    GhostType *ghost; // pointer to the ghost that is in the room
    sem_t mutex; // semaphore to prevent the room from being modified by multiple threads
};

struct House{
    HunterListType hunterList; // collection of hunters, maybe just an array of hunters?
    RoomListType rooms; // linkedList of rooms
    EvidenceListType evList; // shared evidence list that hunters access
};

struct Ghost{
    GhostClass class; // the class of the ghost
    RoomType *currRoom; // pointer to the current room the ghost is in
    int boredom; // boredom counter, initialize it to 0
};




//Room functions
RoomType* createRoom(char *name);
void initRoomList(RoomListType *list);
void addRoom(RoomListType *list, RoomType *room); // add room to the room list
void connectRooms(RoomType *room1, RoomType *room2); // connect the 2nd room to the first room
RoomType* chooseStartingRoom(RoomListType *list, GhostType *ghost);

void cleanupRoomData(RoomListType *list); // cleanup the room data
void cleanupRoomList(RoomListType *list); // used before the program quits

// for debugging, remove later?
void printRoomAdj(RoomType *room);
void printRoomEv(RoomType *room);
void printRoomHunter(RoomType *room);
void printRoom(RoomType *room);
void printRoomList(RoomListType *list);

// House functions
void initHouse(HouseType *house);
void cleanupHouse(HouseType *house); // cleanup the house before the program ends
void populateRooms(HouseType *house); // populate the rooms in the house

// Ghost functions
void initGhost(GhostType *ghost, RoomType *startingRoom);
char checkHunter(GhostType *ghost); // helper function that checks if a hunter is in the room
void* ghostAction(void *arg); // helper function to choose a random action for the ghost to take
void ghostMove(GhostType *ghost); // ghost action to move to another room

// Hunter functions
void initHunter(char *name, HunterType *hunter, RoomType *startingRoom, EvidenceType evType, EvidenceListType *evList, HouseType *house);
void initHunterList(HunterListType *list);
void addHunter(HunterListType *list, HunterType *hunter); // adds to the house's master list of hunters
void removeHunter(HunterListType *list, HunterType *hunter);
void cleanupHunterList(HunterListType *list);
void* hunterAction(void *arg);
void hunterMove(HunterType *hunter); // move to random connected room
void hunterCollect(HunterType *hunter); // collect evidence in the room
char hunterReview(HunterType *hunter); // review evidence
char checkGhost(HunterType *hunter); // check if ghost is in the room

// Evidence functions
void initEvidenceList(EvidenceListType *list);
void addEvidence(EvidenceListType *list, EvidenceType ev); // for hunters to add to the list
void leaveEvidence(GhostType *ghost); // ghost action to leave evidence behind
void cleanupEvidenceList(EvidenceListType *list);



// Helper Utilies
int randInt(int,int);        // Pseudo-random number generator function
float randFloat(float, float);  // Pseudo-random float generator function
enum GhostClass randomGhost();  // Return a randomly selected a ghost type
void ghostToString(enum GhostClass, char*); // Convert a ghost type to a string, stored in output paremeter
void evidenceToString(enum EvidenceType, char*); // Convert an evidence type to a string, stored in output parameter

void hunterNameInput(char* name);// handle entering a name for a hunter
int evidenceCheck(EvidenceListType *list);// loops through the hunter's evidence list and checks if sufficient or not
void finalResult(HouseType *house, GhostType *ghost);// displays final results

// Logging Utilities
void l_hunterInit(char* name, enum EvidenceType equipment);//used
void l_hunterMove(char* name, char* room);
void l_hunterReview(char* name, enum LoggerDetails reviewResult);
void l_hunterCollect(char* name, enum EvidenceType evidence, char* room);
void l_hunterExit(char* name, enum LoggerDetails reason);
void l_ghostInit(enum GhostClass type, char* room);//used
void l_ghostMove(char* room);//used
void l_ghostEvidence(enum EvidenceType evidence, char* room);//used
void l_ghostExit(enum LoggerDetails reason);