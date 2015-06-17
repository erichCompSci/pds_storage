#ifndef _CONTEXT_BINDABLE_H_
#define _CONTEXT_BINDABLE_H_

/*
 *
 *  $Id: ContextBindable.h,v 1.8 2007-12-24 14:40:26 eisen Exp $
 *
 */
extern "C"
{
#include "atl.h"
}

#include <list>
#include <vector>
#include <string>

#include "Proactive.h"
#include "objectId.h"

typedef enum { no_binding, context_binding, entity_binding } binding_type;

class BaseEventWrapper;
class Context;
class Domain;

class ContextBindable : public objectId, public Proactive
{
 public:
  
  virtual ~ContextBindable();

  virtual void unbound_from (Context *);
  virtual void add_referrer (Context *);
  virtual void remove_referrer (Context *);
  virtual unsigned int referrer_count() const;

  virtual void make_real();
  virtual bool is_placeholder();

  Context *parent_;
  list<Context*> referrers_;

  virtual void send_creation_event() = 0;
  virtual void send_deletion_event() = 0;
  virtual void send_event (BaseEventWrapper* ev, unsigned short which_event);
  
  attr_list attributes_;
  virtual const char* get_name() { return my_name_; }
  virtual string get_full_name ();
 protected:
  char * my_name_;
  Domain *domain_;
  bool is_placeholder_;

  ContextBindable (const char* my_name, Context*, Domain*, handler_tag_FMStructDescRec * format_list);
  ContextBindable (const char

  virtual size_t get_name_list (vector<string>& name_vec);
  

};

#endif
