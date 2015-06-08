/*
 *
 *  $Id: sinktest.c,v 1.6 2008-09-24 12:39:17 eisen Exp $
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "atl.h"
#include "evpath.h"

#include "agent/pds.h"
#include "common/formats.h"

int keep_going = 1;

int
event_handler (CManager cm, void* event, void* client_data, attr_list event_list)
{
  pds_entity_change_event* evt = (pds_entity_change_event*) event;
  switch (evt->type)
    {
    case PDS_ENTITY_CHANGE_CREATION:
      fprintf (stdout, "received a creation event\n");
      break;
    case PDS_ENTITY_CHANGE_DELETION:
      fprintf (stdout, "received a deletion event\n");
      keep_going = 0;
      break;
    case PDS_ENTITY_CHANGE_DATA:
      fprintf (stdout, "received a data event\n");
      break;
    case PDS_ENTITY_CHANGE_BIND:
      break;
    case PDS_ENTITY_CHANGE_UNBIND:
      fprintf (stdout, "received an unbind event\n");
      keep_going = 0;
      break;
    case PDS_ENTITY_CHANGE_ATTRS:
      fprintf (stdout, "received an attribute change event\n");
      break;
    default:
      fprintf (stdout, "received an unknown event\n");
    }
  return 1;
}

int main (int argc, char *argv[])
{
  pds_service wps;
  pds_domain_id_t new_domain_id;
  EVaction action;
  CManager cm;
  attr_list contact_attrs;

  contact_attrs = create_attr_list();
  set_attr (contact_attrs,
            attr_atom_from_string("PDS_IP_HOST"),
            Attr_String,
            strdup (getenv("HOSTNAME")));
  set_attr (contact_attrs,
            attr_atom_from_string("PDS_IP_PORT"),
            Attr_Int4,
            (attr_value*)8848);

  cm = CManager_create();
  wps = pds_service_open (contact_attrs);

  new_domain_id = pds_open_domain (wps, 
                                  "newDomain",
                                  "newDomaintype",
                                  1,
                                  "wp-test");
  
  action = pds_register_for_domain_changes (cm, new_domain_id, event_handler, NULL); 
  action = pds_register_for_entity_changes_by_channel (cm,
					    new_domain_id,
					    "newEntity",
					    null_pds_context_id,
					    event_handler,
					    NULL,
                        ENTITY_CREATE_DESTROY);

  while (keep_going)
    {
      CMpoll_network (cm);
    }
  
  pds_cancel_changes_registration (action);
  free_attr_list (contact_attrs);

  pds_service_close (wps);
  CManager_close (cm);
  
  return 0;
}

/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.5  2007/12/04 21:06:26  pmw
 *  Merge changes from evpath branch.  Also probably includes
 *  Darwin fixes.  Compiles on isleroyale and marquesas, I think
 *  I'm misconfigured somehow for building on gondor.
 *
 *  Revision 1.4.2.1  2007/01/05 22:40:42  pmw
 *  interim commit for evpath changes, new branch
 *
 *  Revision 1.4  2003/01/28 16:31:32  mansour
 *  Update the proactive test cases
 *
 *  Revision 1.3  2002/08/07 18:51:48  eisen
 *  Widespread changes to modify how attribute handling works.  Stop using
 *  attr_atom_from_string() to get atom values.  Instead, those are set by the
 *  application.  Stop using get_attr() (api has changes).  Stop using
 *  xmit_object and start using the ATL-supplied encode/decode routines.
 *
 *  Revision 1.2  2001/10/09 21:37:57  pmw
 *  configuration changes
 *
 *  Revision 1.1.1.1  2000/12/13 22:14:25  pmw
 *  imported sources
 *
 *  Revision 1.5  2000/11/29 04:37:38  pmw
 *  changed channel-handling again, Entity now performs most of its own
 *  change event sends
 *
 *  Revision 1.4  2000/11/26 07:44:23  pmw
 *  changed <strings.h> to <string.h>, to eliminate evidence of me being a
 *  dumbass.
 *
 *  Revision 1.3  2000/11/22 21:42:50  pmw
 *  proactivity based on entity events now appears to work correctly.  In particular,
 *  creation events based on the prior registration of placeholder entities and their
 *  later "creation" as real entities works.
 *
 *  Revision 1.2  2000/11/21 21:35:47  pmw
 *  Domain change events now work correctly.  moved event transmission
 *  responsibilities to Proactive class.
 *
 *  Revision 1.1  2000/11/21 00:24:46  pmw
 *  Added sinktest program, doesn't work yet.  removed unused variable warnings
 *  from bulkload.
 *
 *
 */
