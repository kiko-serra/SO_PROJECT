all: tokenring

tokenring: tokenring.c
	gcc -Wall -o tokenring tokenring.c

run:
	./tokenring 5 0.01 10

clean:
	@rm -rf *.o tokenring pipe*

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./tokenring 5 0.01 10
