/*
 *
 *  $Id: Domain.cc,v 1.16 2008-01-01 00:38:18 pmw Exp $
 *
 */
#include "config.h"

extern "C" {
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <memory.h>
#include <string.h>
  //#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include "atl.h"
#include "evpath.h"

};

#ifdef __GNUG__
#if (__GNUC__ == 2)
#include <alloc.h>
#endif
#endif

extern "C" 
{
#include "common/formats.h"
#include "utils.h"
};

#include "Domain.h"
#include "EventWrapper.h"



const char Domain::PDS_NAME_SEPARATOR = '/';
const unsigned long Domain::make_name_path = 0x1;
const unsigned long Domain::make_placeholders = 0x2;

vector <Domain*> Domain::global_domain_list_;


Domain::Domain (const char* name, const char* type, const int version, const char* app)
  : Proactive (),
    attributes_ (create_attr_list()),
    root_(0)
{
  root_ = new Context ("/", this, 0);
  set_up_stone(DOMAIN_CHANGE);
  domain_name_ = (name == NULL ? NULL : strdup (name));
  domain_type_ = (type == NULL ? NULL : strdup (type));
  domain_version_ = version;
  application_name_ = (app == NULL ? NULL : strdup (app));

  /*
   *  Insert into the list
   */
  Domain::global_domain_list_.push_back (this);

  /*
   *  Insert into the global object map
   */
  stamp_pair sp (get_timestamp(), this);
  global_object_map.insert (object_map_pair (this->obj_id, sp));

  /*
   *  Our root context is already real
   */
  root_->make_real();

  /*
   *  Fill out the rest of the structure
   */
  create_time_ = time (NULL);
  expiration_ = 86400;	/* = 60*60*24 */
  
}

Domain::~Domain()
{
  pdsTrace_out (pdsengineVerbose, "<Domain::~Domain() this=%p, name=""%s"" >", this, domain_name_);

  for (list<Entity*>::iterator i = entity_list_.begin();
       i != entity_list_.end();
       i++)
    {
      Entity *e = *i;
      pdsTrace_out (pdsengineVerbose, "deleting entity %p", e);
      delete e;
    }

  pdsTrace_out (pdsengineVerbose, "erasing entity list");
  entity_list_.erase (entity_list_.begin(), entity_list_.end());

  root_->kill_bindings();

  free (domain_name_);
  free (domain_type_);
  free (application_name_);
  delete root_;
  pdsTrace_out (pdsengineVerbose, "</Domain::~Domain()>");
}


const char*
Domain::split_name (const char* source)
{
  const char* f;
  /*
   *  Try to deal with trailing separator characters.
   *  Check from the end of the source string and work backward.  Normally
   *  we'd just start at the end of the string and look for the first
   *  separator character.  If the string ends with a sep char, though,
   *  the search ends immediately and we have a bogus split.  So, start
   *  at the end of the string - 1.  This works because if the last component
   *  of the string more than one character long, we'll correctly find the
   *  next-to-last separator.  If the last component of the name is only one
   *  character long, skipping a non-separator last character will point directly
   *  at the separator we want to find.  Skipping a separator last character should
   *  point at a non-separator character, which means we'll correctly find the
   *  next-to-last one.  This will break if someone ends a name with two or more
   *  separator characters, but that's an invalid name anyway since it implies a
   *  null name component.
   *
   *  source + strlen (source) points at the terminating \0 - subtract 1 to point
   *  at the last actual character, and 1 again to skip that character.
   */
  if (strlen (source) < 2) {
    f = source;
  } else {
    for (f = source + strlen (source) - 2; (*f != PDS_NAME_SEPARATOR && f != source); f--)
      {
        ;
      }
  }

  // we should have stopped at either the first character or the rightmost usable
  // separator, so in either case it's OK to simply return f.
  return f;
}


