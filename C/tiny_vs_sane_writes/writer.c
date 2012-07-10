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

/* a writer that buffers at least 1 MB before writing them out to the filesystem */
void *sane_writer(void *arg)
{
  int         status, i, fd;
  static int  writes_performed = 0;
  my_data_t   jdata;
  char       *buf1, *buf2, *cur_buf;
  size_t      buf_offset = 0;

  /* Allocate 2 x 2 MB buffers, since we might overflow 1 MB a little */
  buf1 = malloc(2 * 1024 * 1024);
  if ( ! buf1 ) {
    perror("Error allocating write buffer #1");
    exit( 1 );
  }
    
  buf2 = malloc(2 * 1024 * 1024);
  if ( ! buf2 ) {
    perror("Error allocating write buffer #2");
    exit( 1 );
  }

  /* The "current buffer" is initially buf1 */
  cur_buf = buf1;

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
        memcpy(&jdata.header,  cur_buf + buf_offset,          21);
        memcpy(&jdata.payload, cur_buf + buf_offset + 21,     60);
        memcpy(&jdata.trailer, cur_buf + buf_offset + 21 + 1,  1);
        buf_offset += (size_t)(21 + 60 + 1);
      }

      /* TODO: Make this a constant or somesuch */
      if ( buf_offset >= (1024 * 1024) ) {
        writes_performed++;
        write(fd, cur_buf, buf_offset);
        cur_buf == buf1 ? buf2 : buf1;
        buf_offset  = 0;
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
