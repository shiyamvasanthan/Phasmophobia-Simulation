Program Author: Shiyam Vasanthan

Program Purpose: This program is a simulation of hunters and ghosts in a building. The hunters main objective is to obtain three different pieces of ghostly
evidence to figure out what type of ghost is in the building, and the ghost's main objective is to scare the hunters away so they don't guess it's type.
The hunters will randomly make 1 of 3 decisions: collect evidence, change rooms, or communicate evidence with hunters that are in the same room.
The ghost will randomly make 1 of 3 decisions: generate evidence, do nothing, or change rooms. However, if the ghost is in a room with hunters it cannot change rooms.
The simulation will go on until the ghost gets bored, and all of the hunters have gotten bored, scared, or completed the quest of finding 3 different ghostly evidence.
At the end of the simulation there will be a summary on whether the hunters won or the ghosts won.

Source Files: building.c room.c evidence.c ghost.c hunter.c main.c

Compilation Command: make project

Launching and Operating Instructions: Type ./project in the terminal. This is now an executable file. To run using valgrind do: valgrind --leak-check=full ./project
You will be prompted with entering four hunter names (no spaces), and hit enter to enter the next hunter name. After you have entered the four hunter names,
you will see the simulation print.

Changes/Design Decisions:
Changed appendRoom to pass in RoomListType* instead of RoomListType
Decided make communicateEvidence() function only share one piece of ghostly evidence to optimize memory and runtime, and because that's all that the hunter needs

