#!/usr/sbin/dtrace -s

#pragma D option bufsize=512k
#pragma D option aggsize=512k
#pragma D option quiet

syscall::write:entry
/ execname == "timer_test" /
{
  @[probefunc,arg2] = count();
}

tick-1sec
{
  printf("\n%Y\n",walltimestamp);
  printa(@);
  trunc(@);
}
