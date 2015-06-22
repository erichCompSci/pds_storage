/*
 *
 *  $Id: CleaningThread.cc,v 1.15 2008-07-01 13:31:19 chaos Exp $
 *
 */
#define _REENTRANT
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <deque>
#include <string>


#include "Context.h"
#include "Entity.h"
#include "Domain.h"
#include "CleaningThread.h"
#include "EngineGuard.h"

bool CleaningThread::go = false;

class InternalCleaningThread 
{
public:
  /***********************************************
    Function:

    Notes:

  ************************************************/
  static bool expired (const string& timeout, const Context& ctx) 
  {
    bindingMap& bmap = (bindingMap&)ctx.get_name_map();
    bindingMap::iterator bi;  
    for (bi = bmap.begin(); bi != bmap.end(); bi++)
      {
        if (((*bi).second.second)->is_placeholder()) continue;
        if ((*bi).second.first == entity_binding)
          {
            const char* name = (*bi).first;
            if (strcmp(name, _heartbeat_entry_name.c_str()) == 0) {
              Entity * entity = (Entity*) (*bi).second.second;
              pds_entity_char_data_t data = (*(entity->get_char_data()));
              if (strncmp(timeout.c_str(), (const char*)data.data, data.data_size) >= 0)
                return true;
              else 
                return false;
            } // if, HeartBeat entity
          } // if, an entity binding
      } // for, iterate over all entities
    return false;    
  }
  /***********************************************
    Function:

    Notes:

  ************************************************/
  static string get_timeout_timestamp()
  {
     struct timeval now;
     gettimeofday(&now, 0);
     char buf[100];
     sprintf(buf, "%ld", now.tv_sec - _timeout);
     string s = buf;
     return s;
  }

  static long _timeout;
  static long _sleep_period;
  static string _heartbeat_entry_name;
private:
  InternalCleaningThread();
  ~InternalCleaningThread();
};

long InternalCleaningThread::_timeout = 600; // seconds
long InternalCleaningThread::_sleep_period = 60;
string InternalCleaningThread::_heartbeat_entry_name = "gs_heartbeat";


/***********************************************
  Function:

  Notes:
    // scan all contexts in the system
    // if we find a context with an entity 
    // name HeartBeat then check
    // the hearbeat against the time out
    // if too old, then remove that context
************************************************/
void 
CleaningThread::cleanup()
{
   // acquire a lock so that no requests are
   // processed during the cleanup
   WriterGuard guard;

   Domain* d = Domain::find_first_matching_domain (
              "ECHO",
              "",
              1,
              "GroupServer");
  if (d == 0)
    return;
  
  string timeout = InternalCleaningThread::get_timeout_timestamp();

  deque<Context*> checklist;

  Context* root_context = d->get_root_context();
  checklist.push_front(root_context);
  
  do {  
    Context *current = checklist.back();
    checklist.pop_back();
    vector<Context*> to_be_removed;
    {
      const bindingMap& bmap = current->get_name_map();
      bindingMap::const_iterator bi;  \
      for (bi = bmap.begin(); bi != bmap.end(); bi++)
        {
          const char* temp = (*bi).first;
          if (((*bi).second.second)->is_placeholder()) continue;
          if ((*bi).second.first == context_binding)
            {
              Context* ctx = (Context*) (*bi).second.second;          
              if (InternalCleaningThread::expired(timeout, *ctx)) {
                to_be_removed.push_back(ctx);
                
              } // if, expired
              else {
                // add this context to the list to be checked later
                checklist.push_front(ctx);
              }
            } // if, a context binding
        } // for, scan all bindings
    }
    vector<Context*>::const_iterator iter;
    for (iter=to_be_removed.begin(); iter != to_be_removed.end(); iter++)
      d->remove_context((*iter));

  }while(checklist.size() > 0);  

}
/***********************************************
  Function:

  Notes:

************************************************/
int 
CleaningThread::start(void* arg) 
{
  struct timeval tv;

  go = true;
  while(go) {
    tv.tv_sec = InternalCleaningThread::_sleep_period;  
    tv.tv_usec = 0;
    select (0, 0, 0, 0, &tv);
    cleanup();	  
  }
  return 0;
}

int
CleaningThread::stop(void* arg)
{
  go = false;
  return 0;
}

/***********************************************
  Function:

  Notes:

************************************************/
void 
CleaningThread::sleep_period(long period)
{
  InternalCleaningThread::_sleep_period = period;
}
/***********************************************
  Function:

  Notes:

************************************************/
void 
CleaningThread::timeout(long to)
{
  InternalCleaningThread::_timeout = to;
}
/***********************************************
  Function:

  Notes:

************************************************/
void 
CleaningThread::heartbeat_entity_name(const char* entity_name)
{
  InternalCleaningThread::_heartbeat_entry_name = entity_name;
}
