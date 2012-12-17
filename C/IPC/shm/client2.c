#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <string.h>

#include "cliserv2.h"

int
main(int argc, char **argv)
{
  int               i, nloop, nusec;
  pid_t             pid;
  char              msg[MESGSIZE];
  long              offset;
  struct shmstruct *ptr;

  if (argc != 4) {
    printf("usage: client2 <name> <#loops> <#usec>\n");
    exit(1);
  }

  nloop = atoi(argv[2]);
  nusec = atoi(argv[3]);

  /* create shared memory segment */
}
