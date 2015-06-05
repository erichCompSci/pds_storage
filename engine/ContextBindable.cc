/*
 *
 *  $Id: ContextBindable.cc,v 1.10 2007-12-24 14:40:26 eisen Exp $
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
#include <unistd.h>

#include "atl.h"
#include "evpath.h"


};

#include <vector>
#include <string>

#include "common/formats.h"
#include "agent/pdsdefs.h"

#include "utils.h"
#include "EventWrapper.h"
#include "ContextBindable.h"
#include "Domain.h"
#include "Context.h"

void
ContextBindable::unbound_from (Context *c)
{
  remove_referrer (c);
}

unsigned int
ContextBindable::referrer_count() const
{
  return referrers_.size();
}

void
ContextBindable::add_referrer (Context *c)
{
  referrers_.push_back (c);
}


void
ContextBindable::remove_referrer (Context *c)
{
  referrers_.erase (find (referrers_.begin(), referrers_.end(), c));
}

ContextBindable::ContextBindable (const char* my_name, Context* parent, Domain* d,
                                  handler_tag_FMStructDescRec * format_list) :
  Proactive (format_list),
  domain_ (d),
  parent_ (parent),
  is_placeholder_ (true),
  attributes_ (NULL),
  my_name_(strdup(my_name))
{
  /*
   *  Insert into the global object map
   */
  stamp_pair sp (get_timestamp(), this);
  objectId::global_object_map.insert (object_map_pair (this->obj_id, sp));
  if (parent != 0)
	add_referrer (parent);
}


ContextBindable::~ContextBindable()
{
  free(my_name_);
  free_attr_list (attributes_);

  for (list<Context*>::iterator i = referrers_.begin();
       i != referrers_.end();
       i++)
    {      
      //  tell the referring context object not to notify me since I'm going away anyway, and
      //  I already know I'm being unbound (since I'm calling unbind).  I think this is
      //  a little schizo but I'm not sure how to deal with it.
      Context *c = *i;
      c->unbind (this, false);
    }

  referrers_.clear();
}


void
ContextBindable::make_real()
{
  set_up_stones();  
  is_placeholder_ = false;
  if (attributes_ == NULL)
    attributes_ = create_attr_list();
  send_creation_event();
}

void 
ContextBindable::send_event(BaseEventWrapper* ev, unsigned short which_event)
{
  /*FIXME: Need to uncomment and handle this code in the new system
  if (domain_ != 0)
    domain_->send_wrapped_event(ev);
  */
    
 Proactive::send_event_(ev->event(), which_event);
}
bool
ContextBindable::is_placeholder()
{
  return is_placeholder_;
}




/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.9  2007/12/04 21:06:19  pmw
 *  Merge changes from evpath branch.  Also probably includes
 *  Darwin fixes.  Compiles on isleroyale and marquesas, I think
 *  I'm misconfigured somehow for building on gondor.
 *
 *  Revision 1.8.2.1  2007/01/05 22:40:33  pmw
 *  interim commit for evpath changes, new branch
 *
 *  Revision 1.8  2003/09/29 00:57:24  mansour
 *  Added a checkpointing thread.
 *  Fix the code for XML dump and load
 *  Fix the code for get_full_name
 *
 *  Revision 1.7  2003/02/04 17:27:42  mansour
 *  *** empty log message ***
 *
 *  Revision 1.6  2003/01/30 19:16:26  mansour
 *  Add support for domain events
 *
 *  Revision 1.5  2003/01/29 03:36:39  mansour
 *  A bunch of changes to make get_full_name work. Add a my_name_ at the ContextBindable level. This really helps for debugging only and can be removed. Fixed the algorithm for get_full_name.
 *
 *  Also, added calls to generate events on context_creation and deletion.
 *
 *  Revision 1.4  2002/08/05 22:49:25  pmw
 *  Changes to support gcc 3.0 and xerces-c 2.0
 *
 *  Revision 1.3  2002/04/09 19:13:22  pmw
 *  got rid of get_full_name() nonsense.  now I just use the name the
 *  caller gave me and if it's not fully qualified, well, life is tough, you
 *  oughta know the name since you used it to get a context ID to begin with.
 *
 *  Revision 1.2  2002/03/28 04:46:07  pmw
 *  changes for getting names of referrers.
 *
 *  Revision 1.1  2001/10/09 21:37:55  pmw
 *  configuration changes
 *
 *  Revision 1.5  2001/09/23 22:55:53  pmw
 *  Contexts now support attributes of their own.  Practically, this is supported
 *  by having ContextBindable host the attribute list instead of Entity/Context.  Also
 *  added the usual functions on the client side and handlers for getting/setting
 *  this attr list for Contexts.
 *
 *  Revision 1.4  2001/07/15 02:29:37  pmw
 *  Fixed context creation problem and added some changes for Contexts to return
 *  their binding lists.
 *
 *  Revision 1.3  2001/07/02 19:42:30  pmw
 *  some more little tweaks.
 *
 *  Revision 1.2  2001/07/02 17:23:02  pmw
 *  needs definition of Context class to call unbind method
 *
 *  Revision 1.1  2001/07/02 15:07:43  pmw
 *  removed ContextBindable class def and impl to their own source files
 *
 *
 */



string
ContextBindable::get_full_name ()
{
  string fullname;
    
  fullname = my_name_;
  if (0 == this->parent_)    
    return fullname;
   
  Context* p = parent_;  
  /*ContextBindable* c = this;*/
  while (p->parent_) {
    /*
	const bindingMap& parent_name_map = p->get_name_map();
    bindingMap::const_iterator i;
    binding_pair bp;
    bool found = false;
    char *n;
       
    for (i = parent_name_map.begin(); !found && i != parent_name_map.end(); i++)
       {
         bp = (*i).second;
         n = const_cast<char*> ((*i).first);
         found = (bp.second == c);
       }
      
    assert (found == true);
    */    
    fullname = "/" + fullname;
	fullname = p->my_name_ + fullname;
       
    /*c = p;*/
    p = p->parent_;
  }
  fullname = "/" + fullname;
  return fullname;
}

size_t
ContextBindable::get_name_list (vector<string>& name_vec)
{
  Context* p;
  string fullname;
  list<Context*>::iterator a_referrer;
  
  for (a_referrer = referrers_.begin(); a_referrer != referrers_.end(); a_referrer++)
    {
      p = *a_referrer;
      fullname = p->get_full_name ();
      name_vec.push_back (fullname);
    }

  return name_vec.size();
}

