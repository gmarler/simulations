/* Copyright 2004 Sun Microsystems, Inc. ALL RIGHTS RESERVED
   Use of this software is authorized pursuant to the 
   terms of the license found at http://developers.sun.com/berkeley_license.html

   Listing 2.  A threaded example of using ports to reap fd status using POLLIN events
   Compile with Sun Studio 9: cc -mt -o port_poll listing2_poll.c
   Compile with gcc 3.4.1: gcc -o port_poll listing2_poll.c

   Usage: ./port_poll  <Number of threads> <Number of fds> <Iterations per fd>
   Usage example: ./port_poll 2 2 2
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <port.h>
#include <sys/port_impl.h>
#include <fcntl.h>
#include <sys/stat.h>

extern long random(void);

int numfds = 0;
int iteration = 0;

#define	TESTMSG		"hello"
#define	TESTFILE	"/tmp/port"
#define	TESTFSUF	"node"

void
port_close(int *fds, pthread_t mythid, int fdcnt, int fds_close){
  char	*start;
  char	*name;
  char	prefix[40];
  char	number[40];

  int i;
  for(i = 0;i < fdcnt; i++){
    if(fds_close)
      close(fds[i]);

    /* The following creates the filename to delete */
    start = &number[0];
    sprintf(start,"%d",i);
    sprintf(prefix,"%s%d%s",TESTFILE,mythid,TESTFSUF);
    name = strcat(prefix, start);

    /* Remove the file */
    unlink(name);
  }
}

int
dissociate_objects(int port, int *fds, int numfds, pthread_t mythid){
  int	i;
  int	result;

  for(i = 0;i < numfds; i++){
    result = port_dissociate(port, PORT_SOURCE_FD, (uintptr_t)fds[i]);
    if(result == -1){
      perror("port_dissociate failed.");
      port_close(fds, mythid, i, 1);
      close(port);
      return (result);
    }
  }
  return (0);
}

/* The thread will execute the following function once is it created.
   This function does the following:
   a) Creates a number of fifos file descriptors (fds)
   b) Registers all fds to detect POLLIN event
   c) Executes a loop which:
   i)   Writes randomly to a fifo 
   ii)  Waits for event completion and read data out of the fifo
   iii) Re-enables fifo
   */

