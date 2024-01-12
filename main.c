#include "defs.h"

int main(int argc, char *argv[])
{
    // Initialize a random seed for the random number generators
    srand(time(NULL));

    //Populate building with rooms, ghosts, and hunters
    BuildingType building;
    initBuilding(&building);
    populateRooms(&building);
    	
    HunterType h1, h2, h3, h4;
    
    initGhost(&building);
    
    printf("Ghost is in room: %s\n", building.ghost.room->name);
    	
	initHunters(&building, &h1, &h2, &h3, &h4);
	  	
	pthread_t hunterThreads[MAX_HUNTERS];
	pthread_t g;
	
	while(1){
		for (int i = 0; i < MAX_HUNTERS; i++){
			if (building.hunters[i].fear < MAX_FEAR && building.hunters[i].boredom > 0 && building.hunters[i].numGhostly < 3){
				pthread_create(&hunterThreads[i], NULL, hunterThread, &building.hunters[i]);
				pthread_join(hunterThreads[i], NULL);
			}
		}
		
		if(building.ghost.boredom > 0){
			pthread_create(&g, NULL, ghostThread, &building.ghost);
			pthread_join(g, NULL);
		}
		
		//If all the hunters and the ghost have exited the thread, exit the simulation
		if ((building.hunters[0].fear >= MAX_FEAR || building.hunters[0].boredom <= 0 || building.hunters[0].numGhostly >= 3) && (building.hunters[1].fear >= MAX_FEAR || building.hunters[1].boredom <= 0 || building.hunters[1].numGhostly >= 3) && (building.hunters[2].fear >= MAX_FEAR || building.hunters[2].boredom <= 0 || building.hunters[2].numGhostly >= 3) && (building.hunters[3].fear >= MAX_FEAR || building.hunters[3].boredom <= 0 || building.hunters[3].numGhostly >= 3) && building.ghost.boredom <= 0){
			break;
		}
		
	}

	printSummary(&building);
	cleanupBuilding(&building);
    return 0;
}

/*
Function: hunterThread    
Purpose: Thread for the hunters
in: Current hunter*/
void* hunterThread(void* arg){
	HunterType* hunter = (HunterType*) arg;
	        	
	//If hunter is in the same room as the ghost
	if(hunter->room->ghost != NULL){
		hunter->fear += FEAR_RATE;
		hunter->boredom = BOREDOM_MAX;
		printf("Hunter %s is in room with ghost! Fear: %d Boredom: %d\n", hunter->name, hunter->fear, hunter->boredom);
	}
	
	// If the hunter's room is available for modification, lock it
	if (sem_trywait(&(hunter->room->mutex)) == 0) {
		//Generate a random number between 1 and 3
		int decision = randInt(1, 4);
		
		//If changing rooms
		if(decision == 2){
			RoomType* next_room = nextRoom(hunter->room);
			
			if(sem_trywait(&(next_room->mutex)) != 0) {
				sem_post(&(hunter->room->mutex));
				return 0;
			} 
			
			hunterDecision(decision, hunter, next_room);
			sem_post(&(next_room->mutex));
		} else {
			hunterDecision(decision, hunter, NULL);
		}	
	}
		
	if(hunter->fear >= MAX_FEAR || hunter->boredom <= 0 || hunter->numGhostly >= 3){
		printf("Hunter %s has left the simulation\n", hunter->name);
		return(0);
	}
	
	sem_post(&(hunter->room->mutex));
	usleep(USLEEP_TIME);
    return(0);
}

