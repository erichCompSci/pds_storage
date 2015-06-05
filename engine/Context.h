#ifndef _CONTEXT_H_
#define _CONTEXT_H_

/*
 *
 *  $Id: Context.h,v 1.8 2007-12-04 21:06:18 pmw Exp $
 *
 */
#include <cstdlib>
#include <cstring>

#include "get_hash_map.h"

#include "ContextBindable.h"

  
class Cstring_eq
{ 
 public:

  bool operator() (const char* p, const char* q) const
    {
      return (strcmp (p,q) == 0);
    }
};

class Domain;  

typedef pair<binding_type, ContextBindable*> binding_pair;

typedef hash_map<const char *, binding_pair, hash<const char *>, Cstring_eq> bindingMap;

class Context : public ContextBindable
{

 public:

  /*Context (const char* my_name, Domain*);*/
  Context (const char* my_name, Domain*, Context* parent);
  Context (const char* my_name, Domain*, Context* parent, const char* name_to_bind);
  virtual ~Context();

  int bind (const char*, const binding_pair&, const char* fullname = 0);

  binding_pair lookup (const char*) const;
  binding_pair lookup (const ContextBindable*) const;

  binding_pair unbind (const char*);
  binding_pair unbind (const binding_pair&, const bool notify = true);
  binding_pair unbind (const ContextBindable*, const bool notify = true);

  virtual void make_real();

  static binding_pair null_binding_pair;

  int level_count (binding_type nt);
  int entity_count() { return level_count (entity_binding); };
  int context_count() { return level_count (context_binding); };

  const bindingMap& get_name_map() const
    {
      return name_map_;
    };

  virtual void send_creation_event();
  virtual void send_deletion_event();
  virtual void send_bind_event (const char* name);
  virtual void send_unbind_event (const char* name);

  virtual void kill_bindings();

 protected:

  bindingMap name_map_;
  
  
};



/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.7.2.2  2007/01/09 01:37:29  pmw
 *  various changes including a fix for the delete context server hang
 *
 *  Revision 1.7.2.1  2007/01/05 22:40:33  pmw
 *  interim commit for evpath changes, new branch
 *
 *  Revision 1.7  2003/10/02 16:27:45  mansour
 *  NULL was not defined and broke the compile on irix-n32 platforms
 *  Removed DOS EOL chars
 *
 *  Revision 1.6  2003/09/29 00:57:22  mansour
 *  Added a checkpointing thread.
 *  Fix the code for XML dump and load
 *  Fix the code for get_full_name
 *
 *  Revision 1.5  2003/01/29 03:36:39  mansour
 *  A bunch of changes to make get_full_name work. Add a my_name_ at the ContextBindable level. This really helps for debugging only and can be removed. Fixed the algorithm for get_full_name.
 *
 *  Also, added calls to generate events on context_creation and deletion.
 *
 *  Revision 1.4  2002/08/13 20:25:51  pmw
 *  Fixed craziness about including the hash_map header.  it's different
 *  between GCC 2.x and 3.0 and again between 3.0 and 3.1.  Introduced
 *  get_hash_map.h header to try and isolate the pain...
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
 *  Revision 1.2  2000/11/27 23:15:50  pmw
 *  Fixed problem where bindings weren't being promoted from placeholders
 *  in the Context binding_map.  Context now supports a promote_binding call,
 *  which calls make_real on whatever the binding was bound to.  For entities,
 *  this does the right thing (sets up the channel, etc)
 *
 *  Revision 1.1  2000/11/21 00:27:56  pmw
 *  New header files, should have been in last commit.
 *
 *
 */
#endif
