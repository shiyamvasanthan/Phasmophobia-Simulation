#include "defs.h"

/*
Function: initHunter    
Purpose: Initializes hunter's evidence linkedlist, fear, boredom, equipment type, name, room pointer, number of ghostly evidence, and building pointer
in: Hunter, name, building, equipment type
out: Initialized hunter*/
void initHunter(HunterType* hunter, char* name, BuildingType* b, EvidenceClassType e){
	hunter->fear = INIT_FEAR;
	hunter->boredom = BOREDOM_MAX;
	hunter->evidenceType = e;
	strcpy(hunter->name, name);
	hunter->evidence = (EvidenceListType*)malloc(sizeof(EvidenceListType));
	initEvidenceList(hunter->evidence);
	hunter->room = b->rooms.head->room;
	hunter->numGhostly = 0;
	hunter->building = b;
}

/*
Function: initHunters    
Purpose: Asks user for hunter's names, initalizes hunters, adds them to the building collection, and places them in the van, also sets the index of each hunter
in: LinkedList of evidence
out: Cleaned up linked list*/
void initHunters(BuildingType* b, HunterType* h1, HunterType* h2, HunterType* h3, HunterType* h4){	
	h1->hunterIndex = 0;
	h2->hunterIndex = 1;
	h3->hunterIndex = 2;
	h4->hunterIndex = 3;
	
	char name[MAX_STR];
	
	//No need to randomly generate equipment type, just assign one to each hunter
	printf("Enter the name for Hunter 1: ");
	scanf("%s", name);
	initHunter(h1, name, b, EMF);
	
	printf("Enter the name for Hunter 2: ");
	scanf("%s", name);
	initHunter(h2, name, b, TEMPERATURE);
	
	printf("Enter the name for Hunter 3: ");
	scanf("%s", name);
	initHunter(h3, name, b, TEMPERATURE);
	
	printf("Enter the name for Hunter 4: ");
	scanf("%s", name);
	initHunter(h4, name, b, TEMPERATURE);
		
	b->hunters[0] = *h1;
	b->hunters[1] = *h2;
	b->hunters[2] = *h3;
	b->hunters[3] = *h4;
			
	b->rooms.head->room->hunters[0] = h1;
	b->rooms.head->room->hunters[1] = h2;
	b->rooms.head->room->hunters[2] = h3;
	b->rooms.head->room->hunters[3] = h4;
	
	//Set the number of hunters in the van to 4
    b->rooms.head->room->numHunters = 4;
}

/*
Function: hunterDecision    
Purpose: The ghost will make a decision based on a random number, 1 means it will collect evidence, 2 means change rooms, 3 means to communicate evidence
in: Decision number, Hunter, and next room if changing rooms
out: Updated Hunter*/
void hunterDecision(int decision, HunterType* hunter, RoomType* nextRoom){
	switch(decision){
		case 1:
			collectEvidence(hunter);
			break;
		case 2:
			changeRoom(hunter, NULL, nextRoom);
			printf("Hunter %s changed rooms to: %s\n", hunter->name, hunter->room->name);
			break;
		case 3:
			communicateEvidence(hunter);
			break;
	}
}

/*
Function: selectRandomHunter    
Purpose: Hunter will randomly select a hunter in the room
in: Index of the current hunter, and the hunter's current room
return: C_NOK if there's no other hunter's in the room*/
int selectRandomHunter(int index, RoomType* room){
	//Array of indexes
	int hunters[MAX_HUNTERS];
	
	if(room->numHunters <= 1){
		return C_NOK;
	}
	
	for(int i = 0; i < MAX_HUNTERS; i++){
		//If a hunter that is not the current hunter exists
		if(room->hunters[i] != NULL && i != index){
			hunters[i] = i;
		} else {
			hunters[i] = -1;
		}
		
	}
	
	int j;
	while(1){
		j = randInt(0, MAX_HUNTERS);
		if(hunters[j] != -1){
			return hunters[j];
		}
	}
	
	return C_NOK;
}
