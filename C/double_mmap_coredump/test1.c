#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define TESTBF "TESTBF"

#define PAGESIZ 8192
#define MEMSIZ  63 * 1024 * 1024
#define MINSIZ  15 * 1024 * 1024

/* DEFINE BASE MEMORY FOR DEBUGGIN THE CORE ON SUN/HP
 */
static char goodpages[PAGESIZ + PAGESIZ + MEMSIZ + PAGESIZ];
#define GOODBASE &goodpages[0]
#define GOODEND  GOODBASE + PAGESIZ + PAGESIZ + MEMSIZ + PAGESIZ
#define COREHELP_MODE 1
#define COREHELP_PROT (PROT_READ)

static char mypages[PAGESIZ + MEMSIZ + PAGESIZ];
#define BASE &mypages[0]
#define BBPRIBASE "p"

#if !defined(MAP_NORESERVE)
#define MAP_NORESERVE 0
#ifdef __sun
#error "MAP_NORESERVE should be defined on SunOS"
#endif
#endif

#ifdef COREHELP_MODE
static char *goodmem = 0;
static int corehelpon = 0;
#endif


/* 
 * Some systems (Solaris) accept the MAP_NORESERVE flag for SHARED and PRIVATE
 * mappings.
 *
 * Since it should only *do anything* for private mappings, the right thing
 * to do is to just pass it with MAP_PRIVATE;
 *
 * Define flags for internal use to get acceptable behavior on HP (and
 * existing behavior on Sun)
 *
 */
#define TEST_MAP_PRIVATE_NORESERVE MAP_NORESERVE
#define TEST_MAP_SHARED_NORESERVE  MAP_NORESERVE


/* TEST:
 *
 * 1. Create a file with all zeros
 * 2. mmap() file shared in one memory range
 * 3. mmap() file private in another memory range
 * 4. Make modifications to the shared memory region
 * 5. sleep for prolonged time, then purposely core dump the process
 * 6. Examine the two memory regions, to see if private region is
 *    updated or not
 * */

#ifdef COREHELP_MODE
    void *goodmm;
    corehelpon = 1;
#endif

int fd = open(fname, O_RDWR | O_CREAT, 0666);

int mapbit = MAP_SHARED | MAP_FIXED | TEST_MAP_SHARED_NORESERVE;

mm = mmap(mem, size, PROT_READ | PROT_WRITE, mapbit, fd, 0);

#ifdef COREHELP_MODE
  if (corehelpon) {
    /* before we map, lets make sure goodmem is within a good range */
    if ((goodmem < GOODBASE) || (goodmem >= GOODEND))
      goodmem = (char *) (((uintptr_t ) GOODBASE + PAGESIZ + PAGESIZ - 1) & -PAGESIZ);

      goodmm = mmap(goodmem, size, COREHELP_PROT,
                    MAP_PRIVATE | MAP_FIXED | TEST_MAP_PRIVATE_NORESERVE, fd, 0);
      if (goodmm == MAP_FAILED)
      {
            fprintf(stderr, TESTBF ":%s:mmap-private errno=%d (%s)",
                    __func__, errno, strerror(errno));
      }
  }
#endif

