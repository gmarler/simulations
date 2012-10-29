
#include "tempfile.h"

int main(void)
{
  char *mydir = "/tmp/GM";
  int i;
  size_t fsize;

  create_tempfile(mydir,8192);

  for (i = 0; i < 25; i++) {
    fsize = rand_tempfile_size();
    printf("File Size: %ld\n",fsize);
  }
}
