
#include "tempfile.h"
#include <dirent.h>
#include <sys/stat.h>

int main(void)
{
  char *mydir = "./TDIR";
  int i;
  size_t fsize;
  DIR *dir;
  struct dirent *ent;
  struct stat stat_buf;
  char path[1024];

  for (i = 0; i < 25; i++) {
    fsize = rand_tempfile_size();
    create_tempfile(mydir,fsize);
  }

  dir = opendir(mydir);
  if (dir != NULL) {
    /* skip '.' and '..' */
    if ((readdir(dir) != NULL) && (readdir(dir) != NULL)) {
      while((ent = readdir(dir)) != NULL) {
        snprintf(path, 1024, "%s/%s", mydir, ent->d_name);
        if (lstat(path, &stat_buf) == -1) {
          perror("lstat failed");
          continue;
        }
        switch (stat_buf.st_mode & S_IFMT) {
          case S_IFREG:
            printf("Found file %s\n",path);
            break;

          default:
            continue;
        }
      }
    }
  }
}
