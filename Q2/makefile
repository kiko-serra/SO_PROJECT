all: txt2epub run

txt2epub: txt2epub.c
	gcc -Wall -o txt2epub txt2epub.c  

run: txt2epub
	./txt2epub f1.txt f2.txt f3.txt f4.txt

clean:
	@rm -f *.o txt2epub *.epub *.zip