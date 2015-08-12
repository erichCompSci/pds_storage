/*
 *
 *  $Id: basic.c,v 1.22 2008-09-24 12:39:17 eisen Exp $
 *
 */
#define _XOPEN_SOURCE 600
#include "config.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>


#include "agent/pds.h"
#include "common/formats.h"
#include "cmdline.h"
#include "common/logging.h"


const float RANGE_RAND = 10.0;
const float LOWER_RAND_BOUND = 10.0;

pds_domain_id_t new_domain_id;
CManager cm;

pds_service wps;
char *pds_host;
pds_context_id_t cid1;
pds_entity_id_t eid1;
attr_list contact_attrs;
pds_entity_float_data_t ft;
float curr_float;
int proc_id, group_id;
//Hardcoded size, if we change the entity name this has to change too
char send_group[20];

float get_rand_float() { return RANGE_RAND * ((float) rand() / (float)RAND_MAX) + LOWER_RAND_BOUND; }

int get_rand_time() { return (rand() % 2) + 1; }

static void
alarm_handler(int sig)
{
  static int count = 0;
  curr_float = get_rand_float();  
  ft.data = &curr_float;
  ft.data_size = 1;

  printf("Setting the data to %f\n", curr_float);
  if((pds_set_entity_float_data(new_domain_id, send_group, cid1, &ft, 0)) <= 0 ) 
  {
    fprintf(stderr, "Error: client_modify #%d:%d could not set /experimental/pool data\n", proc_id, group_id);
  }
  if (count == 14)
  {
    struct rusage resource_usage;
    if(getrusage(RUSAGE_SELF, &resource_usage))
    {
      fprintf(stderr, "Error: couldn't get the resource_usage\n");
    }
    else
    {
      if(!log_rusage(&resource_usage))
      {
        fprintf(stderr, "Error: couldn't log the resource_usage\n");
      }
    }
    count = 0;
  }
  else
    count++;
  
  alarm(2);
    
}
  
int main (int argc, char *argv[])
{
  char log_name[100];
  char * cod_func = "int i;\n\
     float total = 0;\n\
     for(i = 0; i < the_size; ++i)\n\
     {\n\
       float * temp_float_ptr;\n\
       entity_float_data_change_event * old_event;\n\
       old_event = EVdata_entity_float_data_change_event(i);\n\
       temp_float_ptr = &(old_event->float_data.data[0]);\n\
       total = total + (*temp_float_ptr);\n\ 
     }\n\
     float average;\n\
     average = total / the_size;\n\
     printf(\"The value of average is: \\%f\\n\", average);\n\
     entity_float_data_change_event new_event;\n\
     entity_float_data_change_event * old_event = EVdata_entity_float_data_change_event(0);\n\
     new_event.float_data.data_size = 1;\n\
     new_event.float_data.data[0] = average;\n\
     for(i = 0; i < 32; ++i)\n\
     {\n\
       new_event.entity_id.id[i] = old_event->entity_id.id[i];\n\
     }\n\
     EVsubmit(0, new_event);\n\0"; 


  if (argc != 3)
  {
    fprintf(stderr, "Usage: client_modify pos_unique_number [123], where unique_number is a positive or zero unique number for the group\n"
                    "And the second number is a identifier for which group the fprintf is submitting too...\n");
    exit(1);
  }
  
  proc_id = atoi(argv[1]);
  group_id = atoi(argv[2]);
  if (group_id != 1 && group_id != 2 && group_id != 3)
  {
    fprintf(stderr, "Error: the group id should be one, two or three.  It is: %d\n", group_id);
    exit(1);
  }


  strcpy(send_group, "/experimental/pool");
  strcat(send_group, argv[2]);

  strcpy(log_name, "modify_");
  strcat(log_name, argv[1]);
  strcat(log_name, argv[2]);

  if(!initialize_log(log_name))
  {
    fprintf(stderr, "Error: log not initialized, quitting!\n");
    exit(1);
  }

  printf("Program starting\n");

  srand(time(NULL));

  /* Set up sigaction for handler call */
  struct sigaction sa;
  sa.sa_flags = SA_RESTART;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = alarm_handler;
  if(sigaction(SIGALRM, &sa, NULL) == -1)
  {
    fprintf(stderr, "SIGALRM sigaction failed!\n");
    exit(1);
  }

  float temp_float = get_rand_float(); 


  char **bindings;
  int i2;
  atom_t VAL1_ATOM, VAL2_ATOM;

  ft.data = &temp_float;
  ft.data_size = 1;

  pds_host = getenv ("PDS_SERVER_HOST");

  if (!pds_host && !access(PDS_CONNECT_FILE, F_OK))
  {
      char hostname[128];
      FILE * temp_ptr = fopen(PDS_CONNECT_FILE, "r");
      fscanf(temp_ptr, "%s", hostname);
      printf("Hostname is: %s\n", hostname);
      fclose(temp_ptr);
      pds_host = strdup(hostname);
  }

  if (pds_host == NULL) 
      pds_host = getenv ("HOSTNAME");

  if (pds_host == NULL) {
      char hostname[128];
      if (gethostname(&hostname[0], sizeof(hostname)) == 0) {
	  pds_host = strdup(hostname);
      }
  }

  contact_attrs = create_attr_list();
  set_attr (contact_attrs,
            attr_atom_from_string("IP_HOST"),
            Attr_String,
            strdup (pds_host));
  set_attr (contact_attrs,
            attr_atom_from_string("IP_PORT"),
            Attr_Int4,
            (attr_value*)8848);

  /* Get the CM from the client_manager 
  cm = CManager_create();
  CMlisten(cm);*/

  if ((wps = pds_service_open (contact_attrs)) == NULL)
    {
      fprintf (stderr, "Couldn't init PDS client-side (is pdsd running?)\n");
      exit (1);
    }

  cm = pds_get_CManager();
  new_domain_id = pds_open_domain (wps, 
                                  "newDomain",
                                  "newDomaintype",
                                  1,
                                  "wp-register");


  cid1 = pds_get_root_context (new_domain_id);
  if((atoi(argv[1])) == 0)
  {
    eid1 = pds_create_entity_float (new_domain_id, send_group, null_pds_context_id, &ft, NULL);
    if(!pds_aggregate_entity(new_domain_id, cod_func, eid1, ENTITY_DATA_CHANGE_FLOAT))
    {
      fprintf(stderr, "Error: pds_aggregate_entity failed\n");
      return 1;
    }
  }

  

  if((pds_set_entity_float_data (new_domain_id, send_group, cid1, &ft, 0)) > 0 )
    printf("Successfully set the new float data, should see something in the other program.\n");
  else
    printf ("Failed to set the new float data\n");

  fflush (0);

  alarm(2);

  CMrun_network(cm);

  pds_service_close (wps);
  
  return 0;
}

