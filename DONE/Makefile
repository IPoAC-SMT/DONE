all: main.o  ./obj/functions.o ./obj/GUI.o ./obj/interface.o ./obj/settings.o ./obj/netlib.o ./obj/logicalController.o
	gcc main.o ./obj/* -lm -lraylib -o ./bin/executable

./obj/%.o: src/%.c
	gcc -Wall -Wextra -c $< -o $@

main.o:
	gcc -Wall -Wextra -c main.c

.PHONY: clean
clean:
	rm ./bin/executable ./obj/* main.o

.PHONY: exec
exec:
	./bin/executable
