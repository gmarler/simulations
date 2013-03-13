#ifndef TEMPFILE_H
#define TEMPFILE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <strings.h>
#include <time.h>

char *create_tempfile(char *dirpath, size_t size);
size_t rand_tempfile_size(void);

#endif
