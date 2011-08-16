CC=gcc
PLUGIN_NAME=Manager
TARGET=lib${PLUGIN_NAME}.so
CONFIG=plugin.xml
DATA=logo.png
INCS=$(PLUGIN_NAME).h
SRCS=$(PLUGIN_NAME).c
OBJS=${SRCS:.c=.o}

LIBS=gtk+-3.0 gstreamer-0.10 sqlite3

CFLAGS=-I../../core -I../../ui `pkg-config --cflags ${LIBS}` -Wall -O2 -fPIC
LDFLAGS=`pkg-config --libs ${LIBS}` -Wall -O2 -shared -fPIC

all: ${TARGET}
	cp ${TARGET} ../../conf/plugin/${TARGET} && \
	mkdir ../../conf/plugin/$(PLUGIN_NAME)/ && \
	cp ${CONFIG} ../../conf/plugin/$(PLUGIN_NAME)/${CONFIG} && \
	cp ${DATA} ../../conf/plugin/$(PLUGIN_NAME)/${DATA}

${TARGET}:${OBJS}
	${CC} -o ${TARGET} ${OBJS} ${LDFLAGS}

${OBJS}:${INCS}

.c.o:
	${CC} -c $< ${CFLAGS}

clean:
	rm -rf ../../conf/plugin/$(PLUGIN_NAME)
	rm -f *.o ${TARGET}

rebuild: clean all