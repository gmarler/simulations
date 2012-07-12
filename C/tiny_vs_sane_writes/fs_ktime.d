#!/usr/sbin/dtrace -s

#pragma D option bufsize=512k
#pragma D option aggsize=512k
#pragma D option quiet
#pragma D option cleanrate=397Hz
#pragma D option dynvarsize=5m


fbt:ufs::entry,
fbt::zfs:entry
{
  self->fsfunc[probefunc,stackdepth] = vtimestamp;
}

fbt:ufs::return
/ self->fsfunc[probefunc,stackdepth] /
{
  @on_cpu["UFS ON CPU"] = sum(vtimestamp - self->fsfunc[probefunc,stackdepth]);
}

fbt:zfs::return
/ self->fsfunc[probefunc,stackdepth] /
{
  @on_cpu["ZFS ON CPU"] = sum(vtimestamp - self->fsfunc[probefunc,stackdepth]);
}

fbt:ufs::return,
fbt:zfs::return
/ self->fsfunc[probefunc,stackdepth] /
{
  self->fsfunc[probefunc,stackdepth] = 0;
}

tick-30sec
{
  printf("\n%Y\n",walltimestamp);

  /* normalize(@on_cpu,1000000); */

  printa(@on_cpu);

  trunc(@on_cpu);
}