/*
Function: ghostThread    
Purpose: Thread for the ghost
in: Ghost*/
void* ghostThread(void* arg){
	GhostType* ghost = (GhostType*) arg;
	    
	//If ghost is in same room as a hunter
	if (ghost->room->numHunters > 0){
		ghost->boredom = BOREDOM_MAX;
		printf("Ghost is in room with a hunter! Boredom: %d\n", ghost->boredom);
		
		if(sem_trywait(&(ghost->room->mutex)) == 0){
			//Pick a random number between 1 and 2
			int decision = randInt(1, 3);
			
			ghostDecision(decision, ghost, NULL);
		}
	} else {
		ghost->boredom--;
				
		if(sem_trywait(&(ghost->room->mutex)) == 0){
			//Pick a random number between 1 and 3
			int decision = randInt(1, 4);
		
			if(decision == 3){
				RoomType* next_room = nextRoom(ghost->room);
				
				if(sem_trywait(&(next_room->mutex)) != 0) {
					sem_post(&(ghost->room->mutex));
					return 0;
				} 
				
				ghostDecision(decision, ghost, next_room);
				sem_post(&(next_room->mutex));
			} else {
				ghostDecision(decision, ghost, NULL);
			}
		}
	}
		    
	if (ghost->boredom <= 0){
		printf("Ghost has left the simulation\n");
		return(0);
	}
	
	sem_post(&(ghost->room->mutex));
	usleep(USLEEP_TIME);
    return(0);
}

/*
Function: printSummary    
Purpose: When all threads have finished print whether the ghosts or hunter has won, and print the ghost type
in: Building*/
void printSummary(BuildingType* b){
	int numMaxFear = 0;
	int winIndex;
	
	for(int i = 0; i < MAX_HUNTERS; i++){
		if(b->hunters[i].fear >= MAX_FEAR){
			printf("Hunter %s got scared!\n", b->hunters[i].name);
			numMaxFear++;
		} else {
			printf("Hunter %s won!\n", b->hunters[i].name);
			winIndex = i;
		}
	}
	
	if(numMaxFear == MAX_HUNTERS){
		char type[MAX_STR];
		ghostTypeString(type, b->ghost.ghostType);
		printf("Ghost has won! GhostType was %s", type);
	} else {
		int isEmf = 0;
		int isTemp = 0;
		int isFinger = 0;
		int isSound = 0;
		
		EvidenceNodeType* curr = b->hunters[winIndex].evidence->head;
		EvidenceNodeType* prev = NULL;
		
		while(curr != NULL){
			prev = curr;
			curr = curr->next;
			
			if(prev->evidence->evidenceType == EMF){
				isEmf = 1;
			} else if (prev->evidence->evidenceType == TEMPERATURE){
				isTemp = 1;
			} else if (prev->evidence->evidenceType == FINGERPRINTS){
				isFinger = 1;
			} else if (prev->evidence->evidenceType == SOUND){
				isSound = 1;
			}
		}
				
		printf("The hunters collected the following types of data which helped: \n");
		
		if(isEmf){
			char evType[MAX_STR];
			evidenceTypeString(evType, EMF);
			printf("%s\n", evType);
		}
		
		if(isSound){
			char evType[MAX_STR];
			evidenceTypeString(evType, SOUND);
			printf("%s\n", evType);
		}
		
		if(isFinger){
			char evType[MAX_STR];
			evidenceTypeString(evType, FINGERPRINTS);
			printf("%s\n", evType);
		}
		
		if(isTemp){
			char evType[MAX_STR];
			evidenceTypeString(evType, TEMPERATURE);
			printf("%s\n", evType);
		}
		
		char type[MAX_STR];
		ghostTypeString(type, b->ghost.ghostType);
		printf("The ghost type is %s\n", type);
	}
}
/*
  Function:  randInt
  Purpose:   returns a pseudo randomly generated number, 
             in the range min to (max - 1), inclusively
       in:   upper end of the range of the generated number
   return:   randomly generated integer in the range [min, max-1) 
*/
int randInt(int min, int max)
{
    return (rand() % (max - min)) + min;
}

/*
  Function:  randFloat
  Purpose:   returns a pseudo randomly generated number, 
             in the range min to max, inclusively
       in:   upper end of the range of the generated number
   return:   randomly generated integer in the range [0, max-1) 
*/
float randFloat(float a, float b) {
    // Get a percentage between rand() and the maximum
    float random = ((float) rand()) / (float) RAND_MAX;
    // Scale it to the range we want, and shift it
    return random * (b - a) + a;
}
