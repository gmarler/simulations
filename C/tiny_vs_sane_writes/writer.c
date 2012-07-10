#include "writer.h"

/* Non-optimal writing, with tiny writes */

void *tiny_writer(void * arg)
{
  int        status, i;
  static int writes_performed = 0;
  my_data_t  jdata;

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
        writes_performed++;
        writes_pending--;
      }
      if ( writes_performed % 50 == 0) {
        printf("Performed another 50 writes\n");
      }

    }

    status = pthread_mutex_unlock( &write_mutex );
    if (status != 0) {
      perror("Failed to unlock write mutex");
      exit(1);
    }
  }

}