const char*
Domain::split_name (const char* source, char*& head, char*& tail)
{
  const char* f;
  /*
   *  same commentary as in above version, elided for space :)
   */
  f = split_name (source);
  
  if (f == source) {
    head = NULL;
    /*
     *  we possibly have a string of the form "/name".  In this case
     *  we can be pointing to the front of the string and have it be
     *  a separator.  In either case, we don't have a head component.
     *  However, we have to make sure to strdup from the first non-sep
     *  char if f is a separator.
     */
    if (*f == PDS_NAME_SEPARATOR) {
      if (strlen (f) > 1) {
	tail = strdup (source + sizeof (char));
      } else {
	tail = NULL;
      }
    } else {
      // no separator found
      tail = strdup (source);
    }
  } else {
    // found a separator
    const char* g = f;
    // have to be tricky with head since there's no null terminator
    // to work with
    head = (char*) calloc (f - source + 1, sizeof (char));
    strncpy (head, source, f - source);
    tail = strdup (++g);
  }      
  
  return f;
}

int
Domain::bind_context(Context* source, const char* full_name)
{
  char *f, *objname;
  Domain::split_name (full_name, f, objname);
  Context* parent = resolve_or_create_context (f, 0);
  if (parent == 0) return 0;

  parent->bind(objname, binding_pair (context_binding, source));
  return 1;
}

Context*
Domain::get_context_of_name (const char *name,
                             char*& last_component,
                             Context* initial_context,
                             const unsigned long flags)
{
  char *next_token = NULL;
  char sep[2] = { PDS_NAME_SEPARATOR, '\0' };
  Context *parent;
  char *f, *objname;
  binding_pair bp;

  parent = (initial_context == NULL) ? get_root_context() : initial_context;

  /*
   *  Remove the last component of the name (cdr(name), i guess).
   */
  Domain::split_name (name, f, objname);

  /*
   * save objname for the caller
   */
  last_component = objname;

  /*
   *  now tokenize the name
   *
   *  if f starts out at NULL, we were passed a string with no initial separator
   *  and only one component (already in last_component from the assignment just
   *  above).  The context of any such name is the initial context, whether we 
   *  were given it or we defaulted it to the root context.  So just skip the 
   *  tokenization and return it.
   *
   *  Otherwise, travel down the tree of contexts as we tokenize the name.
   */
  if (f != NULL) {
    next_token = strtok (f, sep);
  
    while (next_token && parent) {
      bp = parent->lookup (next_token);
      if (bp != Context::null_binding_pair) {
	parent = dynamic_cast<Context*>(bp.second);
      } else if (flags & Domain::make_name_path) {
	Context *child;
	
	child = new Context(next_token, this, parent);
	parent->bind (next_token, binding_pair (context_binding, child));
	parent = child;			  
      } else {
	/*
	 *  Bah!  we didn't find the next component of the path,
	 *  and we haven't been told to create it as we go,
	 *  so we gotta give up and return NULL.
	 */
	parent = NULL;
      }
      
      if (parent
	  && parent->is_placeholder()
	  && ! (flags & Domain::make_placeholders)) {
	parent->make_real();
      }

      next_token = strtok (NULL, sep);
    }
    
    free (f);
  }
  
  /*
   *  parent should now point to the context containing the last name
   *  component OR be NULL because the name wasn't resolved
   */
  return parent;
}



