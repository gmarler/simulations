#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <signal.h>

/* For Solaris 11, compile like so:
 * cc -D_POSIX_PTHREAD_SEMANTICS -g -o thread_sigs thread_sigs.c
 * */
int       quitflag; /* set nonzero by thread */
sigset_t      mask;

pthread_mutex_t  lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t   cwait = PTHREAD_COND_INITIALIZER;

void *
thr_fn(void *arg)
{
  int err, signo;

  for (;;) {
    err = sigwait(&mask, &signo);
    if (err != 0) {
      perror("sigwait failed");
      exit(1);
    }
    
    switch (signo) {
    case SIGINT:
      printf("\ninterrupt\n");
      break;

    case SIGQUIT:
      pthread_mutex_lock(&lock);
      quitflag = 1;
      pthread_mutex_unlock(&lock);
      pthread_cond_signal(&cwait);
      return(0);

    default:
      printf("Unexpected signal %d\n", signo);
      exit(1);
    }
  }
}

int main(void)
{
  int           err;
  sigset_t  oldmask;
  pthread_t     tid;

  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);
  sigaddset(&mask, SIGQUIT);
  sigaddset(&mask, SIGUSR1);
  if ((err = pthread_sigmask(SIG_BLOCK, &mask, &oldmask)) != 0) {
    perror("SIG_BLOCK error");
    exit(1);
  }

  err = pthread_create( &tid, NULL, thr_fn, 0);
  if (err != 0) {
    perror("can't create thread");
    exit(1);
  }

  pthread_mutex_lock(&lock);
  while (quitflag == 0)
    pthread_cond_wait(&cwait, &lock);
  pthread_mutex_unlock(&lock);

  /*  SIGQUIT has been caught and is now blocked; do whatever */
  quitflag = 0;

  /* reset signal mask which unblocks SIGQUIT */
  if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
    perror("SIG_SETMASK error");
    exit(1);
  }
  exit(0);
}



