#include "writer.h"

/* Non-optimal writing, with tiny writes */

void *tiny_writer(void * arg)
{
  /* Go into an effectively infinite loop, waiting on a condition variable to
   * decide when to write data very inefficiently */

}
