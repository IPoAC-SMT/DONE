all: main.o  ./obj/rw-zem.o ./obj/doneserver.o ./obj/functions.o ./obj/GUI.o ./obj/interface.o ./obj/netlib.o ./obj/logicalController.o ./obj/log.o ./obj/netlib.so
	gcc main.o ./obj/*.o -lm -lraylib -o ./bin/executable

./obj/%.o: src/%.c
	gcc -Wall -Wextra -c $< -o $@

./obj/netlib.so: src/netlib.c
	gcc -shared -Wall -Wextra src/netlib.c src/log.c -o ./obj/netlib.so

main.o:
	gcc -Wall -Wextra -c main.c

.PHONY: clean
clean:
	rm ./bin/executable ./obj/* main.o

.PHONY: exec
exec:
	sudo ./bin/executable

.PHONY: cli
cli:
	sudo ./bin/executable --no-gui

.PHONY: IPoAC
IPoAC:
	sudo ./bin/executable --ipoac

.PHONY: prod
prod:
	make
	rm ./obj/* main.o

.PHONY: compile
compile:
	./DoneScript/compiler.py

.PHONY: import
import:
	cp ./DoneScript/compiled/* ./saves/