ContextBindable*
Domain::resolve_or_create_name (const char *fullname,
                                binding_type name_type,
                                Context* initial_context,
                                bool create_placeholder)
{
  Context *parent;
  ContextBindable* rval = NULL;
  Context *c = NULL;
  Entity *e = NULL;
  char* objname;
  binding_pair bp;
  unsigned long flags = Domain::make_name_path;

  if (strcmp(fullname, "/") == 0)
    return root_;
    
  if (create_placeholder) flags |= Domain::make_placeholders;
  // allocates memory for objname, free before returning
  parent = get_context_of_name (fullname, objname, initial_context, flags);

  /*
   *  At this point, parent should point to 
   *  the last Context referred to by the name string.  We'll
   *  get it and search for our last name (that we picked off
   *  above.
   */
  bp = parent->lookup (objname);

  if (bp == Context::null_binding_pair) {
    if (name_type == context_binding) {
      c = new Context (objname, this, parent);
      bp.first = context_binding;
      bp.second = c;
    } else {
      e = new Entity (objname, this, parent);
      bp.first = entity_binding;
      bp.second = e;	  
    }
    parent->bind (objname, bp, fullname);
  }

  /*
   *  Now we have bp set up correctly, and whatever is in bp.second is
   *  the pointer we want to return - whether we found it in the lookup
   *  or we just created it.  This is also irrespective of whether we
   *  promote the binding if it's a placeholder below - that doesn't
   *  change the pointer value, we just call make_real on the entity.
   */

  rval = bp.second;

  /*
   *  We always create placeholders.  If it should have been an entity,
   *  we change it now and submit an event to the entity's creation
   *  channel.
   */
  
  if (rval->is_placeholder() && !create_placeholder) {
    /*
     *  if we found a placeholder, and the user is trying to create
     *  an entity, we have to "promote" it to entity_binding status and fire its
     *  creation event.
     */
    rval->make_real();
  }

  /*
   *  At this point, rval points to an already existing entity/context 
   *  that we looked up, or points to an object we just created, 
   *  Either way, we're ready to return.
   */
  free (objname);
  return rval;
}


ContextBindable*
Domain::resolve_name (const char *fullname, 
                      Context* initial_context)
{
  ContextBindable* rval = NULL;
  Context *parent;
  char *objname;
  binding_pair bp;
  const char * other = fullname;

  // allocates memory for objname, free before returning
  parent = get_context_of_name (fullname, objname, initial_context);

  /*
   *  At this point, parent should point to 
   *  the last Context referred to by the name string.  We'll
   *  get it and search for our last name (that we picked off
   *  above.
   *
   *  If parent is NULL, some part of the context lookup failed.  So 
   *  just return NULL.
   */
  if (parent != NULL)
    {
      if (objname == NULL) {
        // we really want the parent
        rval = parent;
      } else {
        bp = parent->lookup (objname);
        if (bp != Context::null_binding_pair 
          && ! bp.second->is_placeholder())
          rval = bp.second;
        else 
        {
          printf("We've reached the point where the context is a placeholder, so we return null\
                  the name is %s\n", other);
        }
      }
    }

  /*
   *  At this point, rval is either NULL or points to an already
   *  existing entity/context that we looked up.
   *  Either way, we're ready to return.
   */
  free (objname);
  return rval;
}


Context* 
Domain::resolve_or_create_context (const char *name,
                                   Context *initial_context,
                                   bool create_placeholder)
{
  return dynamic_cast<Context*>
    (resolve_or_create_name (name, context_binding, initial_context, create_placeholder));
}

Context* 
Domain::resolve_context (const char *name, Context *initial_context)
{
  return dynamic_cast<Context*>
    (resolve_name (name, initial_context));
}
  
Entity* 
Domain::resolve_or_create_entity (const char *name,
                                  Context *initial_context,
                                  bool create_placeholder)
{
  return dynamic_cast<Entity*> (resolve_or_create_name (name, entity_binding, 
                                                        initial_context,
                                                        create_placeholder));
}

  
Entity* 
Domain::resolve_entity (const char *name, Context *initial_context)
{
  return dynamic_cast<Entity*> (resolve_name (name, initial_context));
}




