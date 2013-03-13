/*
 * =====================================================================================
 *
 *       Filename:  tempfile.c
 *
 *    Description:  Library of routines for temporary file manipulation
 *
 *        Version:  1.0
 *        Created:  10/29/12 11:41:12 AM
 *       Revision:  none
 *       Compiler:  cc (Studio 12)
 *
 *         Author:  Gordon Marler
 *   Organization:  
 *
 * =====================================================================================
 */

#include "tempfile.h"

char *
create_tempfile(char *dirpath, size_t size)
{
  char    data_buf[65536];
  char    fname_buf[512];
  char   *fname_ret; /* Temp filename we return */
  ssize_t nread,
          bytes_to_copy,
          to_read;
  int     rand_fd,
          temp_fd;
  int     i, saved_errno;

  if ((rand_fd = open("/dev/zero",O_RDONLY)) == -1 ) {
    perror("Unable to open /dev/zero");
    return NULL;
  }

  /* prime the string before strcat'ing to it */
  strncpy(fname_buf,dirpath,512);
  strcat(fname_buf,"/");
  strcat(fname_buf,"tempXXXXXX");
  /* strncpy(fname_buf,strcat(dirpath,strcat("/","tempXXXXXX")),512); */
  /*  printf("Going to create %s\n",fname_buf); */

  if ((temp_fd = mkstemp(fname_buf)) == -1) {
    perror("mkstemp");
    return NULL;
  }

  bytes_to_copy = size;

  while (bytes_to_copy > 0) {
    if (bytes_to_copy >= sizeof(data_buf)) {
      to_read = sizeof(data_buf);
    } else { to_read = bytes_to_copy; }

    nread = read(rand_fd, data_buf, to_read);
    char *out_ptr = data_buf;
    ssize_t         nwritten;

    do {
      nwritten = write(temp_fd, out_ptr, nread);
      if (nwritten >= 0) {
        nread -= nwritten;
        out_ptr += nwritten;
        bytes_to_copy -= nwritten;
      } else if (errno != EINTR) {
        goto out_error;
      }
    } while (nread > 0);
  }

  if (nread == 0) {
    if (close(temp_fd) < 0) {
      temp_fd = -1;
      goto out_error;
    }
    close(rand_fd);

    /* success! */
    /* strlen + nul byte */
    fname_ret = malloc(strlen(fname_buf) + 1);
    strncpy(fname_ret,fname_buf,strlen(fname_buf));
    fname_ret[strlen(fname_buf)+1] = '\0';
    return fname_ret;
  }

out_error:
  saved_errno = errno;
  close(rand_fd);
  if (temp_fd >= 0)
    close(temp_fd);

  errno = saved_errno;
  perror("create_tempfile");
  return NULL;
}

size_t
rand_tempfile_size(void)
{
  /* Generate random file sizes starting at a minimum of 256 KB */
  size_t fsizes[12] = {
    256 * 1024,
    512 * 1024,
    768 * 1024,
    1024 * 1024,
    1536 * 1024,
    2048 * 1024,
    2560 * 1024,
    3072 * 1024,
    4096 * 1024,
    4608 * 1024,
    5120 * 1024,
    5632 * 1024
  };
  static int initialized = 0;

  if (initialized == 0) {
    srand(time(NULL));
    printf("Initializing random number generator\n");
    initialized++;
  }

  return(fsizes[rand() % 12]);
}
