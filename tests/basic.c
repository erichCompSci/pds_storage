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


#include "agent/pds.h"
#include "cmdline.h"

#define PDS_CONNECT_FILE "/net/hu21/elohrman/pds_connect"

int main (int argc, char *argv[])
{
  struct gengetopt_args_info args_info;
  pds_service wps;
  pds_domain_id_t new_domain_id;
  pds_context_id_t cid1, cid2, cid3;
  pds_entity_id_t eid1;
  attr_list attrs, attrs2, contact_attrs;
  char *pds_host;
  int val1 = 99, val2 = 98;
  char *str = "This is the first test string I will store.";
  pds_entity_char_data_t tt;
  char **bindings;
  int i2;
  atom_t VAL1_ATOM, VAL2_ATOM;

  if (cmdline_parser (argc, argv, &args_info) != 0) exit(1);
  if (args_info.hostname_given) printf ("hostname is %s\n", args_info.hostname_arg);

  tt.data = (unsigned char*)str;
  tt.data_size = strlen (str);

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

  if ((wps = pds_service_open (contact_attrs)) == NULL)
    {
      fprintf (stderr, "Couldn't init PDS client-side (is pdsd running?)\n");
      exit (1);
    }

  new_domain_id = pds_open_domain (wps, 
                                  "newDomain",
                                  "newDomaintype",
                                  1,
                                  "wp-test");


  cid1 = pds_get_root_context (new_domain_id);
  printf ("Got root context...\n");
  
  attrs = create_attr_list();
  printf ("created attr list...\n");

  VAL1_ATOM = attr_atom_from_string("PDS_TEST_VAL1");
  VAL2_ATOM = attr_atom_from_string("PDS_TEST_VAL2");
  add_int_attr (attrs, VAL1_ATOM, val1);
  add_int_attr (attrs, VAL2_ATOM, val2);
  printf ("[ added attributes ]");
  fflush (0);

  eid1 = pds_create_entity_char (new_domain_id, "newEntity", null_pds_context_id, &tt, attrs);
  printf ("[ created entity %s]", eid1.id);
  fflush (0);

  attrs2 = create_attr_list();
  pds_get_entity_attributes_by_id (new_domain_id, eid1, attrs2);
  printf ("[ retrieved entity attributes ]"); fflush (0);

  dump_attr_list (attrs2);

  bindings = pds_get_binding_list_id (new_domain_id, cid1, OnlyEntityBindings);
  i2 = 0;
  for (i2 = 0; bindings[i2] != NULL; i2++)
    {
      printf ("got binding of root context [%s]\n", bindings[i2]);
      
      free(bindings[i2]);
    }
  free(bindings);

  pds_remove_entity (new_domain_id, "newEntity", null_pds_context_id);
  printf ("[ removed entity ]"); fflush (0);

  cid2 = pds_create_context (new_domain_id, "newContext", cid1);

  cid2 = pds_create_context (new_domain_id, "new3Context/child", cid1);
  cid3 = pds_create_context (new_domain_id, "gatech/coc", cid1);
  cid2 = pds_create_context (new_domain_id, "new2Context", cid3);
  printf ("created new context... \n");

  bindings = pds_get_binding_list_id (new_domain_id, cid1, OnlyContextBindings);
  i2 = 0;
  for (i2 = 0; bindings[i2] != NULL; i2++)
    {
      printf ("got binding of root context [%s]\n", bindings[i2]);
      free(bindings[i2]);
    }
  free(bindings);

  bindings = pds_get_binding_list_id (new_domain_id, cid3, OnlyContextBindings);
  for (i2 = 0; bindings[i2] != NULL; ++i2)
  {
      printf("got binding of root context [%s] from two levels down\n", bindings[i2]);
      free(bindings[i2]);
  }
  free(bindings);

  printf( "Checking for bindings in created context... " );
  bindings = pds_get_binding_list_id( new_domain_id, cid2, OnlyEntityBindings );
  if(bindings[0] == NULL)
    printf("There is no entity bound, nor should there be...\n");
  else
    printf("There is a bound entity, but there shouldn't be...\n");
  printf( "done\n" );
  
  printf ("removing context... "); fflush(0);
  pds_remove_context_by_id (new_domain_id, cid2);
  printf( "done\n" );
  
  if (pds_remove_domain (new_domain_id))
    {
      printf ("domain removal success\n");
    }
  else
    {
      printf ("domain removal failure\n");
    }

  free_attr_list (attrs);
  free_attr_list (attrs2);
  free_attr_list (contact_attrs);

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
