#include "defs.h"

/*
Function: initEvidenceList    
Purpose: Initialize list of evidence
in: List of evidence
out: Initialized list of evidence*/
void initEvidenceList(EvidenceListType* list){
	list->head = NULL;
	list->tail = NULL;
}

/*
Function: initEvidence    
Purpose: Initialize evidence with the given type and value
in: evidence pointer, evidenceType, and value
out: Initialized evidence*/
void initEvidence(EvidenceType* e, EvidenceClassType evidenceType, float value){
	e->evidenceType = evidenceType;
	e->value = value;
}

/*
Function: validateEvidence    
Purpose: Determines whether evidence is ghostly based on the given type and value
in: evidenceType, and value
return: C_OK if the evidence is strictly ghostly, C_NOK otherwise*/
int validateEvidence(EvidenceClassType evidenceType, float value){
	switch(evidenceType){
		case EMF:
			if(value > 4.9 && value <= 5){
				return C_OK;
			}
			return C_NOK;
			break;
		case TEMPERATURE:
			if(value >= -10 && value < 0){
				return C_OK;
			}
			return C_NOK;
			break;
		case FINGERPRINTS:
			if(value == 1){
				return C_OK;
			}
			return C_NOK;
			break;
		case SOUND:
			if(value > 70 && value <= 75){
				return C_OK;
			}
			return C_NOK;
			break;
		default:
			return C_NOK;
	}
}

/*
Function: generateGhostlyEvidence    
Purpose: Randomly generate a ghostly evidence and add it to the room
in: ghost */
void generateGhostlyEvidence(GhostType* ghost){
	EvidenceType* evidence = (EvidenceType*)malloc(sizeof(EvidenceType));
	int i;
	float value;
	switch(ghost->ghostType){
		case POLTERGEIST:
			i = randInt(1, MAX_TYPES);
			if(i == 1){
				value = randFloat(4.7, 5);
				initEvidence(evidence, EMF, value);
			} else if (i == 2){
				value = randFloat(-10, 1);
				initEvidence(evidence, TEMPERATURE, value);
			} else if (i == 3) {
				initEvidence(evidence, FINGERPRINTS, 1);
			}
			break;
		case BANSHEE:
			i = randInt(1, MAX_TYPES);
			if(i == 1){
				value = randFloat(4.7, 5);
				initEvidence(evidence, EMF, value);
			} else if (i == 2){
				value = randFloat(-10, 1);
				initEvidence(evidence, TEMPERATURE, value);
			} else if (i == 3) {
				value = randFloat(65, 75);
				initEvidence(evidence, SOUND, value);
			}
			break;
		case BULLIES:
			i = randInt(1, MAX_TYPES);
			if(i == 1){
				value = randFloat(4.7, 5);
				initEvidence(evidence, EMF, value);
			} else if (i == 2){
				initEvidence(evidence, FINGERPRINTS, 1);
			} else if (i == 3) {
				value = randFloat(65, 75);
				initEvidence(evidence, SOUND, value);
			}
			break;
		case PHANTOM:
			i = randInt(1, MAX_TYPES);
			if(i == 1){
				value = randFloat(-10, 1);
				initEvidence(evidence, TEMPERATURE, value);
			} else if (i == 2){
				initEvidence(evidence, FINGERPRINTS, 1);
			} else if (i == 3) {
				value = randFloat(65, 75);
				initEvidence(evidence, SOUND, value);
			}
			
			break;
	}
	
	char type[MAX_STR];
	evidenceTypeString(type, evidence->evidenceType);
	printf("Ghost generated %s evidence in %s\n", type, ghost->room->name); 
	addEvidenceToCollection(ghost->room->evidence, evidence);
}

/*
Function: addEvidenceToCollection    
Purpose: Adds evidence to the room or hunter or building's shared evidence collection at the tail 
in: Linked list of evidence, and evidence to be added 
out: Updated linked list of evidence*/
void addEvidenceToCollection(EvidenceListType* evidence, EvidenceType* e){
	EvidenceNodeType* curr = evidence->head;
	EvidenceNodeType* new = (EvidenceNodeType*)malloc(sizeof(EvidenceNodeType));
	new->evidence = e;
	
	if(curr == NULL){
		new->next = curr;
		evidence->head = new;
		evidence->tail = new;
	} else {		
		evidence->tail->next = new;
		new->next = NULL;
		evidence->tail = new;
	}
}

