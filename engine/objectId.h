#ifndef _OBJECTID_H_
#define _OBJECTID_H_

/*
 *
 *  $Id: objectId.h,v 1.5 2002-08-13 20:25:51 pmw Exp $
 *
 */
#include <netinet/in.h>

#include "get_hash_map.h"

extern "C"
{
#include "atl.h"
}

#include "config.h"

#include "agent/pdsdefs.h"

class Domain;
class Context;
class Entity;

using namespace std;

class objectId
{
 public:

  typedef pair<unsigned long, void*> stamp_pair;
  typedef pair<unsigned long, stamp_pair> object_map_pair;

  objectId();
  virtual ~objectId();

  static void set_server_addr (attr_list contact_list);

  static Domain* get_domain_ptr_from_id (pds_domain_id_t);
  static Context* get_context_ptr_from_id (pds_context_id_t);
  static Domain* get_context_domain_ptr_from_id (pds_context_id_t);
  static Entity* get_entity_ptr_from_id (pds_entity_id_t);
  static Domain* get_entity_domain_ptr_from_id (pds_entity_id_t);

  static pds_entity_id_t make_pds_entity_id (const Domain *, const Entity *);
  static pds_context_id_t make_pds_context_id (const Domain *, const Context *);
  static pds_domain_id_t make_pds_domain_id (const Domain *);


 public:
  
  unsigned long obj_id;
  
  static hash_map <unsigned long, stamp_pair> global_object_map;
  static in_addr_t global_server_address;
  static in_port_t global_server_port;

 private:

  static unsigned long counter;

};


/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.4  2002/08/05 22:49:25  pmw
 *  Changes to support gcc 3.0 and xerces-c 2.0
 *
 *  Revision 1.3  2002/03/28 04:46:07  pmw
 *  changes for getting names of referrers.
 *
 *  Revision 1.2  2001/10/12 00:15:54  pmw
 *  fixed resolution of server IP address - uses CM's contact list value now
 *
 *  Revision 1.1  2001/10/09 21:37:55  pmw
 *  configuration changes
 *
 *  Revision 1.1.1.1  2000/12/13 22:14:22  pmw
 *  imported sources
 *
 *  Revision 1.2  2000/11/28 22:49:30  pmw
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
#endif
