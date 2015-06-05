/*
 *
 *  $Id: utils.c,v 1.2 2005-08-23 19:52:10 eisen Exp $
 *
 */
#include <sys/time.h>

#include "utils.h"
#include "string.h"
#include <netinet/in.h> /* for struct in_addr */


/*
 *
 *  operations on service object ids
 *
 */

pds_entity_id_t null_pds_entity_id = { {'\0'} };
pds_context_id_t null_pds_context_id = { {'\0'} };
pds_domain_id_t null_pds_domain_id = { {'\0'} };

void
make_domain_id_null (pds_domain_id_t* id)
{
  memset (id->id, '\0', PDS_ID_SIZE);
}

void
make_entity_data_null (pds_entity_data_t* d)
{
  d->data = (unsigned char*)NULL;
  d->data_size = 0;
  d->data_type = Attr_Undefined;
}

void
make_entity_id_null (pds_entity_id_t* id)
{
  memset (id->id, '\0', PDS_ID_SIZE);
}


void
make_context_id_null (pds_context_id_t* id)
{
  memset (id->id, '\0', PDS_ID_SIZE);
}

unsigned long
get_ref_from_id (unsigned char id[PDS_ID_SIZE], int which)
{
  unsigned long rv;
  size_t offset;
  offset = sizeof (in_addr_t) + sizeof (unsigned short)
    + (which * sizeof (unsigned long));
  memcpy (&rv, id + offset, sizeof (unsigned long));
  return rv;
}

int
pds_is_context_id_null (pds_context_id_t* c_id)
{
  return (get_ref_from_id (c_id->id, CONTEXT_REF) == 0 
          && get_ref_from_id (c_id->id, DOMAIN_REF) == 0);
}

int
pds_is_domain_id_null (pds_domain_id_t* d_id)
{
  return (get_ref_from_id (d_id->id, DOMAIN_REF) == 0);
}

int
pds_is_entity_id_null (pds_entity_id_t* e_id)
{
  return (get_ref_from_id (e_id->id, ENTITY_REF) == 0
          && get_ref_from_id (e_id->id, DOMAIN_REF) == 0);
}

unsigned long
get_timestamp()
{
  struct timeval tp;
  gettimeofday (&tp, NULL);
  return tp.tv_sec;
}
  


/*
 * 
 *  $Log: not supported by cvs2svn $
 *  Revision 1.1  2003/10/05 12:10:27  pmw
 *  Changes to make utils a C module instead of C++, allowing people to
 *  link against its functions more easily
 *
 *  Revision 1.1  2001/10/09 21:37:55  pmw
 *  configuration changes
 *
 *  Revision 1.2  2001/09/10 03:55:43  pmw
 *  client lib is now libpdsagent, and is compiled entirely with gcc.  libpdsutil no
 *  longer exists.
 *
 *  Revision 1.1.1.1  2000/12/13 22:14:23  pmw
 *  imported sources
 *
 *  Revision 1.5  2000/11/21 00:23:55  pmw
 *  Since I didn't have enough to do, a little code reorganization to keep
 *  things from getting dramatically out of hand.  Fixed a couple of
 *  bugs-in-waiting.
 *
 *  Revision 1.4  2000/10/25 21:44:03  pmw
 *  Compiles after major revisions to support multiple WP services on
 *  client-side.  This should eventually allow a client to use services by
 *  linking in the library directly or by communicating using CMRPC.
 *
 *  Revision 1.3  2000/01/06 04:59:06  pmw
 *  changed id scheme so java side is simpler
 *
 *  Revision 1.2  1999/12/21 19:16:55  pmw
 *  successful build after changes
 *
 *  Revision 1.1  1999/12/21 18:47:55  pmw
 *  Too many changes - really shouldn't have done this many
 *
 *
 */


