all: samples run

samples: samples.c
	gcc -Wall -o samples samples.c

run:
	./samples quote.txt 4 8

clean:
	@rm -rf *.o samples *.dSYM