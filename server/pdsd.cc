/*
 *
 *  $Id: pdsd.cc,v 1.26 2008-09-23 23:57:25 eisen Exp $
 *
 */

extern "C" {
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
  //#include <assert.h>
#include <time.h>
#include <sys/types.h>
#ifndef HAVE_WINDOWS_H
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include <signal.h>
#else
#include <windows.h>
#include <winsock.h>
#endif

#include "../engine/EngineGuard.h"
#include "pthread.h"
#include "atl.h"
#include "ffs.h"
#include "evpath.h"
#include "cmrpc.h"


};

extern "C"
{
#include "common/formats.h"
}
#include "agent/pds.h"
#include "engine/objectId.h"
#include "engine/Proactive.h"
#include "engine/Domain.h"
#include "engine/CleaningThread.h"
#include "engine/CheckpointThread.h"
int debug_flag;
CManager server_cm;
int verbose = 0;

bool keep_going = true;

extern void register_handlers (CManager cm);
extern attr_list get_contact_list();
extern void set_server_addr();
extern int bulk_load_from_URL (const char* url);
extern int dump_content_wrapper();

extern "C" int handle_http_request (CMConnection cmc, CMTransport transport, 
				    char *buffer, long length);
extern char* dump_repository_path_;
static char* xml_file_name_ = 0;

extern void
shutdown_server()
{
  keep_going = false;
  CheckpointThread::stop(NULL);
  CleaningThread::stop(NULL);
  CMrpc_shutdown();
  pdsTrace_out (pdsdVerbose, "pdsd shutting down");
  CManager_close (server_cm);
}

extern "C" void
sigusr1_handler (int arg)
{
  dump_content_wrapper();
}
void usage(char* prog)
{
  printf("Usage: %s [-report][-quiet][-no_fork][-h][-f <file_name>][-p <path>]\n", prog);
  printf("          -report: report server startup\n");
  printf("          -quiet\n");
  printf("          -no_fork: run server in foreground\n");
  printf("          -h: display help information\n");
  printf("          -f <file_name>: load intial content from file_name\n");
  printf("          -p: run in persistent mode, server will checkpoint to path\n");
  printf("              checkpoint file is pdsd<pid>.xml\n");


}

extern "C" int
clean_thread(void*arg)
{
  return CleaningThread::start(arg);
}

extern "C" int
checkpoint_thread(void*arg)
{
  return CheckpointThread::start(arg);
}

