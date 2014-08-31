#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define PAGESIZ 8192
#define MEMSIZ  63 * 1024 * 1024

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
