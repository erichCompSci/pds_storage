/*
 *
 *  $Id: pdslookup.c,v 1.7 2008-09-24 12:39:19 eisen Exp $
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "evpath.h"
#include "agent/pds.h"


int main (int argc, char *argv[])
{
  pds_service wps;
  attr_list contact_attrs;
  pds_domain_id_t test_domain;
  char* keyval = NULL;

  if (argc < 3)
    {
      fprintf (stderr, "usage: wplookup <wp server host> <keyname>\n");
      exit (1);
    }

  contact_attrs = create_attr_list();
  set_attr (contact_attrs,
            attr_atom_from_string("PDS_IP_HOST"),
            Attr_String,
            strdup (argv[1]));
  set_attr (contact_attrs,
            attr_atom_from_string("PDS_IP_PORT"),
            Attr_Int4,
            (attr_value*)8848);


  fprintf (stderr, "opening wp service...\n");
  if ((wps = pds_service_open (contact_attrs)) == NULL)
    {
      fprintf (stderr, "Couldn't init wp client-side (is WP server running?)\n");
      exit (1);
    }

  fprintf (stderr, "retrieving domain...\n");
  
  test_domain = pds_open_domain (wps, "Test Server", "Testing", 1, "");

  fprintf (stderr, "looking for string value at %s\n", argv[2]);
  
  if (pds_lookup_string (test_domain, argv[2], &keyval) == -1)
    {
      fprintf (stdout, "no entity found at %s\n", argv[2]);
    }
  else
    {
      fprintf (stdout, "value at %s is [%s]\n", argv[2], keyval);
    }

  fprintf (stderr, "done with lookup...\n");
  free_attr_list (contact_attrs);
  free (keyval);

  fprintf (stderr, "closing wp service...\n");
  pds_service_close (wps);
  
  return 0;
}


/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.6  2007/12/04 21:06:27  pmw
 *  Merge changes from evpath branch.  Also probably includes
 *  Darwin fixes.  Compiles on isleroyale and marquesas, I think
 *  I'm misconfigured somehow for building on gondor.
 *
 *  Revision 1.5.2.1  2007/01/05 22:40:44  pmw
 *  interim commit for evpath changes, new branch
 *
 *  Revision 1.5  2005/12/09 01:00:00  eisen
 *  Fix includes after removing #include of cm.h from echo.h.
 *
 *  Revision 1.4  2002/08/07 18:51:49  eisen
 *  Widespread changes to modify how attribute handling works.  Stop using
 *  attr_atom_from_string() to get atom values.  Instead, those are set by the
 *  application.  Stop using get_attr() (api has changes).  Stop using
 *  xmit_object and start using the ATL-supplied encode/decode routines.
 *
 *  Revision 1.3  2001/10/09 21:37:57  pmw
 *  configuration changes
 *
 *  Revision 1.2  2001/06/12 03:14:33  pmw
 *  minor fixes
 *
 *  Revision 1.1.1.1  2000/12/13 22:14:25  pmw
 *  imported sources
 *
 *  Revision 1.4  2000/12/06 01:57:10  pmw
 *  Fixed problems with memory leakage and poor server shutdown mechanics
 *
 *  Revision 1.3  2000/11/30 19:36:58  pmw
 *  added hostname parameter to do remote lookups
 *
 *  Revision 1.2  2000/11/29 04:37:39  pmw
 *  changed channel-handling again, Entity now performs most of its own
 *  change event sends
 *
 *  Revision 1.1  2000/11/28 03:24:47  pmw
 *  reorganizations....
 *
 *  Revision 1.5  2000/11/27 23:14:06  pmw
 *  initialize keyval, compiler doesn't do it on Linux and it segfaults in
 *  free() if no value is found
 *
 *  Revision 1.4  2000/11/26 07:44:23  pmw
 *  changed <strings.h> to <string.h>, to eliminate evidence of me being a
 *  dumbass.
 *
 *  Revision 1.3  2000/11/21 21:35:48  pmw
 *  Domain change events now work correctly.  moved event transmission
 *  responsibilities to Proactive class.
 *
 *  Revision 1.2  2000/11/20 05:09:16  pmw
 *  now uses wp_open_domain() call
 *
 *  Revision 1.1  2000/11/19 23:30:04  pmw
 *  Added new test for bulk loading (bulkload.c) and new utility to query directory
 *  content (wplookup)
 *
 *
 */
