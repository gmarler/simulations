#include <unistd.h>
#include "timers.h"
#include "writer.h"

pthread_mutex_t intr_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  intr_cond  = PTHREAD_COND_INITIALIZER;

int   interrupted    = 0;
int   writes_pending = 0;
char *filename       = NULL;
unsigned int iops    = 0;

int main(int argc, char **argv) {
  pthread_t         timer_thread_id,
                    sig_handler_id,
                    writer_id;
  int               status, c;
  sigset_t          set;
  struct sigaction  act;


  /* Start by masking the "interesting" signals,
   * SIGINT:   To abort
   * SIGRTMIN: Time to issue another I/O (simulates incoming data to be logged)
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
  /* fail if filename not set */
  if (! filename) {
    perror("Must provide filename to log to");
    exit(1);
  }

  report_resolution();

  /* Create the sigwait/sigwaitinfo thread */
  status = pthread_create( &sig_handler_id, NULL, sig_handler, NULL);
  if (status != 0) {
    perror("Startup of sig handler thread");
    exit(1);
  }
  /* Create the writer thread */
  status = pthread_create( &writer_id, NULL, tiny_writer, NULL);
  if (status != 0) {
    perror("Startup of writer thread");
    exit(1);
  }

  /* Start up the timer */
  timer_func(iops);

  printf("Hit Ctrl-C to stop...\n");

  /* Wait for the sigwait thread to receive SIGINT and signal the condition
   * variable */
  status = pthread_mutex_lock( &intr_mutex );
  if (status != 0) {
    perror("main thread intr_mutex lock");
    exit(1);
  }
  while (!interrupted) {
    status = pthread_cond_wait( &intr_cond, &intr_mutex );
    if (status != 0) {
      perror("main thread intr_cond wait");
      exit(1);
    }
  }
  status = pthread_mutex_unlock( &intr_mutex );
  if (status != 0) {
    perror("main thread unlock intr_mutex");
    exit(1);
  }
  printf("Terminating\n");
  exit(0);
}
