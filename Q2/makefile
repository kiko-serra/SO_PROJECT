all: txt2epub

txt2epub: txt2epub.c
	gcc -g -Wall -o txt2epub txt2epub.c  

run:
	./txt2epub f1.txt f2.txt f3.txt f4.txt

clean:
	@rm -rf *.o txt2epub *.epub *.zip *.dSYM

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./txt2epub f1.txt f2.txt f3.txt f4.txt