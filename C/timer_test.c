#include <unistd.h>
#include "timers.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;

int interrupted = 0;

int main(int argc, char **argv) {
  pthread_t         timer_thread_id,
                    sig_handler_id;
  int               status, c;
  sigset_t          set;
  struct sigaction  act;

  char             *filename;
  unsigned int      iops = 0;

  /* Start by masking the "interesting" signals,
   * SIGINT:   To abort
   * SIGRTMIN: Timer expired 
   * */
  sigemptyset (&set);
  sigaddset(&set, SIGINT );
  sigaddset(&set, SIGRTMIN );

  /* Because all threads inherit the signal mask from their creator, all threads
   * in this process will have these 2 signals masked unless one explicitly
   * unmasks it. */
  status = pthread_sigmask( SIG_BLOCK, &set, NULL );

  /*  parse arguments */
  while ( (c = getopt(argc, argv, "f:(logfile)i:(iops)")) != EOF ) {
    switch (c) {
      case 'f':   /* log'f'ile name */
        filename = strdup(optarg);
        printf("Logging to file: %s\n",optarg);
        break;
      case 'i':   /* 'i'ops to generate */
        iops = atoi(optarg);
        printf("Generating %s IOPS\n",optarg);
        break;
      default:
        break;
    }
  }
  /* default IOPS to 1 */
  if (iops == 0)
    iops = 1;

  report_resolution();

  /* Create the sigwait/sigwaitinfo thread */
  status = pthread_create( &sig_handler_id, NULL, sig_handler, NULL);
  if (status != 0) {
    perror("Startup of sig handler thread");
    exit(1);
  }

  /* Start up the timer */
  timer_func(iops);

  printf("Hit Ctrl-C to stop...\n");

  /* Wait for the sigwait thread to receive SIGINT and signal the condition
   * variable */
  status = pthread_mutex_lock( &mutex );
  if (status != 0) {
    perror("main thread mutex lock");
    exit(1);
  }
  while (!interrupted) {
    status = pthread_cond_wait( &cond, &mutex );
    if (status != 0) {
      perror("main thread cond wait");
      exit(1);
    }
  }
  status = pthread_mutex_unlock( &mutex );
  if (status != 0) {
    perror("main thread unlock mutex");
    exit(1);
  }
  printf("Terminating\n");
  exit(0);
}
