/*
 *
 *  $Id: bulkload.c,v 1.7 2008-09-24 12:39:17 eisen Exp $
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "agent/pds.h"


int main (int argc, char *argv[])
{
  pds_service wps;
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

  if (argc < 2) 
    {
      fprintf (stderr, "usage: bulkload <URL>");
      exit(1);
    }
  
  fprintf (stderr, "opening wp service...\n");
  if ((wps = pds_service_open (contact_attrs)) == NULL)
    {
      fprintf (stderr, "Couldn't init wp client-side (is WP server running?)\n");
      exit (1);
    }

  fprintf (stderr, "beginning bulk load...\n");
  pds_bulk_load_from_URL (wps, argv[1]);
  
  fprintf (stderr, "done with bulk load...\n");
  free_attr_list (contact_attrs);

  fprintf (stderr, "closing wp service...\n");
  pds_service_close (wps);
  
  return 0;
}


/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.6  2007/12/04 21:06:26  pmw
 *  Merge changes from evpath branch.  Also probably includes
 *  Darwin fixes.  Compiles on isleroyale and marquesas, I think
 *  I'm misconfigured somehow for building on gondor.
 *
 *  Revision 1.5.2.1  2007/01/05 22:40:42  pmw
 *  interim commit for evpath changes, new branch
 *
 *  Revision 1.5  2005/12/08 23:37:06  eisen
 *  Fix includes after removing #include of cm.h from echo.h.
 *
 *  Revision 1.4  2002/08/07 18:51:48  eisen
 *  Widespread changes to modify how attribute handling works.  Stop using
 *  attr_atom_from_string() to get atom values.  Instead, those are set by the
 *  application.  Stop using get_attr() (api has changes).  Stop using
 *  xmit_object and start using the ATL-supplied encode/decode routines.
 *
 *  Revision 1.3  2001/10/09 21:37:57  pmw
 *  configuration changes
 *
 *  Revision 1.2  2001/06/12 03:14:32  pmw
 *  minor fixes
 *
 *  Revision 1.1.1.1  2000/12/13 22:14:26  pmw
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
 *  Revision 1.3  2000/11/21 21:35:47  pmw
 *  Domain change events now work correctly.  moved event transmission
 *  responsibilities to Proactive class.
 *
 *  Revision 1.2  2000/11/21 00:24:46  pmw
 *  Added sinktest program, doesn't work yet.  removed unused variable warnings
 *  from bulkload.
 *
 *  Revision 1.1  2000/11/19 23:30:03  pmw
 *  Added new test for bulk loading (bulkload.c) and new utility to query directory
 *  content (wplookup)
 *
 *
 */