size_t
Domain::find_matching_domains (const char* domain_name,
                               const char* domain_type,
                               const int domain_version,
                               const char* application_name,
                               vector<Domain*>& matches)
{
  vector <Domain*>::iterator i;
  for (i = Domain::global_domain_list_.begin();
       i != Domain::global_domain_list_.end();
       i++) {
    int keepit = 1;
    Domain* d = *i;

    /*
     *  if the domain matches the passed-in characteristics,
     *  add it to the vector.  when we're done, we'll take
     *  everything in the vector and add it to the list that
     *  gets sent back to the client
     */      
    if (domain_name != NULL
	&& strcmp (domain_name, "*") != 0
	&& d->domain_name_ != NULL
	&& strcmp (domain_name, d->domain_name_) != 0) {
      keepit = 0;
    }

    if (domain_type != NULL
	&& keepit
	&& strcmp (domain_type, "*") != 0
	&& d->domain_type_ != NULL
	&& strcmp (domain_type, d->domain_type_) != 0) {
      keepit = 0;
    }

    if (keepit &&
	domain_version != -1 &&
	domain_version != d->domain_version_) {
      keepit = 0;
    }

    if (application_name != NULL
	&& keepit 
	&& strcmp (application_name, "*") != 0
	&& d->application_name_ != NULL
	&& strcmp (application_name, d->application_name_) != 0) {
      keepit = 0;
    }
	
    /*
     *  if keepit is still true, we can add this domain to the list
     */
    if (keepit)	{
      matches.push_back (d);
    }
  }

  return matches.size();
}

Domain* 
Domain::find_first_matching_domain (const char* domain_name,
                                    const char* domain_type,
                                    const int domain_version,
                                    const char* application_name)
{
  vector<Domain*> v;
  Domain::find_matching_domains (domain_name,
                                 domain_type,
                                 domain_version,
                                 application_name,
                                 v);
  if (v.size() == 0)
    return NULL;
  else
    return v[0];
}


int
Domain::remove_context (Context* target)
{  
  if (target != 0) {
    target->send_deletion_event();
  }

  delete target;
  return 1;
}


int
Domain::remove_entity (Entity* dead_guy)
{
  dead_guy->send_deletion_event();  
  delete dead_guy;
  return 1;
}


int
Domain::bind_entity (const char* source_name, pds_entity_id_t entity_id,
                     pds_context_id_t context_id, const char* new_name)
{
  Entity* entity_ptr;
  Context* target_context;
  Context* init_context;
  char* ctx_name = NULL;
  char* ent_name = NULL;
  int rv = 1;

  /*
   *  Find the source entity
   */
  if (source_name != NULL) {
    entity_ptr = resolve_entity (source_name);
  } else {
    entity_ptr = objectId::get_entity_ptr_from_id (entity_id);
  }

  if (entity_ptr == NULL) {
    rv = -1;
  } else {
    // get the context for the new name
    Domain::split_name (new_name, ctx_name, ent_name);
    init_context = objectId::get_context_ptr_from_id (context_id);
    target_context = resolve_or_create_context (ctx_name, init_context);
    
    // bind the entity pointer in the acquired context
    target_context->bind (ent_name, binding_pair (entity_binding, entity_ptr));
    entity_ptr->send_bind_event();
  }
                                                                       
  free (ctx_name); free (ent_name);
  return rv;
}

int 
Domain::remove_binding(const char* name, Context* initial_context)
{
  Context *target_context = NULL;
  char *bound_name = NULL;
  binding_pair bp;
  int rv = -1;
  target_context = get_context_of_name (name, bound_name, initial_context);

  if (target_context == NULL) {
    rv = -1;
  } else {
    bp = target_context->unbind (bound_name);
    if (bp != Context::null_binding_pair) {
      if (bp.first == entity_binding) {
	Entity *e = dynamic_cast<Entity*> (bp.second);
	e->send_unbind_event();
      } else if (bp.first == context_binding) {
	Context *c = dynamic_cast<Context*> (bp.second);
	c->send_unbind_event(name);
      }
      rv = 1;
    }
  }

  free (bound_name);
  return rv;
}

int
Domain::remove_binding (const char* name)
{
   return remove_binding(name, NULL);
}
int
Domain::remove_binding (const char* name, pds_context_id_t init_context_id)
{
  Context *initial_context = NULL;

  initial_context = objectId::get_context_ptr_from_id (init_context_id);
 
  return remove_binding(name, initial_context);
}

void 
Domain::add_entity_to_list (Entity* new_guy) { 
  entity_list_.push_back (new_guy); 
}

