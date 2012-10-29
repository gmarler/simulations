
#include "tempfile.h"

int main(void)
{
  char *mydir = "./TDIR";
  int i;
  size_t fsize;

  for (i = 0; i < 75000; i++) {
    fsize = rand_tempfile_size();
    create_tempfile(mydir,fsize);
  }

}