void *
port_poll(){
  int		i;
  int		result;
  int 		error;

  int 		port;
  int		ifd;
  int		*fds;

  char		rbuf[40];
  pthread_t	mythid;
  char            prefix[40];
  char            number[40];
  char		*start = NULL;
  char		*name = NULL;

  long		rn;
  int		loopcnt;
  hrtime_t	time_start, time_end;
  hrtime_t	time_duration = 0;
  struct timespec timeout;
  uint_t		nget;
  port_event_t	*pevl;

  if((fds = (int *)calloc(numfds, sizeof (long))) == NULL){
    printf("memory allocation fail.\n");
    exit(1);
  }

  pevl = calloc(numfds + 1, sizeof (port_event_t));

  /* Create test files */
  mythid = pthread_self();
  printf("\nThread ID = %d\n",mythid);
  for(i = 0;i < numfds; i++){
    start = &number[0];
    sprintf(start,"%d",i);
    sprintf(prefix,"%s%d%s",TESTFILE,mythid,TESTFSUF);
    name = strcat(prefix, start);

    error = mkfifo(name, S_IRWXU | S_IRWXG | S_IRWXO);
    if(error){
      char *err = strcat("mkfifo(3C) for ", name);
      err = strcat(name, "\n");
      perror(err);
      port_close(fds, mythid, i, 0);
      return (NULL);
    }
    if((ifd = open(name, O_RDWR)) < 0){
      char *err = strcat("mkfifo(3C) for ", name);
      err = strcat(name, "\n");
      perror(err);
      port_close(fds, mythid, i, 1);
      return (NULL);
    }
    fds[i] = ifd;
  }

  /* Create an event port */
  port = port_create();
  if(port < 0){
    perror("creation of event port failed");
    port_close(fds, mythid, i, 1);
    return (NULL);
  }


  /* Associate all of the file descriptors with the port */
  for(i = 0;i < numfds; i++){
    result = port_associate(port, PORT_SOURCE_FD, (uintptr_t)fds[i], POLLIN, (void *)i);

    if(result == -1){
      perror("port_associate failed.");
      close(port);
      port_close(fds, mythid, i, 1);
      return (NULL);
    }
  }


  /* The follwing section of code writes to the pipes and then waits with port_getn(3C) for
     the events 
     */
  timeout.tv_sec = 5;
  timeout.tv_nsec = 0;
  nget = 1;
  loopcnt = 0;

  while(loopcnt < iteration){
    rn = random();
    rn %= numfds;
    if(write(fds[rn], TESTMSG, strlen(TESTMSG)) != strlen(TESTMSG)){
      perror("write to fifo failed.");
      close (port);
      port_close(fds, mythid, i, 1);
      return (NULL);
    }

    time_start = gethrtime();

    error = port_getn(port, pevl, numfds < PORT_MAX_LIST ? 
        numfds : PORT_MAX_LIST, &nget, &timeout);

    time_end = gethrtime();
    time_duration += (time_end - time_start);
    result = nget;

    if(error){
      perror("port_getn failed [port_getn(3C)] ");
      port_close(fds, mythid, i, 1);
      close (port);
      return (NULL);
    }
    if(result != 1){
      printf("port_getn returned %d fds.\n", result);
      port_close(fds, mythid, i, 1);
      close (port);
      return (NULL);
    }

    if(pevl->portev_object != fds[rn]){
      perror("port_getn failed to return right fd");
      port_close(fds, mythid, i, 1);
      close (port);
      return (NULL);
    }

    if(pevl->portev_events != POLLIN){
      printf("port_getn(3C) failed to return POLLIN events");
      printf("Events returned = 0x%x\n",pevl->portev_events);
      port_close(fds, mythid, i, 1);
      close (port);
      return (NULL);
    }

    if(read(fds[rn], rbuf, strlen(TESTMSG)) != strlen(TESTMSG)){
      perror("read from fifo failed");
      port_close(fds, mythid, i, 1);
      close (port);
      return (NULL);
    }

    rbuf[strlen(TESTMSG)] = '\0';
    loopcnt++;

    /* Reassociate the fd with the port */
    result = port_associate(port, PORT_SOURCE_FD, pevl->portev_object, 
        POLLIN, pevl->portev_user);
  }

  /* Starting to tear down the port by disassociating the fds from the port */
  result = dissociate_objects(port, fds, numfds, mythid);
  if(result){
    printf("Could not dissociate objects from the port\n");
    return (NULL);
  }

  /* Close the port */
  port_close(fds, mythid, i, 1);

  printf("The average time to poll %d fds is %lld nsec\n", 
      numfds, time_duration/loopcnt);
  close(port);
}


int
main(int argc, char *argv[]){
  int		ret;
  int 		numthreads = 0;
  pthread_t	*threads;
  pthread_attr_t	tattr;
  void		*status;
  void *(*func_ptr)();

  if(argv[1] != NULL)
    numthreads = atoi(argv[1]);	/* Number of threads used */
  if(argv[2] != NULL)
    numfds = atoi(argv[2]);		/* Number of file descriptors created */
  if(argv[3] != NULL)
    iteration = atoi(argv[3]);	/* Iterations on one file descriptor */

  if(argc != 4 || numthreads < 1 || numfds < 1 || iteration < 1){
    printf("Usage: %s <Number of threads> <Number of fds> "
        "<Iterations per fd>\n", argv[0]);
    exit (-1);
  }

  /* Initialize with default */
  if(ret = pthread_attr_init(&tattr)){
    perror("Error initializing thread attribute [pthread_attr_init(3C)] ");
    return (-1);
  }

  /* Make it a bound thread */
  if(ret = pthread_attr_setscope(&tattr, PTHREAD_SCOPE_SYSTEM)){
    perror("Error making bound thread [pthread_attr_setscope(3C)] ");
    return (-1);
  }

  /* Create the threads */
  threads = calloc(numthreads, sizeof(pthread_t));
  func_ptr = port_poll;
  int i;
  for(i = 0;i < numthreads; i++){
    if(ret = pthread_create(&threads[i], &tattr, func_ptr, (void *)i)){
      perror("Error starting child threads [pthread_create(3C)] ");
      return (-1);
    }
  }

  printf("Main(): Waiting for threads to finish\n");
  for(i = 0;i < numthreads; i++)
    pthread_join(threads[i], &status);

  return (0);
}
