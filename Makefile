OPT = -g #-Wall -Wextras -Werror #-g for debugging

run: main.o utils.o logger.o house.o room.o hunter.o ghost.o evidence.o
	gcc $(OPT) -o run main.o utils.o logger.o house.o room.o hunter.o ghost.o evidence.o

main.o: main.c defs.h
	gcc $(OPT) -c main.c

utils.o: utils.c defs.h
	gcc $(OPT) -c utils.c

logger.o: logger.c defs.h
	gcc $(OPT) -c logger.c

house.o: house.c defs.h
	gcc $(OPT) -c house.c

room.o: room.c defs.h
	gcc $(OPT) -c room.c

hunter.o: hunter.c defs.h
	gcc $(OPT) -c hunter.c

ghost.o: ghost.c defs.h
	gcc $(OPT) -c ghost.c

evidence.o: evidence.c defs.h
	gcc $(OPT) -c evidence.c

clean:
	rm -f *.o run