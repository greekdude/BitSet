CC = gcc
LIB = lib/
TEST_OUTPUT = -o test.out

lib: PIC LIB

PIC:
	${CC} -c -Wall -Werror -fpic bitset.c

LIB:
	${CC} -shared -o libBitset.so bitset.o

test:
	${CC} *.c ${TEST_OUTPUT}
