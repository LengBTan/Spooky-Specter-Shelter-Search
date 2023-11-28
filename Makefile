OPT = -g #-Wall -Wextras -Werror #-g for debugging



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