extern int
main (int argc, char **argv)
{
  int no_fork = 0;
  int quiet = 0;
  CManager test_cm;
  attr_list try_contact_info;
  attr_list my_contact_info;
  int report = 0;

  test_cm = CManager_create();

  try_contact_info = create_attr_list();
  set_attr (try_contact_info,
	    attr_atom_from_string("IP_HOST"),
	    Attr_String,
	    strdup("localhost"));
  set_attr (try_contact_info,
	    attr_atom_from_string("IP_PORT"),
	    Attr_Int4,
	    (attr_value) PDS_SERVER_PORT);
  my_contact_info = create_attr_list();
  set_attr (my_contact_info,
	    attr_atom_from_string("IP_PORT"),
	    Attr_Int4,
	    (attr_value) PDS_SERVER_PORT);

  while (argv[1] && (argv[1][0] == '-')) 
    {
      if (strcmp(argv[1], "-no_fork") == 0) 
        {
	    no_fork = 1;
        } 
      else if (strcmp(argv[1], "-quiet") == 0) 
        {
	    quiet = 1;
        } 
      else if (strcmp(argv[1], "-report") == 0) 
        {
	    report = 1;
        } 
      else if (argv[1][1] == 'c') 
        {

        } 
      else if (argv[1][1] == 's') 
        {

        } 
      else if (argv[1][1] == 'q') 
        {

        } 
      else if (argv[1][1] == 'v') 
        {
	  verbose = 1;
        } 
      else if (argv[1][1] == 'h') 
        {
          usage(argv[0]);
		  exit(0);
        } 
      else if (argv[1][1] == 'f') 
        {
          /* set the repo path for the load operation */
          xml_file_name_ = (char*) malloc(strlen(argv[2]) + 1);
          strcpy(xml_file_name_ , argv[2]);          
		      argv++;
        }
      else if (argv[1][1] == 'p') 
        {        
          dump_repository_path_ = (char*) malloc(strlen(argv[2]) + 1);
          strcpy(dump_repository_path_, argv[2]);
		  argv++;
        }
      else
        {
	    fprintf(stderr, "Unknown argument \"%s\"\n", argv[1]);
	    exit(0);
	    }
      argv++;
    }


  // initialize the engine mutex
  ReaderGuard::initialize();

  if (argc > 1)
    {
      debug_flag = 1;
    }

  CMConnection conn = CMinitiate_conn(test_cm, try_contact_info);
  if (conn != NULL) {
      if (!quiet) printf("service already running locally\n");
      CMConnection_close(conn);
      exit(0);
  }
  CManager_close(test_cm);

  if (!no_fork) {
      if (!quiet) {
	  printf("Forking server to background\n");
      }
      if (fork() != 0) {
	  /* I'm the parent, return now */
	  exit(0);
      }
  } else {
      if (!quiet) {
	  printf("Running...\n");
      }
  }

  signal (SIGUSR1, sigusr1_handler);

  server_cm = CManager_create();
  pdsTrace_out (pdsdVerbose, "registering handlers...");
  register_handlers (server_cm);

  pdsTrace_out (pdsdVerbose, "setting up network...");
  if (CMlisten_specific (server_cm, my_contact_info) == 0)
    {
      fprintf (stderr, "CMlisten_specific failed, contact info follows\n");
      dump_attr_list (my_contact_info);
      exit(1);
    }

  free_attr_list (my_contact_info);
  objectId::set_server_addr (CMget_contact_list (server_cm));
  Proactive::use_this_CM (server_cm);
  

  if (verbose)
    {
      dump_attr_list (CMget_contact_list(server_cm));
    }


  if (report) {
    struct stat stat_buf;
    static int restart = 0;
    if (restart == 0) {
	    restart++;
	    if (stat("/users/c/chaos/bin/pds_server_report", &stat_buf) == 0) {
	      (void)system("/users/c/chaos/bin/pds_server_report restart");
	    }
    }
  }

  // check if the -f option was set
  if (xml_file_name_ != 0) {
    if (! bulk_load_from_URL (xml_file_name_))
      {
        pdsTrace_out (pdsdVerbose, "bulk load failed");
      }
    else
      {
        pdsTrace_out (pdsdVerbose, "bulk load succeeded");
      }
    /* unset the repo path after load is finished */
    free(xml_file_name_ );
    xml_file_name_ = 0;
  }

  /*
  fork a special thread for garbage collection
  this was specifically added for GS APIs but 
  it could be useful in other applications
  */
  {
      pthread_t gc_thread, cp_thread;
      pthread_create(&gc_thread, NULL, (void*(*)(void*))clean_thread, 0);
      //pthread_create(&cp_thread, NULL, (void*(*)(void*))checkpoint_thread, 0);
  }

  /*
   *  Add the HTTP non-CM message handler
   */
  CMregister_non_CM_message_handler (0x20544547, handle_http_request);
  CMregister_non_CM_message_handler (0x47455420, handle_http_request);

  CMrun_network (server_cm);
}

