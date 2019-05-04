cracker: src/cracker.c
	gcc -pthread -std=c99 -Wall -Werror -o cracker src/cracker.c

test: tests/01_4c_1k.bin 
	./cracker -t 10 tests/01_4c_1k.bin

all:cracker test

clean:
	rm cracker
