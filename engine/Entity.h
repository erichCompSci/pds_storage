#ifndef _ENTITY_H_
#define _ENTITY_H_

/*
 *
 *  $Id: Entity.h,v 1.3 2003-09-29 00:57:22 mansour Exp $
 *
 */
#include <sys/types.h>

extern "C"
{
#include "atl.h"
}

#include "ContextBindable.h"

class Domain;
class Context;

class Entity : public ContextBindable
{
 public:

  Entity (const char* my_name, Domain* d, Context* parent);
  Entity (const char* my_name, Domain* d, Context* parent, int what_data_holder);
  virtual ~Entity();

  virtual void make_real();

  void set_data (void *, unsigned long, attr_value_type);
  const pds_entity_data_t get_data() const;
  virtual void unbound_from (Context *);

  virtual void send_creation_event();
  virtual void send_data_event();
  virtual void send_bind_event();
  virtual void send_unbind_event();
  virtual void send_deletion_event();
  virtual void send_attr_event();

 public:
  
  size_t data_size_;
  unsigned char *data_;
  attr_value_type data_type_;

};


/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.2  2003/01/29 03:36:39  mansour
 *  A bunch of changes to make get_full_name work. Add a my_name_ at the ContextBindable level. This really helps for debugging only and can be removed. Fixed the algorithm for get_full_name.
 *
 *  Also, added calls to generate events on context_creation and deletion.
 *
 *  Revision 1.1  2001/10/09 21:37:55  pmw
 *  configuration changes
 *
 *  Revision 1.3  2001/09/23 22:55:53  pmw
 *  Contexts now support attributes of their own.  Practically, this is supported
 *  by having ContextBindable host the attribute list instead of Entity/Context.  Also
 *  added the usual functions on the client side and handlers for getting/setting
 *  this attr list for Contexts.
 *
 *  Revision 1.2  2001/07/02 04:02:03  pmw
 *  interim checkin for changes to make Contexts also support events.  compiles
 *  but tests probably do not run.
 *
 *  Revision 1.1.1.1  2000/12/13 22:14:23  pmw
 *  imported sources
 *
 *  Revision 1.4  2000/11/29 04:37:36  pmw
 *  changed channel-handling again, Entity now performs most of its own
 *  change event sends
 *
 *  Revision 1.3  2000/11/27 23:15:52  pmw
 *  Fixed problem where bindings weren't being promoted from placeholders
 *  in the Context binding_map.  Context now supports a promote_binding call,
 *  which calls make_real on whatever the binding was bound to.  For entities,
 *  this does the right thing (sets up the channel, etc)
 *
 *  Revision 1.2  2000/11/22 21:42:34  pmw
 *  proactivity based on entity events now appears to work correctly.  In particular,
 *  creation events based on the prior registration of placeholder entities and their
 *  later "creation" as real entities works.
 *
 *  Revision 1.1  2000/11/21 00:27:57  pmw
 *  New header files, should have been in last commit.
 *
 *
 */

#endif
