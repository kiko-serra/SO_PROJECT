all: samples

samples: samples.c
	gcc -Wall -o samples samples.c

run:
	./samples quote.txt 4 8

clean:
	@rm -rf *.o samples *.dSYM

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./samples quote.txt 4 8