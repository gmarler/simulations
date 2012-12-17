#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>

#include "cliserv2.h"

/* Default perms for SysV Shared Memory */
#define SVSHM_MODE (SHM_R | SHM_W | SHM_R >> 3 | SHM_R >> 6)

int
main(int argc, char **argv)
{
  int               id, i, nloop, nusec;
  pid_t             pid;
  char              mesg[MESGSIZE];
  long              offset;
  struct shmstruct *ptr;

  if (argc != 4) {
    printf("usage: client2 <name> <#loops> <#usec>\n");
    exit(1);
  }

  nloop = atoi(argv[2]);
  nusec = atoi(argv[3]);

  /* attach to pre-existing shared memory segment */
  id = shmget(ftok("/tmp/test",1), sizeof(struct shmstruct), SVSHM_MODE);

  ptr = shmat(id, NULL, 0);

  pid = getpid();
  for (i = 0; i < nloop; i++) {
    /* :TODO:12/16/12 11:35:26 PM:: reimplement sleep_usec(nusec) in terms of
     * nanosleep() */
    snprintf(mesg, MESGSIZE, "pid %ld, message %d",
             (long)pid, i);

    if (sem_trywait(&ptr->nempty) == -1) {
      if (errno == EAGAIN) {
        sem_wait(&ptr->noverflowmutex);
        ptr->noverflow++;
        sem_post(&ptr->noverflowmutex);
        continue;
      } else {
        printf("sem_trywait error\n");
        exit(1);
      }
    }

    sem_wait(&ptr->mutex);
    offset = ptr->msgoff[ptr->nput];
    if (++(ptr->nput) >= NMESG)
      ptr->nput = 0;                            /* circular buffer */
    sem_post(&ptr->mutex);
    strcpy(&ptr->msgdata[offset], mesg);
    sem_post(&ptr->nstored);
  }
  exit(0);
}
