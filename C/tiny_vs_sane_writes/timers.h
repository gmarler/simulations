#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

extern int             interrupted;
extern int             writes_pending;
extern pthread_mutex_t intr_mutex;
extern pthread_cond_t  intr_cond;
extern pthread_mutex_t write_mutex;
extern pthread_cond_t  write_cond;

void report_resolution(void);
void timer_func(unsigned int iops);
static void timer_handler(int signum, siginfo_t *info, void *context);
void *sig_handler(void *arg);
