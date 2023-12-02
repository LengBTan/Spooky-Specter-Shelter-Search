#include "defs.h"

/*
    Returns a pseudo randomly generated number, in the range min to (max - 1), inclusively
        in:   lower end of the range of the generated number
        in:   upper end of the range of the generated number
    return:   randomly generated integer in the range [0, max-1) 
*/
int randInt(int min, int max)
{
    return (int) randFloat(min, max);
}

/*
    Returns a pseudo randomly generated floating point number.
    A few tricks to make this thread safe, just to reduce any chance of issues using random
        in:   lower end of the range of the generated number
        in:   upper end of the range of the generated number
    return:   randomly generated floating point number in the range [min, max)
*/
float randFloat(float min, float max) {
    static __thread unsigned int seed = 0;
    if (seed == 0) {
        seed = (unsigned int)time(NULL) ^ (unsigned int)pthread_self();
    }

    float random = ((float) rand_r(&seed)) / (float) RAND_MAX;
    float diff = max - min;
    float r = random * diff;
    return min + r;
}

/* 
    Returns a random enum GhostClass.
*/
enum GhostClass randomGhost() {
    return (enum GhostClass) randInt(0, GHOST_COUNT);
}

/*
    Returns the string representation of the given enum EvidenceType.
        in: type - the enum EvidenceType to convert
        out: str - the string representation of the given enum EvidenceType, minimum 16 characters
*/
void evidenceToString(enum EvidenceType type, char* str) {
    switch (type) {
        case EMF:
            strcpy(str, "EMF");
            break;
        case TEMPERATURE:
            strcpy(str, "TEMPERATURE");
            break;
        case FINGERPRINTS:
            strcpy(str, "FINGERPRINTS");
            break;
        case SOUND:
            strcpy(str, "SOUND");
            break;
        default:
            strcpy(str, "UNKNOWN");
            break;
    }
}

/* 
    Returns the string representation of the given enum GhostClass.
        in: ghost - the enum GhostClass to convert
        out: buffer - the string representation of the given enum GhostClass, minimum 16 characters
*/
void ghostToString(enum GhostClass ghost, char* buffer) {
    switch(ghost) {
        case BANSHEE:
            strcpy(buffer, "Banshee");
            break;
        case BULLIES:
            strcpy(buffer, "Bullies");
            break;
        case PHANTOM:
            strcpy(buffer, "Phantom");
            break;
        case POLTERGEIST:
            strcpy(buffer, "Poltergeist");
            break;
        default:
            strcpy(buffer, "Unknown");
            break;
        
    }
}

void hunterNameInput(char* name) {
    printf("Please enter your hunter's name: ");
    fgets(name, MAX_STR, stdin);
    name[strlen(name)-1] = 0;//remove the nextline character 
}

int evidenceCheck(EvidenceListType *list) {
    sem_wait(&list->mutex);
    char EMF = 0;
    char temp = 0;
    char fingerprints = 0;
    char sound = 0;

    EvidenceNodeType *currNode;
    EvidenceNodeType *nextNode;
    currNode = list->head;
    while(currNode != NULL) { //loop through the evidence list 
        nextNode = currNode->next; 
        if (currNode->evidence == 0) {
            EMF = 1;
        }
        else if (currNode->evidence == 1) {
            temp = 1;
        }
        else if (currNode->evidence == 2) {
            fingerprints = 1;
        }
        else if (currNode->evidence == 3) {
            sound = 1;
        }
        currNode = nextNode;
    }
    sem_post(&list->mutex);
    if (EMF == 1 && temp == 1 && fingerprints == 1) {// ghost is a poltergeist
        return 1;
    }
    else if (EMF == 1 && temp == 1 && sound == 1) {// ghost is a banshee
        return 1;
    }
    else if (EMF == 1 && fingerprints == 1 && sound == 1) {// ghost is bullies
        return 1;
    }
    else if (temp == 1 && fingerprints == 1 && sound == 1) {// ghost is a phantom
        return 1;
    }
    else {
        return 0;
    }
}

void finalResult(HouseType *house, GhostType *ghost) {
    printf("=======================================\n");
    printf("All done! Let's tally the results...\n");
    printf("=======================================\n");
    //below prints out the hunters that ran away/too bored
    int huntersLost = 0;
    HunterNodeType *currNode;
    HunterNodeType *nextNode;
    currNode = house->hunterList.head;

    while(currNode != NULL) { //loop through the hunter list 
        nextNode = currNode->next; 
        if (currNode->data->fear >= FEAR_MAX) {
            printf("%s has run away in fear!\n", currNode->data->name);

            huntersLost++;
        }
        else if (currNode->data->boredom >= BOREDOM_MAX) {
            printf("%s has gotten too bored to continue!\n", currNode->data->name);
            huntersLost++;
        }
        currNode = nextNode;
    }
    printf("---------------------------------------\n");
    //if ghost wins (when all hunters run away in fear or too bored):
    char ghostName[MAX_STR];
    ghostToString(ghost->class, ghostName);
    if (huntersLost >= NUM_HUNTERS) {
        printf("All hunters have left the house from fear or boredom!\n");
        printf("The ghost has won the game!\n");
        printf("Using the evidence they found, they incorrectly determined that the ghost is unknown\n");
        printf("The ghost was actually a %s\n", ghostName);
    }

    //if hunters win (only when 3 evidence was collected AND at least one hunter reviewed it successfully and left the house safely): 
    else {
        printf("It seems the ghost has been discovered!\n");
        printf("The hunters have won the game!\n");
        printf("Using the evidence they found, they correctly determined that the ghost is a %s\n", ghostName);
    }
    
    printf("The hunters collected the following evidence:\n");
    //list evidence collected (no duplicates)
    char evName[MAX_STR];
    char EMF = 0;
    char temp = 0;
    char fingerprints = 0;
    char sound = 0;

    EvidenceNodeType *currNode1;
    EvidenceNodeType *nextNode1;
    currNode1 = house->evList.head;

    while(currNode1 != NULL) { //loop through the ev list 
        nextNode1 = currNode1->next; 
        //this was necessary so no duplicates appear
        if (currNode1->evidence == 0 && EMF == 0) {
            evidenceToString(0, evName);
            printf("%s\n", evName);
            EMF = 1;
        }
        else if (currNode1->evidence == 1 && temp == 0) {
            evidenceToString(1, evName);
            printf("%s\n", evName);
            temp = 1;
        }
        else if (currNode1->evidence == 2 && fingerprints == 0) {
            evidenceToString(2, evName);
            printf("%s\n", evName);
            fingerprints = 1;
        }
        else if (currNode1->evidence == 3 && sound == 0) {
            evidenceToString(3, evName);
            printf("%s\n", evName);
            sound = 1;
        }
        currNode1 = nextNode1;
    }
}
 