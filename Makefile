CC = gcc

CFLAGS = -Wall -Wextra -Ofast -march=native -mtune=native
LDFLAGS = -lcurl
OUTPUT = rule34curl

SOURCES = src/core.c src/cli.c
OBJS = ${SOURCES:.c=.o}

${OUTPUT}:${OBJS}
	${CC} -o ${OUTPUT} ${SOURCES} ${CFLAGS} ${LDFLAGS}
	
clean:
	rm *.o
	rm ${OUTPUT}
