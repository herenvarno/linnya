CC=gcc
PROG_NAME=linnya
INCS=
SRCS=main.c global.c msg.c

OBJS=${SRCS:.c=.o}

LIBS=gtk+-2.0 gstreamer-0.10

CFLAGS=`pkg-config --cflags ${LIBS}` -Wall -O2
LDFLAGS=`pkg-config --libs ${LIBS}` -Wall -O2

all: ${PROG_NAME}

${PROG_NAME}:${OBJS}
	${CC} -o ${PROG_NAME} ${OBJS} ${LDFLAGS}
${OBJS}:${INCS}

.c.o:
	${CC} -g -c $< ${CFLAGS}

clean:
	rm -f *.o ${PROG_NAME}

rebuild: clean all
