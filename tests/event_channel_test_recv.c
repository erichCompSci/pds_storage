/*
 *
 *  $Id: basic.c,v 1.22 2008-09-24 12:39:17 eisen Exp $
 *
 */
#include "config.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "agent/pds.h"
#include "common/formats.h"
#include "cmdline.h"

pds_domain_id_t new_domain_id;
CManager cm;

int domain_event_handler (CManager cm, void* event, void* client_data, attr_list event_list)
{
  pds_domain_change_event_ptr evt = (pds_domain_change_event_ptr) event;
  printf("--------------------------\n");
  printf("Domain event handler called\n");  
  printf("Event descript: %s\n", evt->event_desc);
  printf("Event type: %d\n", evt->event_type);
  printf("Event fullname if applicable: %s\n", evt->event_fullname ? evt->event_fullname : "");
  printf("--------------------------\n");
  return 1;
}
  


int
entity_char_data_change_event_handler (CManager cm, void* event, void* client_data, attr_list event_list)
{
  pds_entity_char_data_change_ntf_ptr evt = (pds_entity_char_data_change_ntf_ptr) event;
  printf("-------------------------\n");
  printf("Entity data change handler called\n");

  pds_entity_char_data_t new_data = evt->char_data;
  char * printable = (char *) malloc (new_data.data_size + 1);
  memcpy(printable, new_data.data, new_data.data_size);
  printable[new_data.data_size] = '\0';
  
  printf("The new data string for %s is:\n%s\n", evt->entity_id.id, printable);
  printf("-------------------------\n");

  return 1;
}

int
entity_int_data_change_event_handler (CManager cm, void* event, void* client_data, attr_list event_list)
{
  int i;
  pds_entity_int_data_change_ntf_ptr evt = (pds_entity_int_data_change_ntf_ptr) event;
  printf("-------------------------\n");
  printf("Entity data change handler called\n");
  
  printf("The new int data for %s is:", evt->id );
  for(i = 0; i < (evt->data_size - 1); ++i)
    printf(" %d,", evt->data[i]); 
  printf( " %d\n", evt->data[i]);

  printf("-------------------------\n");

  return 1;
}

int
entity_float_data_change_event_handler (CManager cm, void* event, void* client_data, attr_list event_list)
{
  int i;
  pds_entity_float_data_change_ntf_ptr evt = (pds_entity_float_data_change_ntf_ptr) event;
  printf("-------------------------\n");
  printf("Entity data change handler called\n");
  pds_entity_float_data_t new_data = evt->float_data;
  
  printf("The new float data for %s is:", evt->entity_id.id );
  for(i = 0; i < (new_data.data_size - 1); ++i)
    printf(" %f,", new_data.data[i]); 
  printf( " %f\n", new_data.data[i]);

  printf("-------------------------\n");

  return 1;
}

int entity_bind_unbind_event_handler (CManager cm, void * event, void * client_data, attr_list event_list)
{
  pds_entity_u_bind_change_ntf_ptr evt = (pds_entity_u_bind_change_ntf_ptr) event;
  printf("-------------------------\n");
  printf("Entity bind/unbind handler called\n");
  if(evt->type == PDS_ENTITY_CHANGE_BIND)
    printf("Entity: %s\t was bound\n", evt->entity_id.id);
  else if(evt->type == PDS_ENTITY_CHANGE_UNBIND)
    printf("Entity: %s\t was unbound\n", evt->entity_id.id);
  else
  {
    printf("Error: bound event handled but neither bound nor unbound\n");
    return 0;
  }
  printf("-------------------------\n");
  return 1;
  
}

void register_domain()
{
  pds_register_for_domain_changes(cm, new_domain_id, domain_event_handler, 0);
  printf("Registered for domain changes...\n");
}

void register_entity_channel(int which_channel, char * print_msg, EVSimpleHandlerFunc handler_func )
{
  pds_register_for_entity_changes_by_channel(cm, new_domain_id, "/newEntity", null_pds_context_id, handler_func, 0, which_channel);
  printf("Registered for: %s\n", print_msg ? print_msg : "NULL");
}


int main (int argc, char *argv[])
{
  printf("Program starting\n");
  struct gengetopt_args_info args_info;
  pds_service wps;
  pds_context_id_t cid1;
  pds_entity_id_t eid1;
  attr_list contact_attrs;
  char *pds_host;
  char *str = "First stored string before any changes are made.";
  float temp_float = 6.3;
  int temp_int[] = {59, 3, 46};
  pds_entity_char_data_t tt;
  pds_entity_int_data_t it;
  pds_entity_float_data_t ft;
  char **bindings;
  int i2;
  atom_t VAL1_ATOM, VAL2_ATOM;

  if (cmdline_parser (argc, argv, &args_info) != 0) exit(1);
  if (args_info.hostname_given) printf ("hostname is %s\n", args_info.hostname_arg);

  tt.data = (unsigned char*)str;
  tt.data_size = strlen (str);

  //it.data = temp_int;
  it.data = malloc (3*sizeof(int));
  it.data[0] = 59; it.data[1]=3; it.data[2] = 46;
  it.data_size = 3;
  
  ft.data = &temp_float;
  ft.data_size = 1;

  pds_host = getenv ("PDS_SERVER_HOST");
  if (pds_host == NULL) pds_host = getenv ("HOSTNAME");
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

  register_domain(); 

  register_entity_channel(ENTITY_BIND_UNBIND, "entity_bind_unbind", entity_bind_unbind_event_handler);
  register_entity_channel(ENTITY_DATA_CHANGE_CHAR, "entity_data_change", entity_char_data_change_event_handler);
  register_entity_channel(ENTITY_DATA_CHANGE_INT, "entity_int_data_change", entity_int_data_change_event_handler);
  register_entity_channel(ENTITY_DATA_CHANGE_FLOAT, "entity_float_data_change", entity_float_data_change_event_handler);

  eid1 = pds_create_entity_char (new_domain_id, "/newEntity", null_pds_context_id, &tt, NULL);
  printf ("[ created entity %s]", eid1.id);
  fflush (0);
  
  
  if(pds_set_entity_float_data (new_domain_id, "/newEntity", null_pds_context_id, &ft, 0) < 0)
  {
    fprintf(stderr, "Failed to set float data for /newEntity\n");
  }

  if(pds_set_entity_int_data (new_domain_id, "/newEntity", null_pds_context_id, &it, 0) < 0)
  {
    fprintf(stderr, "Failed to set int data for /newEntity\n");
  }

  printf("Finished waiting for events...\n");
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
