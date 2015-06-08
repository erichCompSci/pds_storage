/*
 *
 *  $Id: Context.cc,v 1.13 2008-06-26 19:24:17 pmw Exp $
 *
 */
#include "config.h"

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
#include "EventWrapper.h"
#include "Context.h"
#include "Domain.h"

extern "C" {
#include "common/formats.h"
#include "agent/pdsdefs.h"
#include "utils.h"
};

binding_pair Context::null_binding_pair (no_binding, NULL);

Context::Context (const char* my_name, Domain* d, Context* p) :
  ContextBindable (my_name, p, d, context_DescRecs )
{
  set_up_stones();
  d->add_context_to_list(this);
}

Context::Context (const char* my_name, Domain *d, Context* p, const char* name) :
  ContextBindable (my_name, p, d, context_DescRecs )
{
  set_up_stones();
  p->bind (name, binding_pair (context_binding, this));
  d->add_context_to_list(this);
}

Context::~Context()
{
  bindingMap::iterator i;

  pdsTrace_out ( pdsengineVerbose, "<Context::~Context() this=%p>", this );
  pdsTrace_out ( pdsengineVerbose, "Context name map size %d", name_map_.size() );  

  domain_->remove_context_from_list(this);

  pdsTrace_out (pdsengineVerbose, "</Context::~Context()>");
}


void
Context::kill_bindings()
{
  bindingMap::iterator i;

  pdsTrace_out ( pdsengineVerbose, "<Context::kill_bindings() this=%p>", this );
  pdsTrace_out ( pdsengineVerbose, "Context name map size %d", name_map_.size() );  

  for (i = name_map_.begin(); i != name_map_.end(); ++i)
    {
      binding_pair bp = (*i).second;
      // free name that was bound
      //free ((*i).first);            
      bp.second->unbound_from (this);
    }

  name_map_.erase (name_map_.begin(), name_map_.end());
  pdsTrace_out (pdsengineVerbose, "</Context::kill_bindings() this=%p>", this);
}


int
Context::level_count (binding_type nt)
{
  bindingMap::iterator i;
  int c = 0;

  for (i = name_map_.begin(); i != name_map_.end(); i++)
    {
      if ((*i).second.first == nt && !((*i).second.second)->is_placeholder())
        c++;
    }

  return c;
}

    

int
Context::bind (const char* name, const binding_pair& bp, const char* fullname)
{
  pair<char* const, binding_pair> new_guy (strdup (name), bp);

  pair<bindingMap::iterator, bool> p = name_map_.insert (new_guy);
  if (p.second)
    {
      //      string s = cb->get_full_name ();
      bp.second->add_referrer (this);
      //      send_bind_event (const_cast<char*> (s.c_str()));
      send_bind_event (fullname != NULL ? fullname : name);
      return 1;
    }
  else
    {
      return 0;
    }
}  


binding_pair
Context::lookup (const char* name) const
{
  bindingMap::const_iterator i = name_map_.find (name);
  if (i == name_map_.end())
    return Context::null_binding_pair;
  else
    return (*i).second;
}

binding_pair
Context::lookup (const ContextBindable* cb) const
{
  bindingMap::const_iterator i;
  binding_pair bp;
  bool found = false;
//  char *n;

  for (i = name_map_.begin(); !found && i != name_map_.end(); i++)
    {
      bp = (*i).second;
//x      n = const_cast<char*> ((*i).first);
      found = (bp.second == cb);
    }

  return found ? bp : Context::null_binding_pair;
}

binding_pair
Context::unbind (const char* name)
{
  bindingMap::iterator i = name_map_.find (name);
  binding_pair bp;
  char *n;

  if (i != name_map_.end())
    {
      bp = (*i).second;
      n = const_cast<char*> ((*i).first);
      bp.second->unbound_from (this);
      name_map_.erase (i);
      free (n);
    }
  return bp;
}


binding_pair
Context::unbind (const ContextBindable *cb, const bool notify)
{
  ContextBindable* ccb = const_cast<ContextBindable*> (cb);
  return unbind (binding_pair (no_binding, ccb), notify);
}


binding_pair
Context::unbind (const binding_pair& findit, const bool notify)
{
  bindingMap::iterator i;
  binding_pair bp;
  bool found = false;
  char *n;

  for (i = name_map_.begin(); !found && i != name_map_.end(); i++)
    {
      bp = (*i).second;
      n = const_cast<char*> ((*i).first);
      findit.first != no_binding ? found = (bp == findit) : found = (bp.second == findit.second);
    }
  
  if (found)
    {

      if (notify) 
        bp.second->unbound_from (this);
      name_map_.erase (n);
      free (n);
    }
  else
    {
      bp = Context::null_binding_pair;
    }

  return bp;
}



void
Context::make_real()
{
  ContextBindable::make_real();
}

