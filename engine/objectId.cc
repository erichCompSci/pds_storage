/*
 *
 *  $Id: objectId.cc,v 1.17 2008-09-23 23:57:24 eisen Exp $
 *
 */
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <netdb.h>
#include <arpa/inet.h>
extern "C" {
#include <assert.h>
#include <stdio.h>
}

#include "utils.h"
#include "objectId.h"

#include "agent/pds.h"
#include "Domain.h"

in_addr_t objectId::global_server_address;
in_port_t objectId::global_server_port;

// starts at 1 because 0 means null IDs
unsigned long objectId::counter = 1;

hash_map<unsigned long, objectId::stamp_pair> objectId::global_object_map;

objectId::objectId() :
  obj_id (objectId::counter++)
{}  

objectId::~objectId()
{
  objectId::global_object_map.erase (obj_id);
}

void
objectId::set_server_addr (attr_list contact_list)
{
  in_port_t port = 0;
  in_addr_t addr = 0;
  unsigned long p = 0;
  static atom_t CM_IP_HOSTNAME = -1;
  static atom_t CM_IP_ADDR = -1;
  static atom_t CM_IP_PORT = -1;
  static atom_t PDS_IP_PORT = -1;
  static atom_t PDS_IP_ADDR = -1;

  if (CM_IP_HOSTNAME == -1) {
    CM_IP_HOSTNAME = attr_atom_from_string("IP_HOST");
    CM_IP_PORT = attr_atom_from_string("IP_PORT");
    CM_IP_ADDR = attr_atom_from_string("IP_ADDR");
    PDS_IP_PORT = attr_atom_from_string("PDS_IP_PORT");
    PDS_IP_ADDR = attr_atom_from_string("PDS_IP_ADDR");
  }

  query_attr (contact_list, PDS_IP_ADDR, NULL, reinterpret_cast<attr_value*> (&addr));
  if (addr == 0) {
    query_attr ( contact_list, CM_IP_ADDR, NULL, reinterpret_cast<attr_value*> (&addr) );
  }
  if (addr == 0) {
      char *hostname = NULL;
      struct hostent *hostptr;
      struct in_addr *p;

      query_attr (contact_list, CM_IP_HOSTNAME, NULL, reinterpret_cast<attr_value*> (&hostname));
      if (hostname == NULL) {
	hostptr = gethostbyname (getenv ("HOSTNAME"));
      } else {
	hostptr = gethostbyname (getenv (hostname));
      }
      p = (struct in_addr*) hostptr->h_addr_list[0];
      addr = p->s_addr;
  }

  objectId::global_server_address = addr;

  query_attr (contact_list, PDS_IP_PORT, NULL, reinterpret_cast<attr_value*> (&p));
  if (p == 0) {
    query_attr (contact_list, CM_IP_PORT, NULL, reinterpret_cast<attr_value*> (&p));
    if (p == 0) p = PDS_SERVER_PORT;
  }
  port = p;
  objectId::global_server_port = htons (port);

}


pds_context_id_t 
objectId::make_pds_context_id (const Domain* d, const Context* c)
{
  pds_context_id_t newguy;
  make_context_id_null (&newguy);
  unsigned char* p = newguy.id;
  const objectId* od = d;
  const objectId* oc = c;

  memcpy (p, &objectId::global_server_address, sizeof (in_addr_t));
  p += sizeof (in_addr_t);
  memcpy (p, &objectId::global_server_port, sizeof (in_port_t));
  p += sizeof (in_port_t);
  memcpy (p, &od->obj_id, sizeof (unsigned long));
  p += sizeof (unsigned long);
  memcpy (p, &oc->obj_id, sizeof (unsigned long));

  return newguy;
}

