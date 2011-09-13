CC=gcc
PLUGIN_NAME=Downlrc
TARGET=lib${PLUGIN_NAME}.so
CONFIG=plugin.xml
DATA=logo.png Downlrc.py
INCS=$(PLUGIN_NAME).h
SRCS=$(PLUGIN_NAME).c
OBJS=${SRCS:.c=.o}

LIBS=gtk+-3.0 gstreamer-0.10 sqlite3

CFLAGS=-I../../core -I../../ui -I../Lrc -I../Python `pkg-config --cflags ${LIBS}` -Wall -O2 -fPIC
LDFLAGS=-L../../conf/plugin -ILrc -IPython `pkg-config --libs ${LIBS}` -Wall -O2 -shared -fPIC

all: ${TARGET}
	if [ ! -d ../../conf/plugin/${PLUGIN_NAME} ];then mkdir ../../conf/plugin/${PLUGIN_NAME}/;fi
	cp ${TARGET} ../../conf/plugin/${PLUGIN_NAME}/${TARGET}
	cp ${CONFIG} ../../conf/plugin/${PLUGIN_NAME}/${CONFIG}
	@ for data in $(DATA); do \
	(cp ${DATA} ../../conf/plugin/${PLUGIN_NAME}/${data}); \
	done

${TARGET}:${OBJS}
	${CC} -o ${TARGET} ${OBJS} ${LDFLAGS}

${OBJS}:${INCS}

.c.o:
	${CC} -c $< ${CFLAGS}

clean:
	rm -rf ../../conf/plugin/$(PLUGIN_NAME)
	rm -f *.o ${TARGET}

rebuild: clean all