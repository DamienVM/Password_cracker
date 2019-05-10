cracker: src/cracker.c src/cracker_fct.c src/reverse.c src/reverse.h src/sha256.c src/sha256.h
	gcc -g -pthread -std=c99 -Wall -Werror -o cracker src/cracker.c


.PHONY :tests
tests: src/test.c
	gcc -g -pthread -std=c99 -Wall -Werror -o tests/test src/test.c
	./tests/test

all: cracker tests

clean:
	rm -f cracker tests/test
