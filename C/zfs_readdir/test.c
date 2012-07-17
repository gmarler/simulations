#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>

/* Compile with -D_POSIX_PTHREAD_SEMANTICS OR
 * -D_POSIX_C_SOURCE=199506L */

int
main( int argc, char **argv )
{
  int  i;
  char *tdir;
  char tdir_template[15] = "";
  char file_template[40] = "";

  strlcpy( tdir_template, "tempdir_XXXXXX", sizeof(tdir_template) );
  tdir = mkdtemp( tdir_template );;

  printf("tempdir: %s\n",tdir);

  for (i = 0; i < 5; i++) {
    strlcpy( file_template, tdir, sizeof(file_template) );
    strcat( file_template, "/" );
    strcat( file_template, "zfs_readdir_XXXXXXXXXXX" );
    mkstemp( file_template );
  }
}