pds_domain_id_t 
objectId::make_pds_domain_id (const Domain* d)
{
  pds_domain_id_t newguy;
  const objectId* od = d;
  
  union {
    unsigned char t1[PDS_ID_SIZE];
    struct {
      in_addr_t a;
      in_port_t p;
      unsigned long o;
    } s;
  } mytype;

  assert(sizeof(mytype.s) < PDS_ID_SIZE);
  make_domain_id_null (&newguy);
  unsigned char* p = newguy.id;

  mytype.s.a = objectId::global_server_address;
  mytype.s.p = objectId::global_server_port;
  mytype.s.o = od->obj_id;

  memcpy (&p[0], &objectId::global_server_address, sizeof (in_addr_t));
  //  p += sizeof (unsigned long);
  memcpy (&p[sizeof(in_addr_t)], &objectId::global_server_port, sizeof (in_port_t));
  //  p += sizeof (unsigned short);
  memcpy (&p[sizeof(in_addr_t) + sizeof(in_port_t)], &od->obj_id, sizeof (unsigned long));

  return newguy;
}

pds_entity_id_t
objectId::make_pds_entity_id (const Domain* d, const Entity* e)
{
  pds_entity_id_t newguy;
  make_entity_id_null (&newguy);
  unsigned char* p = newguy.id;
  const objectId* od = d;
  const objectId* oe = e;

  memcpy (p, &objectId::global_server_address, sizeof (in_addr_t));
  p += sizeof (in_addr_t);
  memcpy (p, &objectId::global_server_port, sizeof (in_port_t));
  p += sizeof (in_port_t);
  memcpy (p, &od->obj_id, sizeof (unsigned long));
  p += sizeof (unsigned long);
  memcpy (p, &oe->obj_id, sizeof (unsigned long));
  return newguy;
}

Entity*
objectId::get_entity_ptr_from_id (pds_entity_id_t e_id)
{
  unsigned long obj_id;
  stamp_pair sp;
  Entity* ret;

  obj_id = get_ref_from_id (e_id.id, ENTITY_REF);

  if (obj_id == 0)
    {
      // this means the ID is null - no valid object should ever have
      // an id of 0
      ret = NULL;
    }
  else
    {
      sp = global_object_map[obj_id];
      ret = static_cast<Entity*> (sp.second);
    }
  return ret;
}

Domain*
objectId::get_domain_ptr_from_id (pds_domain_id_t d_id)
{
  unsigned long obj_id;
  stamp_pair sp;
  Domain* ret;

  obj_id = get_ref_from_id (d_id.id, DOMAIN_REF);

  if (obj_id == 0)
    {
      ret = NULL;
    }
  else
    {
      sp = global_object_map[obj_id];
      ret = static_cast<Domain*> (sp.second);
    }
  return ret;
}


Context*
objectId::get_context_ptr_from_id (pds_context_id_t c_id)
{
  unsigned long obj_id;
  stamp_pair sp;
  Context* ret;

  obj_id = get_ref_from_id (c_id.id, CONTEXT_REF);

  if (obj_id == 0)
    {
      ret = NULL;
    }
  else
    {
      sp = global_object_map[obj_id];
      ret = static_cast<Context*> (sp.second);
    }
  return ret;
}

Domain*
objectId::get_context_domain_ptr_from_id (pds_context_id_t c_id)
{
  unsigned long obj_id;
  stamp_pair sp;
  Domain* ret;

  obj_id = get_ref_from_id (c_id.id, DOMAIN_REF);

  if (obj_id == 0)
    {
      ret = NULL;
    }
  else
    {
      sp = global_object_map[obj_id];
      ret = static_cast<Domain*> (sp.second);
    }
  return ret;
}


Domain*
objectId::get_entity_domain_ptr_from_id (pds_entity_id_t e_id)
{
  unsigned long obj_id;
  stamp_pair sp;
  Domain* ret;

  obj_id = get_ref_from_id (e_id.id, DOMAIN_REF);

  if (obj_id == 0)
    {
      ret = NULL;
    }
  else
    {
      sp = global_object_map[obj_id];
      ret = static_cast<Domain*> (sp.second);
    }
  return ret;
}



