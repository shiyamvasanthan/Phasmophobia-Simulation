#include "defs.h"

/*
Function: initGhost    
Purpose: Initialize ghost boredom, randomly assign a ghost type, and randomly assign a room to the ghost     
in: The building
out: The building*/
void initGhost(BuildingType* b){
	b->ghost.boredom = BOREDOM_MAX; 
	randomGhostType(&b->ghost);
	randomGhostRoom(&b->ghost, &b->rooms);
}

/*
Function: randomGhostType    
Purpose: Generates a random ghost type
in: The ghost
out: The ghost assigned a ghostType*/
void randomGhostType(GhostType* ghost){
	//Generate a random number from 1 to 4
	int type = randInt(1, MAX_TYPES + 1);

	switch(type){
		case 1:
			ghost->ghostType = POLTERGEIST;
			break;
		case 2:
			ghost->ghostType = BANSHEE;
			break;
		case 3:
			ghost->ghostType = BULLIES;
			break;
		case 4:
			ghost->ghostType = PHANTOM;
			break;
	}
}

/*
Function: randomGhostRoom    
Purpose: Place ghost in random room that is not the van
in: The ghost, building's linked list of rooms
out: Updated ghost*/
void randomGhostRoom(GhostType* ghost, RoomListType* rooms){
	RoomNodeType* curr = rooms->head->next;
	RoomNodeType* prev = rooms->head;
	
	int i = 1;
	//Generate a random number from 1 to 12
	int randomRoom = randInt(1, NUM_ROOMS);
	
	while(curr != NULL){
		prev = curr;
		curr = curr->next;
		if(i == randomRoom){
			break;
		}
		i++;
	}
	
	ghost->room = prev->room;
	
}

/*
Function: ghostDecision    
Purpose: The ghost will make a decision based on a random number, 1 means it will generate ghostly evidence, 2 means do nothing, 3 means to change rooms
in: Decision number, Ghost, and next room if changing rooms
out: Updated ghost*/
void ghostDecision(int decision, GhostType* ghost, RoomType* nextRoom){
	switch(decision){
		case 1:
			generateGhostlyEvidence(ghost);
			break;
		case 2:
			break;
		case 3:
			changeRoom(NULL, ghost, nextRoom);
			printf("Ghost changed rooms to: %s\n", ghost->room->name);
			break;
	}
}

/*
Function: ghostTypeString    
Purpose: Convert the ghostType into a string that can be printed
in: String, ghostType
out: Updated string*/
void ghostTypeString(char* type, GhostClassType ghostType){
	switch(ghostType){
		case POLTERGEIST:
			strcpy(type, "POLTERGEIST");
			break;
		case BANSHEE:
			strcpy(type, "BANSHEE");
			break;
		case PHANTOM:
			strcpy(type, "PHANTOM");
			break;
		case BULLIES:
			strcpy(type, "BULLIES");
			break;
	}
}

