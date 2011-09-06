CC = gcc
SUBDIRS = Lrc Info Python Manager

all: ${TARGET}
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