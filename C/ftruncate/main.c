
#include "tempfile.h"
#include <dirent.h>
#include <sys/stat.h>
#include <getopt.h>

int main(int argc, char **argv)
{
  char *mydir;
  int i;
  size_t fsize;
  DIR *dir;
  struct dirent *ent;
  struct stat stat_buf;
  char path[1024];
  int fd, ret, filecount;
  int c;

  /* Flag set by `--verbose'. */
  static int verbose_flag;

  while (1) {
    static struct option long_options[] = {
      /* These options set a flag. */
      {"verbose",   no_argument, &verbose_flag, 1},
      {"brief",     no_argument, &verbose_flag, 0},
      /* The following options don't set a flag. */
      {"count",     required_argument, NULL, 'c'},
      {"dirname",   required_argument, NULL, 'd'},
      {0, 0, 0, 0}
    };
    /* getopt_long stores the option index here. */
    int option_index = 0;

    c = getopt_long (argc, argv, "c:d:",
        long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1)
      break;

    switch (c) {
      case 0:
        /* (In this example) only options which set */
        /* a flag return zero, so do nothing. */
        break;

      case 'c':
        printf ("option --count (-c) with value `%s'\n", optarg);
        filecount = atoi(optarg);
        break;

      case 'd':
        printf ("option --dirname (-d) with value `%s'\n", optarg);
        mydir = optarg;
        break;

      case '?':
        /* getopt_long already printed an error message. */
        break;

      default:
        abort ();
    }
  }

  /* Instead of reporting `--verbose'
     and `--brief' as they are encountered,
     we report the final status resulting from them. */
  if (verbose_flag)
    puts ("verbose flag is set");

  /* Print any remaining command line arguments (not options). */
  if (optind < argc) {
    printf ("non-option ARGV-elements: ");
    while (optind < argc)
      printf ("%s ", argv[optind++]);
    putchar ('\n');
  }


  for (i = 0; i < filecount; i++) {
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
            /* Test: ftruncate to 256 KB, regardless */
            if ((fd = open(path,O_RDWR)) == -1) {
              printf("Unable to open file for truncation\n");
              continue;
            }
            if ((ret = ftruncate(fd,(off_t)(256 * 1024))) == -1) {
              printf("Failed to ftruncate %s\n",path);
            }
            close(fd);
            sleep(1);
            break;

          default:
            continue;
        }
      }
    }
  }
}
