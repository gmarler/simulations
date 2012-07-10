#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

typedef struct my_data_tag {
  char payload[60];
  char header[21];
  char trailer[1];
} my_data_t;

pthread_mutex_t      write_mutex;
pthread_cond_t       write_cond;
extern int           writes_pending;
extern char         *filename;
extern unsigned int  iops;


void *tiny_writer(void * arg);
