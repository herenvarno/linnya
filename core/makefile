CC = gcc

TARGET = liblinnya-core.so

SRCS = $(wildcard *.c)
INCS = ${SRCS:.c=.h}
OBJS = ${SRCS:.c=.o}

LIBS=sqlite3 gstreamer-0.10
CFLAGS=`pkg-config --cflags ${LIBS}` -Wall -O2
LDFLAGS=`pkg-config --libs ${LIBS}` -Wall -O2

all: ${TARGET}

${TARGET}: ${OBJS}
	${CC} -shared -fPIC -o $@ $^ ${LDFLAGS}

.c.o:
	${CC} -fPIC -c $< ${CFLAGS}

.PHONY:clean
clean:
	${RM} ${TARGET} ${OBJS}
	
rebuild: clean all