void 
Domain::remove_entity_from_list (Entity* dead_guy) { 
  pdsTrace_out (pdsengineVerbose, "<Domain::remove_entity_from_list (Entity* dead_guy)>");

  pdsTrace_out (pdsengineVerbose, "domain %p removing entity %p from entity_list_", this, dead_guy);
  entity_list_.remove (dead_guy); 

  pdsTrace_out (pdsengineVerbose, "</Domain::remove_entity_from_list (Entity* dead_guy)>");
}

void
Domain::add_context_to_list (Context* new_guy) {
  context_list_.push_back (new_guy); 
}

void 
Domain::remove_context_from_list (Context* dead_guy) { 
  context_list_.remove (dead_guy); 
}


void
Domain::send_wrapped_event(BaseEventWrapper* sub_ev)
{
  pds_domain_change_event ev;
  if (sub_ev->my_type() == ENTITY_CHANGE_EVENT)
    ev.type = PDS_DOMAIN_CHANGE_ENTITY_CHANGE;  
  else if (sub_ev->my_type() == CONTEXT_CHANGE_EVENT)
      ev.type = PDS_DOMAIN_CHANGE_CONTEXT_CHANGE;  
      
  ev.event_desc = (char*)sub_ev->desc();
  ev.d_name = domain_name_;
  ev.d_type = domain_type_;
  ev.d_version = domain_version_;
  ev.d_app_name = application_name_;
  ev.domain_id = objectId::make_pds_domain_id(this);
  ev.event_type = sub_ev->type();
  ev.event_fullname = (char*)sub_ev->full_name();

  send_event_(&ev, DOMAIN_CHANGE);
}
/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.15  2007/12/24 14:40:26  eisen
 *  Partial conversion to FFS-based CMRPC.
 *
 *  Revision 1.14  2007/12/04 21:06:19  pmw
 *  Merge changes from evpath branch.  Also probably includes
 *  Darwin fixes.  Compiles on isleroyale and marquesas, I think
 *  I'm misconfigured somehow for building on gondor.
 *
 *  Revision 1.13.2.2  2007/01/09 01:37:29  pmw
 *  various changes including a fix for the delete context server hang
 *
 *  Revision 1.13.2.1  2007/01/05 22:40:33  pmw
 *  interim commit for evpath changes, new branch
 *
 *  Revision 1.13  2005/02/22 19:25:32  pmw
 *  modifications to keep Entity's destructor from hamstringing Domain's
 *  destructor.  the entity list iterator in ~Domain() was being whacked
 *  by ~Entity().  If this functionality is needed, it needs to be
 *  rethought.
 *
 *  Revision 1.12  2004/11/03 06:56:50  mansour
 *  Added APIs for context binding. The XML dump routine expands bounded contexts into a full tree and the load routine loads it as separate contexts.
 *
 *  Revision 1.11  2003/10/21 19:54:13  eisen
 *  Various tweaks and hacks designed to get this to compile cleanly with
 *  Forte 7.
 *
 *  Revision 1.10  2003/09/29 00:57:23  mansour
 *  Added a checkpointing thread.
 *  Fix the code for XML dump and load
 *  Fix the code for get_full_name
 *
 *  Revision 1.9  2003/02/04 17:26:42  mansour
 *  set the domain event type
 *
 *  Revision 1.8  2003/01/30 19:16:26  mansour
 *  Add support for domain events
 *
 *  Revision 1.7  2003/01/29 03:36:39  mansour
 *  A bunch of changes to make get_full_name work. Add a my_name_ at the ContextBindable level. This really helps for debugging only and can be removed. Fixed the algorithm for get_full_name.
 *
 *  Also, added calls to generate events on context_creation and deletion.
 *
 *  Revision 1.6  2002/10/31 23:29:53  pmw
 *  2 fixes:
 *  - single character context names (in particular "/") crashed
 *  - fixed memory stomping bug in get_binding_list when only asking for
 *  entity or context bindings
 *
 *  Revision 1.5  2002/10/31 21:09:01  pmw
 *  Fixed bug where name lookups crash when names aren't present.  Nice
 *  test coverage I have going.
 *
 *  Revision 1.4  2002/10/31 18:20:39  pmw
 *  Fixed bug in context name mangling.
 *
 *  Revision 1.3  2002/08/05 22:49:25  pmw
 *  Changes to support gcc 3.0 and xerces-c 2.0
 *
 *  Revision 1.2  2002/04/09 19:13:22  pmw
 *  got rid of get_full_name() nonsense.  now I just use the name the
 *  caller gave me and if it's not fully qualified, well, life is tough, you
 *  oughta know the name since you used it to get a context ID to begin with.
 *
 *  Revision 1.1  2001/10/09 21:37:55  pmw
 *  configuration changes
 *
 *  Revision 1.5  2001/10/07 22:19:15  pmw
 *  Added domain_change event structures so that Proactive class is always correctly
 *  initialized.
 *
 *  Revision 1.4  2001/07/15 02:29:37  pmw
 *  Fixed context creation problem and added some changes for Contexts to return
 *  their binding lists.
 *
 *  Revision 1.3  2001/07/02 04:02:03  pmw
 *  interim checkin for changes to make Contexts also support events.  compiles
 *  but tests probably do not run.
 *
 *  Revision 1.2  2001/06/12 03:14:32  pmw
 *  minor fixes
 *
 *  Revision 1.1.1.1  2000/12/13 22:14:22  pmw
 *  imported sources
 *
 *  Revision 1.13  2000/12/06 01:57:00  pmw
 *  Fixed problems with memory leakage and poor server shutdown mechanics
 *
 *  Revision 1.12  2000/11/30 01:47:54  pmw
 *  rearranged include files to fix inappropriate inclusions
 *
 *  Revision 1.11  2000/11/29 04:37:36  pmw
 *  changed channel-handling again, Entity now performs most of its own
 *  change event sends
 *
 *  Revision 1.10  2000/11/28 22:49:26  pmw
 *  1) moved CManager stuff to Proactive class, since other classes
 *  don't need to deal with it any longer
 *  2) moved object ID management out of Domain and into objectId.
 *
 *  Revision 1.9  2000/11/27 23:15:51  pmw
 *  Fixed problem where bindings weren't being promoted from placeholders
 *  in the Context binding_map.  Context now supports a promote_binding call,
 *  which calls make_real on whatever the binding was bound to.  For entities,
 *  this does the right thing (sets up the channel, etc)
 *
 *  Revision 1.8  2000/11/27 21:34:41  pmw
 *  Fixed bug in get_context_of_name where passing initial NULL to strtok
 *  on Linux causes segfault.  I should have done it this way to begin with
 *  anyway.
 *
 *  Revision 1.7  2000/11/22 21:42:33  pmw
 *  proactivity based on entity events now appears to work correctly.  In particular,
 *  creation events based on the prior registration of placeholder entities and their
 *  later "creation" as real entities works.
 *
 *  Revision 1.6  2000/11/21 21:35:23  pmw
 *  Domain change events now work correctly.  moved event transmission
 *  responsibilities to Proactive class.
 *
 *  Revision 1.5  2000/11/21 00:23:45  pmw
 *  Since I didn't have enough to do, a little code reorganization to keep
 *  things from getting dramatically out of hand.  Fixed a couple of
 *  bugs-in-waiting.
 *
 *  Revision 1.4  2000/11/20 04:18:24  pmw
 *  Deleted objects now removed from global external reference map
 *
 *  Revision 1.3  2000/11/19 23:28:26  pmw
 *  wp_server now supports bulk load of data with -f command-line option.  Also added
 *  command message so bulk loads can happen on-line.
 *
 *  Revision 1.2  2000/10/30 22:44:13  pmw
 *  basic test program works after lots of changes.
 *  ** server still leaks memory; entries are not removed
 *  from global_object_map when contexts or entities are
 *  destroyed.
 *
 *  Revision 1.1  2000/10/25 21:44:42  pmw
 *  Compiles after major revisions to support multiple WP services on
 *  client-side.  This should eventually allow a client to use services by
 *  linking in the library directly or by communicating using CMRPC.
 *
 *
 */
 
