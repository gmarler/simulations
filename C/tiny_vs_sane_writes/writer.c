#include "writer.h"

/* Non-optimal writing, with tiny writes */

void *tiny_writer(void * arg)
{
  int        status, i, fd;
  static int writes_performed = 0;
  my_data_t  jdata;

  /* Open the log file, truncating it */
  if ((fd = open(filename,
                 O_CREAT | O_RDWR | O_LARGEFILE | O_TRUNC,
                 S_IRUSR | S_IWUSR )) == -1) {
    perror("Unable to open file");
    exit(1);
  }
 
  /* Go into an effectively infinite loop, waiting on a condition variable to
   * decide when to write data very inefficiently */

  while (1) {
    status = pthread_mutex_lock( &write_mutex );
    if (status != 0) {
      perror("Failed to lock write mutex");
      exit(1);
    }

    while (writes_pending > 0) {
      status = pthread_cond_wait( &write_cond, &write_mutex );
      if (status != 0) {
        perror("Failed on tiny_writer wait on write_cond");
        exit(1);
      }

      for ( i = 0; i < writes_pending; i++ ) {
        write(fd, &jdata.header, 21); 
        write(fd, &jdata.payload, 60); 
        write(fd, &jdata.trailer, 1);

        writes_performed++;
      }
    }

    /* Reset writes_pending */
    writes_pending = 0;

    status = pthread_mutex_unlock( &write_mutex );
    if (status != 0) {
      perror("Failed to unlock write mutex");
      exit(1);
    }

    if ( writes_performed % iops == 0) {
      printf("Performed another %d writes\n",iops);
    }
  }

}