/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.25  2008/06/26 19:24:19  pmw
 *  Various fixes related to evpath handling
 *
 *  Revision 1.24  2008/01/01 00:38:21  pmw
 *  Changes to make PDS work with the new FFS libraries
 *
 *  Revision 1.22  2007/12/04 21:06:24  pmw
 *  Merge changes from evpath branch.  Also probably includes
 *  Darwin fixes.  Compiles on isleroyale and marquesas, I think
 *  I'm misconfigured somehow for building on gondor.
 *
 *  Revision 1.21.2.1  2007/01/05 22:40:39  pmw
 *  interim commit for evpath changes, new branch
 *
 *  Revision 1.21  2004/11/05 22:02:57  mansour
 *  Improve PDS performance by using R/W lock instead of a mutex and also cache the XML document instead of re-creating it on each query.
 *
 *  Revision 1.20  2004/10/14 21:17:32  pmw
 *  Changes to support the CMrpc renaming.
 *
 *  Revision 1.19  2004/05/05 12:20:00  eisen
 *  Fix this so that it compiles with changes to CMNonCMHandler.
 *
 *  Revision 1.18  2003/10/21 19:54:14  eisen
 *  Various tweaks and hacks designed to get this to compile cleanly with
 *  Forte 7.
 *
 *  Revision 1.17  2003/10/21 16:03:08  eisen
 *  Changes to avoid warnings and errors and generally make this compile with
 *  SUNWspro7 CC with the -library=stlport4 argument.  Using this correctly
 *  requires setting CXX=CC, CXXFLAGS=-library=stlport4,
 *  CXXCPP="CC -E -library=stlport4".  Chaos is setup to do this now.
 *
 *  Revision 1.16  2003/10/07 17:20:26  pmw
 *  fixed references to dump_server_content_wrapper
 *  re-insert errno.h inclusion
 *  CMTransport function spec related changes (again)
 *
 *  Revision 1.15  2003/10/07 15:13:46  chaos
 *  Tweak non-cm handler to answer changes in CM.  Make load.cc compile where we don't have XML and need a header to reference errno.
 *
 *  Revision 1.14  2003/10/06 14:08:17  mansour
 *  Move dump_server_content_wrapper to libengine
 *
 *  Revision 1.13  2003/09/29 17:31:15  mansour
 *  move the variable definition to pdsd.cc, this fixes a link problem on marquesas
 *
 *  Revision 1.12  2003/09/29 00:57:50  mansour
 *  Added a checkpointing thread.
 *  Fix the code for XML dump and load
 *
 *  Revision 1.11  2003/07/29 03:48:52  pmw
 *  initial changes to support HTTP requests
 *
 *  Revision 1.10  2003/04/07 04:25:49  mansour
 *  Added a thread to delete stale contexts.
 *
 *  Revision 1.9  2003/02/13 20:00:37  pmw
 *  Moved fork() call so as to not hose CMRPC initialization.  Look
 *  on mware-dev for details.
 *
 *  Revision 1.8  2003/01/23 17:59:26  eisen
 *  "Harden" the pds server to make it suitable for running from cron.  The
 *  server now accepts the following additional flags
 *    -no_fork	- tells the server *not* to fork itself to the background
 *    -quiet	- suppresses default output about forking and running
 *    -report	- tells the server to run /users/c/chaos/pds_server_report
 *  		  when appropriate (now only on restart)
 *
 *  Add the ability to test to see if a PDS server is running on the current
 *  host (currently done by checking to see if we can establish a CM connection
 *  to it).
 *
 *  Add the ability to fork to the background if no other server is found.
 *
 *  Revision 1.7  2002/08/07 18:51:46  eisen
 *  Widespread changes to modify how attribute handling works.  Stop using
 *  attr_atom_from_string() to get atom values.  Instead, those are set by the
 *  application.  Stop using get_attr() (api has changes).  Stop using
 *  xmit_object and start using the ATL-supplied encode/decode routines.
 *
 *  Revision 1.6  2002/08/05 22:49:33  pmw
 *  Changes to support gcc 3.0 and xerces-c 2.0
 *
 *  Revision 1.5  2002/03/28 04:49:40  pmw
 *  I have to call CMlisten_specific on the CM value before I give it to
 *  the Proactive class.  Proactive calls ECho_CM_init() on it, which does
 *  a regular CMlisten() if no form of listen had been called before.  This
 *  screwed up the object ID address scheme because the port number was all messed
 *  up.
 *
 *  Revision 1.4  2002/03/08 21:05:38  pmw
 *  Server can now dump its contents to an XML stream
 *
 *  Revision 1.3  2001/10/15 19:58:14  pmw
 *  added new trace framework, shamelessly copied from Greg
 *
 *  Revision 1.2  2001/10/12 00:15:54  pmw
 *  fixed resolution of server IP address - uses CM's contact list value now
 *
 *  Revision 1.1  2001/10/09 21:37:56  pmw
 *  configuration changes
 *
 *  Revision 1.3  2001/10/07 22:19:15  pmw
 *  Added domain_change event structures so that Proactive class is always correctly
 *  initialized.
 *
 *  Revision 1.2  2001/06/26 19:49:32  pmw
 *  fixed several small things on last bughunt.  pdsd.cc now does gen_pthread_init().
 *
 *  Revision 1.1.1.1  2000/12/13 22:14:21  pmw
 *  imported sources
 *
 *  Revision 1.10  2000/12/11 19:42:09  pmw
 *  Now uses CMrun_network() instead of CMpoll_network() - no more non-blocking
 *  select's
 *
 *  Revision 1.9  2000/11/29 04:37:37  pmw
 *  changed channel-handling again, Entity now performs most of its own
 *  change event sends
 *
 *  Revision 1.8  2000/11/21 00:23:56  pmw
 *  Since I didn't have enough to do, a little code reorganization to keep
 *  things from getting dramatically out of hand.  Fixed a couple of
 *  bugs-in-waiting.
 *
 *  Revision 1.7  2000/11/19 23:28:32  pmw
 *  wp_server now supports bulk load of data with -f command-line option.  Also added
 *  command message so bulk loads can happen on-line.
 *
 *  Revision 1.6  2000/10/25 21:44:50  pmw
 *  Compiles after major revisions to support multiple WP services on
 *  client-side.  This should eventually allow a client to use services by
 *  linking in the library directly or by communicating using CMRPC.
 *
 *  Revision 1.5  2000/05/15 21:15:49  pmw
 *  minor changes
 *
 *  Revision 1.4  2000/04/10 20:00:13  pmw
 *  completed port to CM from DataExchange
 *
 *  Revision 1.3  1999/12/21 18:47:57  pmw
 *  Too many changes - really shouldn't have done this many
 *
 *
 */
