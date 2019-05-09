cracker: src/cracker.c
	gcc -g -pthread -std=c99 -Wall -Werror -o cracker src/cracker.c

tests: src/test.c
	gcc -g -pthread -std=c99 -Wall -Werror -o test src/test.c
	./test
	rm test

all: cracker tests

clean:
	rm -f cracker test