/*
Function: collectEvidence    
Purpose: Hunter collects the evidence from the room it's currently in
in: Hunter
out: Updated Hunter*/
void collectEvidence(HunterType* hunter){
	EvidenceNodeType* curr = hunter->room->evidence->head;
	EvidenceNodeType* prev = NULL;
	//Variable that tells if there is evidence in the room
	int evidenceFound = 0;
	
	while(curr != NULL){
		prev = curr;
		curr = curr->next;
		
		//Check if the hunter can read the type of evidence and if the hunter already has that evidence
		if(prev->evidence->evidenceType == hunter->evidenceType && checkDuplicateEvidence(hunter->evidence, prev->evidence->evidenceType, prev->evidence->value) == C_OK){
			int type = prev->evidence->evidenceType;
			float value = prev->evidence->value;
			removeEvidenceFromRoom(hunter->room->evidence, type, value);
			
			EvidenceType* e = (EvidenceType*)malloc(sizeof(EvidenceType));
			initEvidence(e, type, value);
			
			addEvidenceToCollection(hunter->evidence, e);
			addEvidenceToCollection(&hunter->building->sharedEvidence, e);
			evidenceFound = 1;
			
			char evType[MAX_STR];
			evidenceTypeString(evType, type);
			
			//If the evidence is ghostly reset the hunter's boredom to max
			if(validateEvidence(type, value) == C_OK){
				hunter->boredom = BOREDOM_MAX;
								
				checkDuplicateEvidenceType(hunter, type);
				printf("Hunter %s picked up ghostly %s evidence from %s\n", hunter->name, evType, hunter->room->name);
			} else {
				printf("Hunter %s picked up standard %s evidence from %s\n", hunter->name, evType, hunter->room->name);
			}
			break;
		}
	}
	
	//If no evidence is found generate standard evidence
	if(curr == NULL || evidenceFound == 0){
		generateStandardEvidence(hunter);
	}
}

/*
Function: removeEvidenceFromRoom    
Purpose: Removes the evidence from the room based on evidenceType and value
in: Room's linked list of evidence, evidenceType and value
out: Updated room's linked list of evidence*/
void removeEvidenceFromRoom(EvidenceListType* evidence, EvidenceClassType evidenceType, float value){
	EvidenceNodeType* curr = evidence->head;
	EvidenceNodeType* prev = NULL;
	
	while(curr != NULL){
		if(curr->evidence->evidenceType == evidenceType && curr->evidence->value == value){
			break;
		}
		
		prev = curr;
		curr = curr->next;
	}
	
	if(prev == NULL){
		evidence->head = curr->next;
	} else {
		if(prev->next == evidence->tail){
			prev->next = curr->next;
			evidence->tail = prev->next;
		} else {
			prev->next = curr->next;
		}
	}
	
	
	free(curr->evidence);
	free(curr);
}

/*
Function: generateStandardEvidence    
Purpose: Adds standard evidence to hunter and adds it to the shared evidence
in: Hunter
out: Updated hunter*/
void generateStandardEvidence(HunterType* hunter){
	EvidenceType* evidence = (EvidenceType*)malloc(sizeof(EvidenceType));
	float value;
	switch(hunter->evidenceType){
		case EMF:
			value = randFloat(0, 4.9);
			initEvidence(evidence, EMF, value);
			break;
		case TEMPERATURE:
			value = randFloat(0, 27);
			initEvidence(evidence, TEMPERATURE, value);
			break;
		case FINGERPRINTS:
			initEvidence(evidence, FINGERPRINTS, 0);
			break;
		case SOUND:
			value = randFloat(40, 70);
			initEvidence(evidence, SOUND, value);
			break;
	}
	
	//Make sure the hunter doesn't generate duplicate standard evidence
	if(checkDuplicateEvidence(hunter->evidence, evidence->evidenceType, evidence->value) == C_OK){
		char type[MAX_STR];
		evidenceTypeString(type, evidence->evidenceType);
		printf("Hunter %s generated standard %s evidence in %s\n", hunter->name, type, hunter->room->name);
		addEvidenceToCollection(hunter->evidence, evidence);
		addEvidenceToCollection(&hunter->building->sharedEvidence, evidence);
	}
}