/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.16  2007/12/04 21:06:20  pmw
 *  Merge changes from evpath branch.  Also probably includes
 *  Darwin fixes.  Compiles on isleroyale and marquesas, I think
 *  I'm misconfigured somehow for building on gondor.
 *
 *  Revision 1.15.2.1  2007/01/05 22:40:35  pmw
 *  interim commit for evpath changes, new branch
 *
 *  Revision 1.15  2005/12/08 23:37:05  eisen
 *  Fix includes after removing #include of cm.h from echo.h.
 *
 *  Revision 1.14  2005/08/24 13:13:45  eisen
 *  Include assert.h so we can use assert()
 *
 *  Revision 1.13  2005/08/23 19:52:10  eisen
 *  Try to fix some 64/32-bit issues.  Try to use in_addr_t where we're
 *  manipulating and representing IP addresses.  (Expect to fix this again for
 *  IPv6, or when hell freezes over, whichever comes first.)
 *
 *  Revision 1.12  2005/02/22 07:18:20  pmw
 *  If there's any justice, this has knocked the cross-platform
 *  connection bug.  The open_service exchange actually sends
 *  the address and port with PBIO integer encoding, so in some
 *  cases there's an extra byte-swap happening.  Fixed by performing
 *  byte-swapping manually for just that case BEFORE the addr/port
 *  is stored in pdsagent's list of services.
 *
 *  Revision 1.11  2004/10/14 21:17:31  pmw
 *  Changes to support the CMrpc renaming.
 *
 *  Revision 1.10  2003/11/09 03:57:06  mansour
 *  Add wrapper API for building XPath queries
 *
 *  Revision 1.9  2003/10/31 05:39:36  mansour
 *  Added support for xpath queries
 *
 *  Revision 1.8  2003/10/05 12:10:26  pmw
 *  Changes to make utils a C module instead of C++, allowing people to
 *  link against its functions more easily
 *
 *  Revision 1.7  2003/10/02 16:29:00  mansour
 *  remove DOS EOL chars
 *
 *  Revision 1.6  2003/09/29 00:57:25  mansour
 *  Added a checkpointing thread.
 *  Fix the code for XML dump and load
 *  Fix the code for get_full_name
 *
 *  Revision 1.5  2002/08/07 18:51:45  eisen
 *  Widespread changes to modify how attribute handling works.  Stop using
 *  attr_atom_from_string() to get atom values.  Instead, those are set by the
 *  application.  Stop using get_attr() (api has changes).  Stop using
 *  xmit_object and start using the ATL-supplied encode/decode routines.
 *
 *  Revision 1.4  2002/01/15 05:18:23  pmw
 *  I think I finally i∆killed the hostname/IP_ADDR problems.  Now I just always
 *  try to do htonl on IP_ADDR values before using them.  Should never hurt
 *  because the macro is null where host and network formats are the same.
 *
 *  Revision 1.3  2002/01/14 00:45:57  pmw
 *  CM IP_* attributes are all apparently stored in host byte-order now.  Modified
 *  set_server_address to always byte-swap these values to network order at
 *  startup for use in object IDs.  Also, set_server_address now pays attention to
 *  the IP_PORT attribute in the contact list instead of using PDS_SERVER_PORT
 *  directly (the pdsd startup sets this attribute in the contact list using PDS_SERVER_PORT
 *  or something else specified by the user).
 *
 *  Revision 1.2  2001/10/12 00:15:54  pmw
 *  fixed resolution of server IP address - uses CM's contact list value now
 *
 *  Revision 1.1  2001/10/09 21:37:55  pmw
 *  configuration changes
 *
 *  Revision 1.1.1.1  2000/12/13 22:14:23  pmw
 *  imported sources
 *
 *  Revision 1.2  2000/11/28 22:49:29  pmw
 *  1) moved CManager stuff to Proactive class, since other classes
 *  don't need to deal with it any longer
 *  2) moved object ID management out of Domain and into objectId.
 *
 *  Revision 1.1  2000/11/21 00:23:53  pmw
 *  Since I didn't have enough to do, a little code reorganization to keep
 *  things from getting dramatically out of hand.  Fixed a couple of
 *  bugs-in-waiting.
 *
 *
 */
