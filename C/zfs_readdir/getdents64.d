#!/usr/sbin/dtrace -s

#pragma D option bufsize=512k
#pragma D option aggsize=512k
#pragma D option quiet

syscall::getdents64:entry
{
  @s[stack(),ustack()] = count();
}

tick-1sec
{
  printf("\n%Y\n",walltimestamp);
  printa(@s);
  trunc(@s);
}
