int-set:		main.o int-set.o int-set-strings.o
			gcc main.o int-set.o int-set-strings.o -o int-set

int-set.o:		int-set.c int-set.h
			gcc -std=c18 -g -Wall -c int-set.c

main.o:			main.c int-set.h int-set-strings.h
			gcc -std=c18 -g -Wall -c main.c

int-set-strings.o:	int-set-strings.c int-set-strings.h int-set.h
			gcc -std=c18 -g -Wall -c int-set-strings.c

clean:
			rm -f *~ *.o int-set