/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.21  2007/12/04 21:06:25  pmw
 *  Merge changes from evpath branch.  Also probably includes
 *  Darwin fixes.  Compiles on isleroyale and marquesas, I think
 *  I'm misconfigured somehow for building on gondor.
 *
 *  Revision 1.20.2.2  2007/01/09 01:37:32  pmw
 *  various changes including a fix for the delete context server hang
 *
 *  Revision 1.20.2.1  2007/01/05 22:40:42  pmw
 *  interim commit for evpath changes, new branch
 *
 *  Revision 1.20  2005/12/08 23:37:06  eisen
 *  Fix includes after removing #include of cm.h from echo.h.
 *
 *  Revision 1.19  2005/08/23 19:57:00  eisen
 *  Make test use gethostname if HOSTNAME isn't set.
 *
 *  Revision 1.18  2005/02/22 19:25:30  pmw
 *  modifications to keep Entity's destructor from hamstringing Domain's
 *  destructor.  the entity list iterator in ~Domain() was being whacked
 *  by ~Entity().  If this functionality is needed, it needs to be
 *  rethought.
 *
 *  Revision 1.17  2003/02/12 20:50:07  eisen
 *  Tweaks to kill warnings on Irix compilers.
 *
 *  Revision 1.16  2003/02/03 15:30:25  chaos
 *  remove C++-style comments from C files so they compile on elvis
 *
 *  Revision 1.15  2002/10/31 23:29:53  pmw
 *  2 fixes:
 *  - single character context names (in particular "/") crashed
 *  - fixed memory stomping bug in get_binding_list when only asking for
 *  entity or context bindings
 *
 *  Revision 1.14  2002/10/31 18:20:39  pmw
 *  Fixed bug in context name mangling.
 *
 *  Revision 1.13  2002/10/30 17:59:07  pmw
 *  Fixed context creation bug when used with null_pds_context_id
 *
 *  Revision 1.12  2002/10/30 05:06:24  pmw
 *  Modifications to test new get_binding_list stuff
 *
 *  Revision 1.11  2002/10/30 04:42:01  pmw
 *  typos...
 *
 *  Revision 1.10  2002/10/30 03:53:00  pmw
 *  added parameter to get_binding_list functions to optionally get only entities
 *  or contexts
 *
 *  Revision 1.9  2002/10/28 20:45:18  pmw
 *  Fixed bug in get_binding_list handler - used wrong context ID if no
 *  context name was given
 *  Added lines to basic test to exercise binding list method
 *
 *  Revision 1.8  2002/08/08 00:43:40  eisen
 *  Eliminate the use of attr_atom_from_string() to assign atom values.
 *
 *  Revision 1.7  2002/08/07 18:51:48  eisen
 *  Widespread changes to modify how attribute handling works.  Stop using
 *  attr_atom_from_string() to get atom values.  Instead, those are set by the
 *  application.  Stop using get_attr() (api has changes).  Stop using
 *  xmit_object and start using the ATL-supplied encode/decode routines.
 *
 *  Revision 1.6  2001/10/09 22:44:09  pmw
 *  added support for gengetopt command-line parsing stuff
 *
 *  Revision 1.5  2001/10/09 21:37:57  pmw
 *  configuration changes
 *
 *  Revision 1.4  2001/10/07 22:19:51  pmw
 *  Uses PDS_SERVER_HOST env var to determine what host to contact
 *
 *  Revision 1.3  2001/09/03 20:09:10  pmw
 *  changes to deal with string-marshaled attr lists
 *
 *  Revision 1.2  2001/06/12 03:14:32  pmw
 *  minor fixes
 *
 *  Revision 1.1.1.1  2000/12/13 22:14:25  pmw
 *  imported sources
 *
 *  Revision 1.9  2000/11/29 04:37:38  pmw
 *  changed channel-handling again, Entity now performs most of its own
 *  change event sends
 *
 *  Revision 1.8  2000/11/26 07:44:23  pmw
 *  changed <strings.h> to <string.h>, to eliminate evidence of me being a
 *  dumbass.
 *
 *  Revision 1.7  2000/11/22 21:42:50  pmw
 *  proactivity based on entity events now appears to work correctly.  In particular,
 *  creation events based on the prior registration of placeholder entities and their
 *  later "creation" as real entities works.
 *
 *
 */
