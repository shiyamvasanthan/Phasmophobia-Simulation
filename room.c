#include "defs.h"

/*
Function: initRoomList    
Purpose: Initialize room list                       
in: The room list
out: The initalized room list */
void initRoomList(RoomListType* list){
	list->head = NULL;
}

/*
Function: initRoom    
Purpose: Initialize room's linked lists, mutex, number of hunters, number of connected rooms, ghost pointer, and name                       
in: The room
out: The initalized room*/
void initRoom(RoomType* room, char* name){
	strcpy(room->name, name);
	room->rooms = (RoomListType*)malloc(sizeof(RoomListType));
	initRoomList(room->rooms);
	room->evidence = (EvidenceListType*)malloc(sizeof(EvidenceListType));
	initEvidenceList(room->evidence);
	room->connectedRooms = 0;
	room->numHunters = 0;
	room->ghost = NULL;
	sem_init(&(room->mutex), 0, 1);
}

/*
Function: appendRoom    
Purpose: Add room to end of the linked list                  
in: The room list, room node to be added
out: The updated list of rooms*/
void appendRoom(RoomListType* rooms, RoomNodeType* room){
	RoomNodeType* curr = rooms->head;
	RoomNodeType* prev = NULL;
	
	if(curr == NULL){
		room->next = curr;
		rooms->head = room;
	} else {
		while(curr != NULL){
			prev = curr;
			curr = curr->next;
		}
		
		prev->next = room;
		room->next = curr;
	}
}

/*
Function: connectRooms    
Purpose: Adds room 2's node to room1 linked list and vice versa                
in: The two rooms that are to be connected */
void connectRooms(RoomType* room1, RoomType* room2){
	RoomNodeType* room1_node = (RoomNodeType*)malloc(sizeof(RoomNodeType));
	RoomNodeType* room2_node = (RoomNodeType*)malloc(sizeof(RoomNodeType));
	
	room1_node->room = room1;
	room2_node->room = room2;
	
	appendRoom(room1->rooms, room2_node);
	appendRoom(room2->rooms, room1_node);	
	
	room1->connectedRooms++;
	room2->connectedRooms++;
}

/*
Function: nextRoom    
Purpose: If changing rooms, find the room that the hunter/ghost will be changing to, and return it                
in: The hunter/ghost's current room
return: The next room that the hunter/ghost will go to*/
RoomType* nextRoom(RoomType* room){
	RoomNodeType* curr = room->rooms->head;
	RoomNodeType* prev = NULL;
	
	int i = 0;
	int randomRoom = randInt(0, room->connectedRooms);
	
	while(curr != NULL){
		prev = curr;
		curr = curr->next;
		
		if(i == randomRoom){
			return prev->room;
		}
		
		i++;
	}
	
	return NULL;
}

/*
Function: changeRoom    
Purpose: Hunter/Ghost will change rooms          
in: The hunter/ghost, the next room they will be going to
out: Updated hunter/ghost*/
void changeRoom(HunterType* hunter, GhostType* ghost, RoomType* nextRoom){
	if(ghost == NULL){
		hunter->room->hunters[hunter->hunterIndex] = NULL;
		hunter->room->numHunters--;
		
		nextRoom->hunters[hunter->hunterIndex] = hunter;
		nextRoom->numHunters++;
		
		hunter->room = nextRoom;
		
		hunter->boredom--;
	} else if (hunter == NULL){
		ghost->room->ghost = NULL;
		nextRoom->ghost = ghost;
		ghost->room = nextRoom;
	}
}

/*
Function: cleanupConnectedRooms    
Purpose: For each room clean up the nodes only of it's connected rooms        
in: Linked list of rooms of the building
out: Updated linked list*/
void cleanupConnectedRooms(RoomListType* rooms){
	RoomNodeType* curr = rooms->head;
	RoomNodeType* prev = NULL;
	
	while(curr != NULL){
		prev = curr;
		curr = curr->next;
		
		RoomNodeType* connectedCurr = prev->room->rooms->head;
		RoomNodeType* connectedPrev = NULL;
		
		while(connectedCurr != NULL){
			connectedPrev = connectedCurr;
			connectedCurr = connectedCurr->next;
			free(connectedPrev);
		}
		
		free(prev->room->rooms);
	}
}

/*
Function: cleanupAllRooms    
Purpose: Cleans up all the room nodes, room pointers, and linked list of evidence, of the building's linked list       
in: Linked list of rooms
out: Cleaned up linked list of rooms*/
void cleanupAllRooms(RoomListType* rooms){
	RoomNodeType* curr = rooms->head;
	RoomNodeType* prev = NULL;
	
	while(curr != NULL){
		prev = curr;
		curr = curr->next;
		cleanupEvidenceList(prev->room->evidence);
		free(prev->room->evidence);
		free(prev->room);
		free(prev);
	}
}
