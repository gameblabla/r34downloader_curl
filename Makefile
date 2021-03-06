CC = gcc

CFLAGS = -Ofast -march=native -mtune=native
LDFLAGS = -lcurl
OUTPUT = rule34curl

SOURCES = main.c
OBJS = ${SOURCES:.c=.o}

${OUTPUT}:${OBJS}
	${CC} -o ${OUTPUT} ${SOURCES} ${CFLAGS} ${LDFLAGS}
	
clean:
	rm *.o
	rm ${OUTPUT}
