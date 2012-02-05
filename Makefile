.PHONY = ALL CLEAN

#CC = gcc -Wall -pedantic -std=c99 -lfcgi
# BSD:
CC = gcc -Wall -pedantic -std=c99 -L/usr/local/lib -I/usr/local/include -lfcgi

ALL: main

main:
	${CC} main.c -o ident_query

clean:
	rm -rf *.o ident_query
