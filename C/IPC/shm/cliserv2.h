
#define MESGSIZE         256                    /* max #bytes per message, incl. null at end */
#define NMESG          50000                    /* max #messages */

struct shmstruct {                              /* struct stored in shared memory */
  sem_t    mutex;                               /* 3 Posix memory-based semaphores */
  sem_t    nempty;
  sem_t    nstored;
  long     nput;                                /* index into msgoff[] for next put */
  long     noverflow;                           /* #overflows by senders */
  sem_t    noverflowmutex;                      /* mutex for noverflow counter */
  long     msgoff[NMESG];                       /* offset in shared memory of each message */
  char     msgdata[NMESG * MESGSIZE];           /* the actual messages */
};

