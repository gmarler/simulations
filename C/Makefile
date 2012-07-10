                        
# See standards(5) and lfcompile(5) man pages
CFLAGS := -D_POSIX_SOURCE -D_POSIX_C_SOURCE=200112L -D_XOPEN_SOURCE=500 \
          -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D__EXTENSIONS__ \
          -D_POSIX_PTHREAD_SEMANTICS -mt

LIBS   := -lpthread -lrt

test1: test1.c          
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIBS)
                        
timer_test: timer_test.c timers.o
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIBS)
                        
timers.o:  timers.c timers.h
	$(CC) $(CFLAGS) -g -c $@ $<