void
Context::send_bind_event (const char *n)
{
  pds_context_u_bind_change_ntf evt;
  evt.type = PDS_CONTEXT_CHANGE_BIND;
  evt.fullname = const_cast<char*> (n);
  evt.context_id = objectId::make_pds_context_id (domain_, this);
  ContextEventWrapper wrap (evt);
  send_event (&wrap, CONTEXT_BIND_UNBIND );
  //  free (n);
}

void
Context::send_unbind_event (const char *n)
{
  pds_context_u_bind_change_ntf evt;
  evt.type = PDS_CONTEXT_CHANGE_UNBIND;
  evt.fullname = const_cast<char*>(n);
  evt.context_id = objectId::make_pds_context_id (domain_, this);
  ContextEventWrapper wrap (evt);
  send_event (&wrap, CONTEXT_BIND_UNBIND);

  //  free (n);
}

  
void
Context::send_creation_event()
{
  pds_context_exist_change_ntf evt;
  evt.type = PDS_CONTEXT_CHANGE_CREATION;
  string fullname = get_full_name();
  evt.fullname = (char*)fullname.c_str();
  evt.context_id = objectId::make_pds_context_id (domain_, this);
  ContextEventWrapper wrap (evt);
  send_event (&wrap, CONTEXT_CREATE_DESTROY);

}


void
Context::send_deletion_event()
{
  pds_context_exist_change_ntf evt;
  evt.type = PDS_CONTEXT_CHANGE_DELETION;
  string fullname = get_full_name();
  evt.fullname = (char*)fullname.c_str();
  evt.context_id = objectId::make_pds_context_id (domain_, this);  
  ContextEventWrapper wrap (evt);
  send_event (&wrap, CONTEXT_CREATE_DESTROY);
}


/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.12  2008/01/01 00:38:18  pmw
 *  Changes to make PDS work with the new FFS libraries
 *
 *  Revision 1.11  2007/12/24 14:40:26  eisen
 *  Partial conversion to FFS-based CMRPC.
 *
 *  Revision 1.10  2007/12/04 21:06:18  pmw
 *  Merge changes from evpath branch.  Also probably includes
 *  Darwin fixes.  Compiles on isleroyale and marquesas, I think
 *  I'm misconfigured somehow for building on gondor.
 *
 *  Revision 1.9.2.2  2007/01/09 01:37:28  pmw
 *  various changes including a fix for the delete context server hang
 *
 *  Revision 1.9.2.1  2007/01/05 22:40:33  pmw
 *  interim commit for evpath changes, new branch
 *
 *  Revision 1.9  2003/10/21 19:54:12  eisen
 *  Various tweaks and hacks designed to get this to compile cleanly with
 *  Forte 7.
 *
 *  Revision 1.8  2003/09/29 00:57:22  mansour
 *  Added a checkpointing thread.
 *  Fix the code for XML dump and load
 *  Fix the code for get_full_name
 *
 *  Revision 1.7  2003/02/12 20:50:07  eisen
 *  Tweaks to kill warnings on Irix compilers.
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
 *  Revision 1.3  2002/04/09 19:13:21  pmw
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
 *  Revision 1.6  2001/07/15 02:29:37  pmw
 *  Fixed context creation problem and added some changes for Contexts to return
 *  their binding lists.
 *
 *  Revision 1.5  2001/07/02 19:42:30  pmw
 *  some more little tweaks.
 *
 *  Revision 1.4  2001/07/02 17:55:22  pmw
 *  hopefully final changes to make contexts emit bind/unbind events
 *
 *  Revision 1.3  2001/07/02 15:07:43  pmw
 *  removed ContextBindable class def and impl to their own source files
 *
 *  Revision 1.2  2001/07/02 04:02:03  pmw
 *  interim checkin for changes to make Contexts also support events.  compiles
 *  but tests probably do not run.
 *
 *  Revision 1.1.1.1  2000/12/13 22:14:23  pmw
 *  imported sources
 *
 *  Revision 1.5  2000/12/06 01:57:00  pmw
 *  Fixed problems with memory leakage and poor server shutdown mechanics
 *
 *  Revision 1.4  2000/11/27 23:15:50  pmw
 *  Fixed problem where bindings weren't being promoted from placeholders
 *  in the Context binding_map.  Context now supports a promote_binding call,
 *  which calls make_real on whatever the binding was bound to.  For entities,
 *  this does the right thing (sets up the channel, etc)
 *
 *  Revision 1.3  2000/11/27 21:38:29  pmw
 *  Fixed bug where I was freeing memory before removing entries from the
 *  binding map.
 *
 *  Revision 1.2  2000/11/21 00:23:44  pmw
 *  Since I didn't have enough to do, a little code reorganization to keep
 *  things from getting dramatically out of hand.  Fixed a couple of
 *  bugs-in-waiting.
 *
 *  Revision 1.1  2000/10/25 21:44:41  pmw
 *  Compiles after major revisions to support multiple WP services on
 *  client-side.  This should eventually allow a client to use services by
 *  linking in the library directly or by communicating using CMRPC.
 *
 *
 */
