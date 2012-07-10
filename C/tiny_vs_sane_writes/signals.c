#include "signals.h"

void *sig_handler(void *arg)
{
  sigset_t handler_sigset;
  int      signum, status;

  sigemptyset(&handler_sigset);
  sigaddset( &handler_sigset, SIGINT );
  sigaddset( &handler_sigset, SIGRTMIN );

  while (1) {
    status = sigwait( &handler_sigset, &signum );
    if ( status != 0 ) {
      perror("sigwait failure");
      exit(1);
    }
    if (signum == SIGINT) {
      printf("Received SIGINT\n");
    }
    else if (signum == SIGRTMIN) {
      /* Signal writing thread that it should write data */
    }
  }
}
