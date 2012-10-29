
#include "tempfile.h"
#include <dirent.h>

int main(void)
{
  char *mydir = "./TDIR";
  int i;
  size_t fsize;
  DIR *dir;
  struct dirent *ent;

  for (i = 0; i < 750; i++) {
    fsize = rand_tempfile_size();
    create_tempfile(mydir,fsize);
  }

  dir = opendir(mydir);
  if (dir != NULL) {
    while((ent = readdir(dir)) != NULL) {
      filename = ent->d_name;
    }
  }

}
