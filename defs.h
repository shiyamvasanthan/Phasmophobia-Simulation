#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_STR 64
#define FEAR_RATE 1
#define MAX_FEAR 100
#define MAX_HUNTERS 4
#define USLEEP_TIME 50000
#define BOREDOM_MAX 99
//Number of rooms
#define NUM_ROOMS 13
//Max # of types
#define MAX_TYPES 4 
//Initial fear
#define INIT_FEAR 0
//Success flag
#define C_OK 0
//Fail flag
#define C_NOK -1
// You may rename these types if you wish
typedef enum { EMF, TEMPERATURE, FINGERPRINTS, SOUND } EvidenceClassType;
typedef enum { POLTERGEIST, BANSHEE, BULLIES, PHANTOM } GhostClassType;

//Room
typedef struct {
  char name[MAX_STR];
  struct Ghost* ghost;
  struct Hunter* hunters[MAX_HUNTERS];
  struct RoomList* rooms;
  struct EvidenceList* evidence;
  int connectedRooms;
  int numHunters;
  sem_t mutex;
} RoomType;

//Ghost
typedef struct Ghost {
  GhostClassType ghostType;
  RoomType* room;
  int boredom;
} GhostType;

//Hunter
typedef struct Hunter{
  RoomType* room;
  EvidenceClassType evidenceType;
  struct EvidenceList* evidence;
  char name[MAX_STR];
  int fear;
  int boredom;
  int hunterIndex;
  int numGhostly;
  struct Building* building;
} HunterType;

//RoomNodeType
typedef struct RoomNode{
  RoomType* room;
  struct RoomNode* next;
} RoomNodeType;

//Linked list of rooms
typedef struct RoomList {
  RoomNodeType* head;
} RoomListType;

//Evidence
typedef struct {
  EvidenceClassType evidenceType;
  float value;
} EvidenceType;

//EvidenceNodeType
typedef struct EvidenceNode{
  EvidenceType* evidence;
  struct EvidenceNode* next;
} EvidenceNodeType;

//Linked list of evidence
typedef struct EvidenceList {
  EvidenceNodeType* head;
  EvidenceNodeType* tail;
  sem_t mutex;
} EvidenceListType;

//Building
typedef struct Building{
  HunterType hunters[MAX_HUNTERS];
  GhostType ghost;
  RoomListType rooms;
  EvidenceListType sharedEvidence;
} BuildingType;

int randInt(int, int);          // Generates a pseudorandom integer between the parameters
float randFloat(float, float);  // Generates a pseudorandom float between the parameters

void initBuilding(BuildingType*);
void populateRooms(BuildingType*);   // Populates the building with sample data for rooms
void cleanupBuilding(BuildingType*);

void initRoomList(RoomListType*);
void initRoom(RoomType*, char*);
void appendRoom(RoomListType*, RoomNodeType*);
void connectRooms(RoomType*, RoomType*);
void changeRoom(HunterType*, GhostType*, RoomType*);
RoomType* nextRoom(RoomType*);
void cleanupConnectedRooms(RoomListType*);
void cleanupAllRooms(RoomListType*);

void initGhost(BuildingType*);
void randomGhostType(GhostType*);
void randomGhostRoom(GhostType*, RoomListType*);
void ghostDecision(int, GhostType*, RoomType*);
void ghostTypeString(char*, GhostClassType);

void initHunter(HunterType*, char*, BuildingType*, EvidenceClassType);
void initHunters(BuildingType*, HunterType*, HunterType*, HunterType*, HunterType*);
void hunterDecision(int, HunterType*, RoomType*);
int selectRandomHunter(int, RoomType*);

void initEvidenceList(EvidenceListType*);
void initEvidence(EvidenceType*, EvidenceClassType, float);
int validateEvidence(EvidenceClassType, float);
void generateGhostlyEvidence(GhostType*);
void addEvidenceToCollection(EvidenceListType*, EvidenceType*);
void collectEvidence(HunterType*);
void generateStandardEvidence(HunterType*);
void removeEvidenceFromRoom(EvidenceListType*, EvidenceClassType, float);
void communicateEvidence(HunterType*);
int checkDuplicateEvidence(EvidenceListType*, EvidenceClassType, float);
void checkDuplicateEvidenceType(HunterType*, EvidenceClassType);
void evidenceTypeString(char*, EvidenceClassType);
void cleanupEvidenceNodes(EvidenceListType*);
void cleanupEvidenceList(EvidenceListType*);

void* hunterThread(void*);
void* ghostThread(void*);
void printSummary(BuildingType*);
