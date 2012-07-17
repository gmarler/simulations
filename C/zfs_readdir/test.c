#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>

/* Compile with -D_POSIX_PTHREAD_SEMANTICS OR
 * -D_POSIX_C_SOURCE=199506L */

int
main( int argc, char **argv )
{
  int            i, fd;
  char          *tdir;
  char           tdir_template[15] = "";
  char           file_template[40] = "";
  DIR           *dirp;
  struct dirent *dp;

  strlcpy( tdir_template, "tempdir_XXXXXX", sizeof(tdir_template) );
  tdir = mkdtemp( tdir_template );;

  printf("tempdir: %s\n",tdir);

  for (i = 0; i < 105000; i++) {
    strlcpy( file_template, tdir, sizeof(file_template) );
    strcat( file_template, "/" );
    strcat( file_template, "zfs_readdir_XXXXXXXXXXX" );
    fd = mkstemp( file_template );
    close(fd); /* don't want too many open files */
  }

  if ((dirp = opendir(tdir)) == NULL) {
    perror ("couldn't open temp dir");
    exit(1);
  }

  while (1) {
    while ((dp = readdir(dirp)) != NULL ) {
      /* do nothing - just iterating */
    }
    /* We get here when we're read the entire directory, so just rewind and
     * start again... */
    rewinddir(dirp);
  }

}
