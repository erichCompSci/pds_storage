#ifndef _DOMAIN_H_
#define _DOMAIN_H_

/*
 *
 *  $Id: Domain.h,v 1.9 2007-12-04 21:06:19 pmw Exp $
 *
 */

extern "C"
{
#include <sys/types.h>
#include <limits.h>

#include "atl.h"
}

#include <cstdlib>
#include <cstring>
#include <list>
#include <vector>
#include "get_hash_map.h"



#include "objectId.h"
#include "Proactive.h"
#include "Context.h"
#include "Entity.h"

class BaseEventWrapper;

class Domain : public objectId, public Proactive
{
 public:

  static const char PDS_NAME_SEPARATOR;
  static const unsigned long make_name_path;
  static const unsigned long make_placeholders;

  static vector <Domain*> global_domain_list_;
  static size_t find_matching_domains (const char* domain_name,
                                       const char* domain_type,
                                       const int domain_version,
                                       const char* application_name,
                                       vector<Domain*>& matches);

  static Domain* find_first_matching_domain (const char* domain_name,
                                              const char* domain_type,
                                              const int domain_version,
                                              const char* application_name);


  static const char* split_name (const char *source);

  static const char* split_name (const char *source, char*& head, char*& tail);

 public:

  char *domain_name_;
  char *domain_type_;
  int domain_version_;
  char *application_name_;
  int expiration_;
  time_t create_time_;
  time_t last_modif_;
  time_t last_access_;
  attr_list attributes_;
  Context *root_;
  list<Entity*> entity_list_;
  list<Context*> context_list_;

 public:
  
  Domain (const char* name, const char* type, const int version, const char* app);
  virtual ~Domain();

  Context* get_root_context()
    { return root_; };

  ContextBindable* resolve_or_create_name (const char *name,
                                           binding_type name_type,
                                           Context *initial_context = 0,
                                           bool create_placeholder = false);


  ContextBindable* resolve_name (const char *name, Context* initial_context = 0);

  Context* resolve_or_create_context (const char *name, 
                                      Context *initial_context = 0,
                                      bool create_placeholder = false);
  Entity* resolve_or_create_entity (const char *name, 
                                    Context* initial_context = 0,
                                    bool create_placeholder = false);

  Context* resolve_context (const char *name, Context *initial_context = 0);
  Entity* resolve_entity (const char *name, Context *inital_context = 0);

  Context* get_context_of_name (const char* name,
                                char*& last_component,
                                Context *initial_context = 0,
                                const unsigned long flags = 0);

  int remove_context (Context* c);
  

  int remove_entity (Entity* dead_guy);
  int remove_binding (const char* name, pds_context_id_t initial_context);
  int remove_binding (const char* name, Context* );
  int remove_binding (const char* name);

  int bind_context(Context* source, 
                   const char* full_name);

  int bind_entity (const char* source_name, 
                   pds_entity_id_t source_entity,
                   pds_context_id_t init_context,
                   const char* new_name);

  void send_wrapped_event(BaseEventWrapper* ev);
  

  void add_entity_to_list (Entity* new_guy);
  void remove_entity_from_list (Entity* dead_guy);
  void add_context_to_list (Context* new_guy);
  void remove_context_from_list (Context* dead_guy);


 private:

  Domain();

};

/*
 * 
 *  $Log: not supported by cvs2svn $
 *  Revision 1.8.2.1  2007/01/05 22:40:34  pmw
 *  interim commit for evpath changes, new branch
 *
 *  Revision 1.8  2005/02/22 19:25:32  pmw
 *  modifications to keep Entity's destructor from hamstringing Domain's
 *  destructor.  the entity list iterator in ~Domain() was being whacked
 *  by ~Entity().  If this functionality is needed, it needs to be
 *  rethought.
 *
 *  Revision 1.7  2004/11/03 06:56:50  mansour
 *  Added APIs for context binding. The XML dump routine expands bounded contexts into a full tree and the load routine loads it as separate contexts.
 *
 *  Revision 1.6  2003/10/02 16:27:46  mansour
 *  NULL was not defined and broke the compile on irix-n32 platforms
 *  Removed DOS EOL chars
 *
 *  Revision 1.5  2003/09/29 00:57:22  mansour
 *  Added a checkpointing thread.
 *  Fix the code for XML dump and load
 *  Fix the code for get_full_name
 *
 *  Revision 1.4  2003/01/30 19:16:26  mansour
 *  Add support for domain events
 *
 *  Revision 1.3  2002/08/13 20:25:51  pmw
 *  Fixed craziness about including the hash_map header.  it's different
 *  between GCC 2.x and 3.0 and again between 3.0 and 3.1.  Introduced
 *  get_hash_map.h header to try and isolate the pain...
 *
 *  Revision 1.2  2002/08/05 22:49:25  pmw
 *  Changes to support gcc 3.0 and xerces-c 2.0
 *
 *  Revision 1.1  2001/10/09 21:37:55  pmw
 *  configuration changes
 *
 *  Revision 1.4  2001/10/07 22:19:15  pmw
 *  Added domain_change event structures so that Proactive class is always correctly
 *  initialized.
 *
 *  Revision 1.3  2001/09/03 20:09:10  pmw
 *  changes to deal with string-marshaled attr lists
 *
 *  Revision 1.2  2001/07/02 04:02:03  pmw
 *  interim checkin for changes to make Contexts also support events.  compiles
 *  but tests probably do not run.
 *
 *  Revision 1.1.1.1  2000/12/13 22:14:23  pmw
 *  imported sources
 *
 *  Revision 1.4  2000/11/28 22:49:26  pmw
 *  1) moved CManager stuff to Proactive class, since other classes
 *  don't need to deal with it any longer
 *  2) moved object ID management out of Domain and into objectId.
 *
 *  Revision 1.3  2000/11/22 21:42:34  pmw
 *  proactivity based on entity events now appears to work correctly.  In particular,
 *  creation events based on the prior registration of placeholder entities and their
 *  later "creation" as real entities works.
 *
 *  Revision 1.2  2000/11/21 21:35:23  pmw
 *  Domain change events now work correctly.  moved event transmission
 *  responsibilities to Proactive class.
 *
 *  Revision 1.1  2000/11/21 00:27:56  pmw
 *  New header files, should have been in last commit.
 *
 *
 *  Revision 1.10  2000/11/20 04:18:25  pmw
 *  Deleted objects now removed from global external reference map
 *
 *  Revision 1.9  2000/10/30 22:44:16  pmw
 *  basic test program works after lots of changes.
 *  ** server still leaks memory; entries are not removed
 *  from global_object_map when contexts or entities are
 *  destroyed.
 *
 *  Revision 1.8  2000/10/25 21:44:48  pmw
 *  Compiles after major revisions to support multiple WP services on
 *  client-side.  This should eventually allow a client to use services by
 *  linking in the library directly or by communicating using CMRPC.
 *
 *  Revision 1.7  2000/09/19 21:12:12  pmw
 *  revised for new cmrpc handler interface
 *
 *  Revision 1.6  2000/04/10 20:00:12  pmw
 *  completed port to CM from DataExchange
 *
 *  Revision 1.5  2000/01/06 04:59:06  pmw
 *  changed id scheme so java side is simpler
 *
 *  Revision 1.4  2000/01/05 17:14:33  pmw
 *  server passes basic tests
 *
 *  Revision 1.3  1999/12/21 18:47:55  pmw
 *  Too many changes - really shouldn't have done this many
 *
 *  Revision 1.2  1999/12/09 22:35:14  pmw
 *  added client_priv header, RCS comments
 *
 *
 */

#endif
