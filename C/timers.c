
#include "timers.h"

void report_resolution(void)
{
  clockid_t types[] = { CLOCK_REALTIME, CLOCK_HIGHRES, (clockid_t) - 1 };
  int i;
  struct timespec res;

  for ( i = 0; types[i] != (clockid_t) - 1; i++ )
  {
    if ( clock_getres( types[i], &res ) != 0 ) {
      printf( "Timer %d not supported.\n", types[i] );
    }
    else {
      printf( "Timer: %d, Seconds: %ld, NSECs: %ld\n", i, res.tv_sec, res.tv_nsec );
    }
  }

}

void
timer_func(unsigned int iops)
{
  struct sigaction       act;

  timer_t           itimerid;
  struct itimerspec    value;
  struct sigevent      event;
  double                  Hz;
  double                secs;

  static long    timer_enter;


  /* First, set up signal handling for when timer is activated */
  memset(&act, 0, sizeof(act));
  sigemptyset( &act.sa_mask );
  act.sa_flags     = SA_SIGINFO | SA_RESTART;
  act.sa_sigaction = timer_handler;

  if ( (sigaction(SIGRTMIN, &act, NULL)) == -1 ) {
    perror("Unable to set up SIGRTMIN signal handler\n");
    exit(1);
  }

  /* Now, create/activate timer */
  Hz = iops; /* define IOPS */
  /* Conversion from Hz to interval lengths in nsecs:
   *  ( 1 sec / Hz ) * ( 1,000,000,000 nsec / sec ) */

  /* Initial expiration of the timer */
  value.it_value.tv_sec     = 0;
  value.it_value.tv_nsec    = ( (double) 1.0 / Hz ) * 1000000000;
  /* and then often enough thereafter to make it expire Hz times a sec */
  value.it_interval.tv_sec  = 0;
  value.it_interval.tv_nsec = ( (double) 1.0 / Hz ) * 1000000000;

  /* Set up how to handle timer expiration */
  timer_enter                  = 0; /* initialize to 0 */
  event.sigev_notify           = SIGEV_SIGNAL;
  event.sigev_signo            = SIGRTMIN;
  event.sigev_value.sival_ptr  = (void *)&timer_enter;

  if ( timer_create(CLOCK_HIGHRES, &event, &itimerid) == -1 ) {
    perror("timer_create failed");
    exit(1);
  }

  if (timer_settime( itimerid, 0, &value, NULL ) == -1) {
    perror("timer_settime failed");
    exit(1);
  }
}

static void timer_handler(int signum, siginfo_t *info, void *context)
{
  long  *entry_ptr;

  switch (info->si_code) {
    case SI_TIMER:
      entry_ptr = info->si_value.sival_ptr;
      *entry_ptr++;
      /* TODO: If modulo 8000, then print a message */
      break;
    default:
      printf("UNKNOWN SIGNAL\n");
  }
}

void *sig_handler (void *arg)
{
  int sig_number;
  int signal_count = 0;
  int status, c;


  static sigset_t  myset;


  sigemptyset (&myset);
  sigaddset(&myset, SIGINT );
  sigaddset(&myset, SIGRTMIN );

  while (1) {
    sigwait( &myset, &sig_number );

    if (sig_number == SIGINT) {
      printf("Got SIGINT\n");
      status = pthread_mutex_lock( &mutex );
      if (status != 0) {
        perror("Failed to lock mutex");
        exit(1);
      }
      interrupted = 1;
      status = pthread_cond_signal( &cond );
      if (status != 0) {
        perror("Failed to signal condition");
        exit(1);
      }
      status = pthread_mutex_unlock( &mutex );
      if (status != 0) {
        perror("Failed to unlock mutex");
        exit(1);
      }
    }
    else if (sig_number == SIGRTMIN) {
      signal_count++;
      if ( signal_count % 50 == 0) {
        printf("Received another 50 timer signals\n");
      }
    }
  }
} 

