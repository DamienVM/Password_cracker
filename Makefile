cracker: src/cracker.c
	gcc -pthread -std=c99 -Wall -Werror -o cracker src/cracker.c

test: src/test.c
	export LD_LIBRARY_PATH=$$HOME/local/lib:$$LD_LIBRARY_PATH
	gcc -L$$HOME/local/lib -lcunit -I$$HOME/local/include -Wall -Werror -o test src/test.c
	./test

all: cracker test

clean:
	rm cracker
	rm test
