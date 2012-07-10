#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

typedef struct my_data_tag {
  char payload[150];
  char header[21];
  char trailer[1];
} my_data_t;

pthread_mutex_t write_mutex;
pthread_cond_t  write_cond;
extern int      writes_pending;
extern char    *filename;


void *tiny_writer(void * arg);
