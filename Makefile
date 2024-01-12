OPT = -Wall
LIBS = -lpthread
OBJ = building.o room.o ghost.o evidence.o hunter.o main.o

project: $(OBJ)
	gcc $(OPT) -o project $(OBJ) $(LIBS)
	
building.o: building.c defs.h
	gcc $(OPT) -c building.c $(LIBS)
	
room.o: room.c defs.h
	gcc $(OPT) -c room.c $(LIBS)
	
ghost.o: ghost.c defs.h
	gcc $(OPT) -c ghost.c $(LIBS)
	
evidence.o: evidence.c defs.h
	gcc $(OPT) -c evidence.c $(LIBS)
	
hunter.o: hunter.c defs.h
	gcc $(OPT) -c hunter.c $(LIBS)
	
main.o: main.c defs.h
	gcc $(OPT) -c main.c $(LIBS)
	
clean:
	rm -f project $(OBJ)
