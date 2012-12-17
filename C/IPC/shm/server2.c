#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#include "cliserv2.h"

/* Default perms for SysV Shared Memory */
#define SVSHM_MODE (SHM_R | SHM_W | SHM_R >> 3 | SHM_R >> 6)

int
main(int argc, char **argv)
{
  int               id, lastnoverflow, temp;
  long              index, offset;
  struct shmstruct *ptr;

  if (argc != 2) {
    printf("usage: server2 <name>\n");
    exit(1);
  }

  /*-----------------------------------------------------------------------------
   *  create shm segment
   *-----------------------------------------------------------------------------*/
  id = shmget(ftok("/tmp/test",1), sizeof(struct shmstruct), IPC_CREAT | SVSHM_MODE);

  ptr = shmat(id, NULL, 0);

  /* Initialize the array of offsets */
  for (index = 0; index < NMESG; index++) {
    ptr->msgoff[index] = index * MESGSIZE;
  }

  /* Initialize the semaphores in shared memory */
  sem_init(&ptr->mutex, 1, 1);
  sem_init(&ptr->nempty, 1, NMESG);
  sem_init(&ptr->nstored, 1, 0);
  sem_init(&ptr->noverflowmutex, 1, 1);

  /* This program is the consumer */
  index = 0;
  lastnoverflow = 0;
  for ( ; ; ) {
    sem_wait(&ptr->nstored);
    sem_wait(&ptr->mutex);
    offset = ptr->msgoff[index];
    printf("index = %ld: %s\n", index, &ptr->msgdata[offset]);
    if (++index >= NMESG)
      index = 0;                                /* circular buffer */
    sem_post(&ptr->mutex);
    sem_post(&ptr->nempty);

    sem_wait(&ptr->noverflowmutex);
    temp = ptr->noverflow;                      /* don't printf while mutex held */
    sem_post(&ptr->noverflowmutex);
    if (temp != lastnoverflow) {
      printf("noverflow = %d\n", temp);
      lastnoverflow = temp;
    }
  }
  exit(0);
}
