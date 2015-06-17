/*
 *
 *  $Id: Entity.cc,v 1.13 2008-01-01 00:38:18 pmw Exp $
 *
 */

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
  //#include <assert.h>
#include <time.h>
#include <sys/types.h>

#include "atl.h"
#include "evpath.h"

#include "common/formats.h"
};

#include "common/formats.h"
#include "utils.h"
#include "EventWrapper.h"
#include "Entity.h"
#include "Domain.h"
#include "Context.h"


Entity::Entity (const char* my_name, Domain* d, Context* parent) :
  ContextBindable (my_name, parent, d, entity_DescRecs ),
  data_size_ (0),
  data_ (NULL),
  data_type_ (Attr_Undefined)
{
  domain_->add_entity_to_list(this);
}

Entity::Entity (const char * my_name, Domain * d, Context * parent, int what_data_holder) :
  ContextBindable (my_name, parent, d),
  data_size_ (0),
  data_ (NULL),
  data_type_ (Attr_Undefined)
{
  domain_->add_entity_to_list(this);
  if
}


Entity::~Entity()
{
  /*
   *  must also deal with releasing all bindings
   */
  pdsTrace_out (pdsengineVerbose, "<Entity::~Entity()>");

  /*
    PMW - removed this because it trashes the list iterator in the domain destructor.
    don't know if this is necessary, but if so it needs to be rethought.

    pdsTrace_out (pdsengineVerbose, "removing myself %p from domain %p", this, domain_);
    domain_->remove_entity_from_list(this);
  */

  pdsTrace_out (pdsengineVerbose, "</Entity::~Entity()>");
}



void
Entity::make_real()
{
  ContextBindable::make_real();
}


const pds_entity_data_t
Entity::get_data() const
{
  pds_entity_data_t rval;
  rval.data = data_;
  rval.data_size = data_size_;
  rval.data_type = data_type_;
  return rval;
}

void
Entity::set_data (void *new_data, unsigned long len, attr_value_type type)
{  
  free (data_);
  data_ = (unsigned char*) malloc (len + 1);
  memcpy (data_, new_data, len);
  data_[len] = 0;

  data_size_ = len;
  data_type_ = type;
}

void
Entity::unbound_from (Context *c)
{
  ContextBindable::unbound_from (c);
}


void 
Entity::send_creation_event()
{
  pds_entity_exist_change_ntf evt;
  evt.type = PDS_ENTITY_CHANGE_CREATION;
  evt.entity_id = objectId::make_pds_entity_id (domain_, this);
  evt.entity_data = get_data();
  EntityEventWrapper wrap (evt);
  send_event (&wrap, ENTITY_CREATE_DESTROY );
}

void 
Entity::send_data_event()
{
  pds_entity_data_change_ntf evt;
  evt.entity_id = objectId::make_pds_entity_id (domain_, this);
  evt.entity_data = get_data();
  EntityEventWrapper wrap (evt);
  send_event (&wrap, ENTITY_DATA_CHANGE);
}

void 
Entity::send_bind_event()
{
  pds_entity_u_bind_change_ntf evt;
  evt.type = PDS_ENTITY_CHANGE_BIND;
  evt.entity_id = objectId::make_pds_entity_id (domain_, this);
  EntityEventWrapper wrap (evt);
  send_event (&wrap, ENTITY_BIND_UNBIND);
}

void 
Entity::send_unbind_event()
{
  pds_entity_u_bind_change_ntf evt;
  evt.type = PDS_ENTITY_CHANGE_UNBIND;
  evt.entity_id = objectId::make_pds_entity_id (domain_, this);
  EntityEventWrapper wrap (evt);
  send_event (&wrap, ENTITY_BIND_UNBIND);
}

void
Entity::send_deletion_event()
{
  pds_entity_exist_change_ntf evt;
  evt.type = PDS_ENTITY_CHANGE_DELETION;
  evt.entity_id = objectId::make_pds_entity_id (domain_, this);
  make_entity_data_null (&(evt.entity_data));
  EntityEventWrapper wrap (evt);
  send_event (&wrap, ENTITY_CREATE_DESTROY);
}

