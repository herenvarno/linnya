CC = gcc

TARGET = linnya

SUBDIRS = core ui plugin
TARDIRS = core ui

SRCS = $(wildcard *.c)
INCS = ${SRCS:.c=.h}
OBJS = ${SRCS:.c=.o}

LIBS=sqlite3 gstreamer-0.10 gtk+-3.0
CFLAGS=-Iliblinnya-core -Iliblinnya-ui `pkg-config --cflags ${LIBS}` -Wall -O2
LDFLAGS=-L./core -L./ui -llinnya-core -llinnya-ui `pkg-config --libs ${LIBS}` -Wall -O2

all: ${TARGET}
	@ for subdir in ${TARDIRS}; do \
		(cp $$subdir/liblinnya-$$subdir.so conf/lib/liblinnya-$$subdir.so); \
	done

${TARGET}: ${OBJS} libs
	${CC} ${OBJS} -o $@ ${LDFLAGS}

.c.o:
	${CC} -c $< ${CFLAGS}

libs:
	@ for subdir in $(SUBDIRS); do \
		(cd $$subdir && $(MAKE)); \
	done

.PHONY:clean
clean:
	${RM} conf/lib/*.so
	$(RM) $(TARGET) $(OBJS)
	@ for subdir in $(SUBDIRS); do \
		(cd $$subdir && $(MAKE) clean); \
	done

rebuild: clean all