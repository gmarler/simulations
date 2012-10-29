#!/usr/sbin/dtrace -Cs

#pragma D option bufsize=256k
#pragma D option aggsize=256k
#pragma D option quiet

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

BEGIN { event = 0; }

syscall::fcntl:entry
/ arg1 == F_FREESP && 
  strstr(fds[arg0].fi_pathname,"/bb/bigmem") != NULL /
{
  @c[execname] = count();
  @t["TOTAL"]  = count();

  self->ts     = timestamp;
  event++;
}

syscall::fcntl:return
/ self->ts /
{
  this->elapsed                = timestamp - self->ts;
  @lat[execname]               = quantize(this->elapsed);
  @avg["ftruncate() AVG Latency (ns)"] = avg(this->elapsed);
  @max["ftruncate() MAX Latency (ns)"] = max(this->elapsed);

  self->ts = 0;
}

tick-1sec
/ event /
{
  printf("\n%Y\n",walltimestamp);

  printa(@lat);
  printa(@c);
  printa(@t);
  printa(@avg);
  printa(@max);

  trunc(@lat);
  trunc(@c);
  trunc(@t);
  /* NOTE: We are purposely not truncating @max or @avg */

  event = 0;
}

tick-24hour
{
  exit(0);
}