void
Entity::send_attr_event()
{

}


/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.11  2007/12/04 21:06:19  pmw
 *  Merge changes from evpath branch.  Also probably includes
 *  Darwin fixes.  Compiles on isleroyale and marquesas, I think
 *  I'm misconfigured somehow for building on gondor.
 *
 *  Revision 1.10.2.1  2007/01/05 22:40:34  pmw
 *  interim commit for evpath changes, new branch
 *
 *  Revision 1.10  2005/02/22 19:25:32  pmw
 *  modifications to keep Entity's destructor from hamstringing Domain's
 *  destructor.  the entity list iterator in ~Domain() was being whacked
 *  by ~Entity().  If this functionality is needed, it needs to be
 *  rethought.
 *
 *  Revision 1.9  2003/10/05 12:10:25  pmw
 *  Changes to make utils a C module instead of C++, allowing people to
 *  link against its functions more easily
 *
 *  Revision 1.8  2003/09/29 00:57:24  mansour
 *  Added a checkpointing thread.
 *  Fix the code for XML dump and load
 *  Fix the code for get_full_name
 *
 *  Revision 1.7  2003/07/27 04:49:46  mansour
 *  initialize the event full name and description
 *
 *  Revision 1.6  2003/02/04 17:27:07  mansour
 *  Initialize the desc field for create events
 *
 *  Revision 1.5  2003/01/30 19:16:26  mansour
 *  Add support for domain events
 *
 *  Revision 1.4  2003/01/29 03:36:39  mansour
 *  A bunch of changes to make get_full_name work. Add a my_name_ at the ContextBindable level. This really helps for debugging only and can be removed. Fixed the algorithm for get_full_name.
 *
 *  Also, added calls to generate events on context_creation and deletion.
 *
 *  Revision 1.3  2002/11/16 03:06:34  pmw
 *  fixed bug with data length in setting new entity data buffer
 *
 *  Revision 1.2  2002/08/05 22:49:25  pmw
 *  Changes to support gcc 3.0 and xerces-c 2.0
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
 *  Revision 1.1.1.1  2000/12/13 22:14:22  pmw
 *  imported sources
 *
 *  Revision 1.8  2000/12/06 01:57:00  pmw
 *  Fixed problems with memory leakage and poor server shutdown mechanics
 *
 *  Revision 1.7  2000/11/29 04:37:36  pmw
 *  changed channel-handling again, Entity now performs most of its own
 *  change event sends
 *
 *  Revision 1.6  2000/11/27 23:15:51  pmw
 *  Fixed problem where bindings weren't being promoted from placeholders
 *  in the Context binding_map.  Context now supports a promote_binding call,
 *  which calls make_real on whatever the binding was bound to.  For entities,
 *  this does the right thing (sets up the channel, etc)
 *
 *  Revision 1.5  2000/11/25 23:20:53  pmw
 *  ltmain.sh
 *
 *  Revision 1.4  2000/11/22 21:42:34  pmw
 *  proactivity based on entity events now appears to work correctly.  In particular,
 *  creation events based on the prior registration of placeholder entities and their
 *  later "creation" as real entities works.
 *
 *  Revision 1.3  2000/11/21 21:35:24  pmw
 *  Domain change events now work correctly.  moved event transmission
 *  responsibilities to Proactive class.
 *
 *  Revision 1.2  2000/11/21 00:23:45  pmw
 *  Since I didn't have enough to do, a little code reorganization to keep
 *  things from getting dramatically out of hand.  Fixed a couple of
 *  bugs-in-waiting.
 *
 *  Revision 1.1  2000/10/25 21:44:43  pmw
 *  Compiles after major revisions to support multiple WP services on
 *  client-side.  This should eventually allow a client to use services by
 *  linking in the library directly or by communicating using CMRPC.
 *
 *
 */
