#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

extern int             interrupted;
extern pthread_mutex_t mutex;
extern pthread_cond_t  cond;

void report_resolution(void);
void timer_func(unsigned int iops);
static void timer_handler(int signum, siginfo_t *info, void *context);
void *sig_handler(void *arg);
