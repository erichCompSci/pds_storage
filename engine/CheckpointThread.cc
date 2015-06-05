/*
 *
 *  $Id: CheckpointThread.cc,v 1.7 2008-07-01 13:31:19 chaos Exp $
 *
 */
#include <sys/time.h>
#include <unistd.h>
#include <deque>
#include <string>

#include "Context.h"
#include "Entity.h"
#include "Domain.h"
#include "CheckpointThread.h"
#include "EngineGuard.h"

bool CheckpointThread::go = false;

static long _sleep_period = 60;
extern int dump_content_wrapper();

/***********************************************
  Function:

  Notes:
************************************************/
void 
CheckpointThread::checkpoint()
{
  // acquire a lock so that no requests are
  // processed during checkpointing
  WriterGuard guard;
  dump_content_wrapper();
}
/***********************************************
  Function:

  Notes:

************************************************/
int 
CheckpointThread::start(void* arg) 
{
  struct timeval tv;
  go = true;

  while(go) {
    tv.tv_sec = _sleep_period;  
    tv.tv_usec = 0;
    select (0, 0, 0, 0, &tv);
    checkpoint();	  
  }
  return 0;
}
int
CheckpointThread::stop(void* arg)
{
  go = false;
  return 0;
}
/***********************************************
  Function:

  Notes:

************************************************/
void 
CheckpointThread::sleep_period(long period)
{
  _sleep_period = period;
}