/*
Function: checkDuplicateEvidence    
Purpose: Prevents the sharing of duplicate evidence from hunters, and prevents the collection of duplicate evidence from a room
in: Linked list of evidence, evidencetype, and value
return: C_OK if there is no duplicates based on type and value, C_NOK otherwise*/
int checkDuplicateEvidence(EvidenceListType* evidence, EvidenceClassType evidenceType, float value){
	EvidenceNodeType* curr = evidence->head;
	EvidenceNodeType* prev = NULL;
	
	while(curr != NULL){		
		prev = curr;
		curr = curr->next;
		
		if(prev->evidence->evidenceType == evidenceType && prev->evidence->value == value){
			return C_NOK;
		}
	}
	
	return C_OK;
}

/*
Function: checkDuplicateEvidenceType    
Purpose: Increases the hunter's number of ghostly evidence, if the hunter doesn't already have an evidence of that type
in: Hunter Linked list of evidence, evidencetype */
void checkDuplicateEvidenceType(HunterType* hunter, EvidenceClassType evidenceType){
	EvidenceNodeType* curr = hunter->evidence->head;
	EvidenceNodeType* prev = NULL;
	
	while(curr != NULL){		
		prev = curr;
		curr = curr->next;
		
		if(prev->evidence->evidenceType == evidenceType){
			return;
		}
	}
	
	hunter->numGhostly++;
}

/*
Function: communicateEvidence    
Purpose: Hunter will select a random hunter in the room and append one piece of ghostly evidence
in: Hunter */
void communicateEvidence(HunterType* hunter){
	EvidenceNodeType* curr = hunter->evidence->head;
	EvidenceNodeType* prev = NULL;
	
	int randomHunter = selectRandomHunter(hunter->hunterIndex, hunter->room);
	
	//If there is another hunter in the room
	if(randomHunter != C_NOK){
		while(curr != NULL){
			prev = curr;
			curr = curr->next;
			
			//If the evidence is ghostly and not a duplicate, add it to the other hunter's collection
			if(validateEvidence(prev->evidence->evidenceType, prev->evidence->value) == C_OK && checkDuplicateEvidence(hunter->room->hunters[randomHunter]->evidence, prev->evidence->evidenceType, prev->evidence->value) == C_OK){
				checkDuplicateEvidenceType(hunter->room->hunters[randomHunter], prev->evidence->evidenceType);
				addEvidenceToCollection(hunter->room->hunters[randomHunter]->evidence, prev->evidence); 
				
				char type[MAX_STR];
				evidenceTypeString(type, prev->evidence->evidenceType);
				printf("Hunter %s communicated ghostly %s evidence with Hunter %s\n", hunter->name, type, hunter->room->hunters[randomHunter]->name);
				break;
			}
		}
		
		printf("Hunter %s communicated nothing with Hunter %s\n", hunter->name, hunter->room->hunters[randomHunter]->name);
	} else {
		printf("Hunter %s tried to communicate but there are no hunters in the room\n", hunter->name);
	}
}

/*
Function: evidenceTypeString    
Purpose: Convert the evidenceType into a string that can be printed
in: String, evidenceType
out: Updated string*/
void evidenceTypeString(char* type, EvidenceClassType evidenceType){	
	switch(evidenceType){
		case EMF:
			strcpy(type, "EMF");
			break;
		case TEMPERATURE:
			strcpy(type, "TEMPERATURE");
			break;
		case FINGERPRINTS:
			strcpy(type, "FINGERPRINTS");
			break;
		case SOUND:
			strcpy(type, "SOUND");
			break;	
	}
}

/*
Function: cleanupEvidenceNodes    
Purpose: Cleans up only the nodes in the evidence linked list
in: LinkedList of evidence
out: Cleaned up linked list*/
void cleanupEvidenceNodes(EvidenceListType* evidence){
	EvidenceNodeType* curr = evidence->head;
	EvidenceNodeType* prev = NULL;
	
	while(curr != NULL){
		prev = curr;
		curr = curr->next;
		free(prev);
	}
}

/*
Function: cleanupEvidenceList    
Purpose: Cleans up the nodes and evidence in the linked list
in: LinkedList of evidence
out: Cleaned up linked list*/
void cleanupEvidenceList(EvidenceListType* evidence){
	EvidenceNodeType* curr = evidence->head;
	EvidenceNodeType* prev = NULL;
	
	while(curr != NULL){
		prev = curr;
		curr = curr->next;
		free(prev->evidence);
		free(prev);
	}
}
