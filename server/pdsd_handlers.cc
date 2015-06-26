/*
 *
 *  $Id: pdsd_handlers.cc,v 1.35 2008-06-26 19:24:19 pmw Exp $
 *
 */

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <unistd.h>
#include <signal.h>

#include "ffs.h"
#include "atl.h"
#include "evpath.h"
#include "cmrpc.h"

};

extern "C" 
{
#include "common/formats.h"
#include "engine/utils.h"
}
#include "engine/objectId.h"
#include "engine/Proactive.h"
#include "engine/Context.h"
#include "engine/Entity.h"
#include "engine/Domain.h"
#include "engine/EngineGuard.h"

extern int bulk_load_from_URL (const char* url);
extern void shutdown_server();
extern void xpath_query(const char* xpath, pds_domain_id_t domain_id, xpath_query_results_msg_ptr output);

extern CManager server_cm;

/*
 *
 *  Beginning of handler functions
 *
 */

static void
handle_shutdown_server (void* in_msg, void* out_msg, CMrpc_options opt)
{
	WriterGuard guard;
//  shutdown_server_msg_ptr msg = static_cast<shutdown_server_msg_ptr> (in_msg);
  return_status_msg_ptr return_msg = static_cast<return_status_msg_ptr> (out_msg);

  pdsTrace_out (pdsdVerbose, "pdsd handle_shutdown_server enter");

  for (vector<Domain*>::iterator i = Domain::global_domain_list_.begin();
       i != Domain::global_domain_list_.end();
       i++)
    {
      Domain *d = *i;
      delete d;
    }
  Domain::global_domain_list_.clear();

  return_msg->status = 1;

  pdsTrace_out (pdsdVerbose, "pdsd handle_shutdown_server exit");
  return;
}

static void
handle_shutdown_server_cleanup( CManager cm, void* msg )
{
  shutdown_server();
}

static void
handle_create_domain (void* in_msg, void* out_msg, CMrpc_options opt)
{
  WriterGuard guard;
  create_domain_msg_ptr msg = static_cast<create_domain_msg_ptr> (in_msg);
  return_domain_id_msg_ptr return_msg = static_cast<return_domain_id_msg_ptr> (out_msg);

  Domain *new_domain;

  pdsTrace_out (pdsdVerbose, "pdsd handle_create_domain enter");

  new_domain = new Domain (msg->domain_name,
                           msg->domain_type,
                           msg->domain_version,
                           msg->application_name);
  pdsTrace_out (pdsdVerbose, "created new domain %p", new_domain);
  /*
   * Return to the caller
   */
  return_msg->domain_id = objectId::make_pds_domain_id (new_domain);
  ID_TRACE_OUT(pdsdVerbose, "domain id is ", return_msg->domain_id.id);
  pdsTrace_out (pdsdVerbose, "pdsd handle_create_domain exit");

  return;
}


static void
handle_open_domain (void* in_msg, void* out_msg, CMrpc_options opt)
{
  ReaderGuard guard;
  create_domain_msg_ptr msg = static_cast<create_domain_msg_ptr> (in_msg);
  return_domain_id_msg_ptr return_msg = static_cast<return_domain_id_msg_ptr> (out_msg);

  pdsTrace_out (pdsdVerbose, "pdsd handle_open_domain enter");

  Domain* d = Domain::find_first_matching_domain (msg->domain_name,
						  msg->domain_type,
						  msg->domain_version,
						  msg->application_name);

  if (d == NULL)
    {
      pdsTrace_out (pdsdVerbose, "Creating domain: %s", msg->domain_name);
      d = new Domain (msg->domain_name,
		      msg->domain_type,
		      msg->domain_version,
		      msg->application_name);
    }

  return_msg->domain_id = objectId::make_pds_domain_id (d);
  ID_TRACE_OUT(pdsdVerbose, "domain id is ", return_msg->domain_id.id);
  pdsTrace_out (pdsdVerbose, "pdsd handle_open_domain exit");

  return;
}


static void
handle_remove_domain (void* in_msg, void* out_msg, CMrpc_options opt)
{
  WriterGuard guard;
  remove_domain_msg_ptr msg = static_cast<remove_domain_msg_ptr> (in_msg);
  return_status_msg_ptr return_msg = static_cast<return_status_msg_ptr> (out_msg);

  pdsTrace_out (pdsdVerbose, "pdsd handle_remove_domain enter");

  Domain* target = objectId::get_domain_ptr_from_id (msg->domain_id);
  ID_TRACE_OUT (pdsdVerbose, "domain id is", msg->domain_id.id);

  if (target == NULL) {
    return_msg->status = 0;
  } else {
    return_msg->status = 1;
    for (vector<Domain*>::iterator j = Domain::global_domain_list_.begin();
         j != Domain::global_domain_list_.end();
         j++) {
      if (*j == target) {
        Domain::global_domain_list_.erase (j);
        pdsTrace_out (pdsdVerbose, "removing domain %p from global list", target);
        break;
      }
    }
    pdsTrace_out (pdsdVerbose, "delete domain %p", target);
    delete target;
  }

  pdsTrace_out (pdsdVerbose, "pdsd handle_remove_domain exit");
  return;
}
 

static void
handle_create_context (void* in_msg, void* out_msg, CMrpc_options opt)
{
  WriterGuard guard;
  Context *new_context;
  create_context_msg_ptr msg = static_cast<create_context_msg_ptr> (in_msg);
  return_context_id_msg_ptr return_msg = static_cast<return_context_id_msg_ptr> (out_msg);
  pdsTrace_out (pdsdVerbose, "pdsd handle_create_context enter");
  ID_TRACE_OUT (pdsdVerbose, "domain id is ", msg->domain_id.id);
  ID_TRACE_OUT (pdsdVerbose, "context id is ", msg->context_id.id);

  Domain *target_domain = objectId::get_domain_ptr_from_id (msg->domain_id);
  pdsTrace_out (pdsdVerbose, "extracted domain %p", target_domain);

  Context *parent = objectId::get_context_ptr_from_id (msg->context_id);
  pdsTrace_out (pdsdVerbose, "extracted parent context %p", parent);
  new_context = target_domain->resolve_or_create_context (msg->c_name, parent);
  pdsTrace_out (pdsdVerbose, "new/resolved context %p", new_context);
  return_msg->context_id = objectId::make_pds_context_id (target_domain, new_context);
  ID_TRACE_OUT (pdsdVerbose, "new context id is ", return_msg->context_id.id);

  pdsTrace_out (pdsdVerbose, "pdsd handle_create_context exit");
  return;
}

static void
handle_bind_context(void* in_msg, void* out_msg, CMrpc_options opt)
{
  WriterGuard guard;
  Context *new_context;

  bind_context_msg_ptr msg = static_cast<bind_context_msg_ptr> (in_msg);
  pdsTrace_out (pdsdVerbose, "pdsd handle_bind_context enter");
  ID_TRACE_OUT (pdsdVerbose, "domain id is ", msg->domain_id.id);
  ID_TRACE_OUT (pdsdVerbose, "context id is ", msg->source_context_id.id);

  Domain *domain = objectId::get_domain_ptr_from_id (msg->domain_id);
  Context *source = 0;
  if (msg->source_context_name != 0) {
      source = domain->resolve_context (msg->source_context_name, domain->get_root_context());
  } else {
    source = objectId::get_context_ptr_from_id (msg->source_context_id);
  }

  if(source == NULL)
  {
    fprintf(stderr, "Error: in handle_bind_context...source is NULL when it shouldn't be\n");
    exit(1);
  }

  const char* link_name = msg->link_name;
  printf("bind context link_name: %s\n", link_name);
 
  return_status_msg_ptr return_msg = static_cast<return_status_msg_ptr> (out_msg);
  return_msg->status = domain->bind_context(source, link_name);
  pdsTrace_out (pdsdVerbose, "pdsd handle_bind_context exit");
  return;
}

static void
handle_unbind_context_by_id (void* in_msg, void* out_msg, CMrpc_options opt)
{
  WriterGuard guard;
  Context *new_context;

  unbind_context_msg_ptr msg = static_cast<unbind_context_msg_ptr> (in_msg);
  pdsTrace_out (pdsdVerbose, "pdsd handle_unbind_context_by_id enter");
  ID_TRACE_OUT (pdsdVerbose, "domain id is ", msg->domain_id.id);

  Domain *domain = objectId::get_domain_ptr_from_id (msg->domain_id);
  const char* link_name = msg->name;
 
  return_status_msg_ptr return_msg = static_cast<return_status_msg_ptr> (out_msg);
  return_msg->status = domain->remove_binding (link_name);  
  pdsTrace_out (pdsdVerbose, "pdsd handle_unbind_context_by_id exit");
  return;
}

static void
handle_remove_context (void* in_msg, void* out_msg, CMrpc_options opt)
{
  WriterGuard guard;
  remove_context_msg_ptr msg = static_cast<remove_context_msg_ptr> (in_msg);
  return_status_msg_ptr return_msg = static_cast<return_status_msg_ptr> (out_msg);
  Context *target_context = 0;
  Domain *target_domain = 0;

  pdsTrace_out (pdsdVerbose, "pdsd handle_remove_context enter");

  target_domain = objectId::get_domain_ptr_from_id (msg->domain_id);
  if (msg->c_name) {
      target_context = target_domain->resolve_context (msg->c_name, objectId::get_context_ptr_from_id (msg->context_id));
  }
  else {
      target_context = objectId::get_context_ptr_from_id (msg->context_id);
  }
  pdsTrace_out (pdsdVerbose, "removing context %p from domain %p", target_context, target_domain);
  target_domain->remove_context (target_context);

  pdsTrace_out (pdsdVerbose, "pdsd handle_remove_context exit");
  return_msg->status = 1;
  
  return;
}

static void
handle_create_entity (void* in_msg, void* out_msg, CMrpc_options opt, unsigned short which_data)
{
  pdsTrace_out (pdsdVerbose, "pdsd handle_create_entity enter");
  WriterGuard guard;
  create_entity_char_msg_ptr msg_char = NULL;
  create_entity_int_msg_ptr msg_int = NULL;
  create_entity_float_msg_ptr msg_float = NULL;
  Domain *target_domain;
  Entity *new_entity;
  return_entity_id_msg_ptr return_msg = static_cast<return_entity_id_msg_ptr> (out_msg);

  if(which_data == ENTITY_DATA_CHANGE_CHAR)
  {
    pdsTrace_out (pdsdVerbose, "pdsd entity data is of type CHAR");
    msg_char = static_cast<create_entity_char_msg_ptr> (in_msg);

    target_domain = objectId::get_domain_ptr_from_id (msg_char->domain_id);
    Context *initial_context = objectId::get_context_ptr_from_id (msg_char->context_id);
  
    pdsTrace_out (pdsdVerbose, "domain %p, initial context %p", target_domain, initial_context);
    new_entity = target_domain->resolve_or_create_entity (const_cast<char*> (msg_char->name),
                                                          initial_context);
  
    pdsTrace_out (pdsdVerbose, "created new entity %p", new_entity);

    attr_list temp_list = attr_list_from_string (msg_char->encoded_attr_list);
    attr_merge_lists (new_entity->attributes_, temp_list);
    free_attr_list (temp_list);

    new_entity->set_data (msg_char->edata.data, msg_char->edata.data_size);
    pdsTrace_out (pdsdVerbose, "submitting char data event");
    new_entity->send_char_data_event();

    return_msg->entity_id = objectId::make_pds_entity_id (target_domain, new_entity);
    return_msg->fullname = NULL;
    return_msg->options = 0;
  
    pdsTrace_out (pdsdVerbose, "pdsd handle_create_entity exit");
    return;
  }
  else if(which_data == ENTITY_DATA_CHANGE_INT)
  {
    pdsTrace_out (pdsdVerbose, "pdsd entity data is of type INT");
    msg_int = static_cast<create_entity_int_msg_ptr> (in_msg);

    target_domain = objectId::get_domain_ptr_from_id (msg_int->domain_id);
    Context *initial_context = objectId::get_context_ptr_from_id (msg_int->context_id);
  
    pdsTrace_out (pdsdVerbose, "domain %p, initial context %p", target_domain, initial_context);
    new_entity = target_domain->resolve_or_create_entity (const_cast<char*> (msg_int->name),
                                                          initial_context);
  
    pdsTrace_out (pdsdVerbose, "created new entity %p", new_entity);

    attr_list temp_list = attr_list_from_string (msg_int->encoded_attr_list);
    attr_merge_lists (new_entity->attributes_, temp_list);
    free_attr_list (temp_list);

    int temp = *(msg_int->edata.data);
    printf("The value of the incoming data is: %d\n", temp);
    new_entity->set_data(msg_int->edata.data, msg_int->edata.data_size);
    pdsTrace_out (pdsdVerbose, "submitting int data event");
    new_entity->send_int_data_event();

    return_msg->entity_id = objectId::make_pds_entity_id (target_domain, new_entity);
    return_msg->fullname = NULL;
    return_msg->options = 0;
  
    pdsTrace_out (pdsdVerbose, "pdsd handle_create_entity exit");
    return;
  }
  else if(which_data == ENTITY_DATA_CHANGE_FLOAT)
  {
    pdsTrace_out (pdsdVerbose, "pdsd entity data is of type FLOAT");
    msg_float = static_cast<create_entity_float_msg_ptr> (in_msg);

    target_domain = objectId::get_domain_ptr_from_id (msg_float->domain_id);
    Context *initial_context = objectId::get_context_ptr_from_id (msg_float->context_id);
  
    pdsTrace_out (pdsdVerbose, "domain %p, initial context %p", target_domain, initial_context);
    new_entity = target_domain->resolve_or_create_entity (const_cast<char*> (msg_float->name),
                                                          initial_context);
  
    pdsTrace_out (pdsdVerbose, "created new entity %p", new_entity);

    attr_list temp_list = attr_list_from_string (msg_float->encoded_attr_list);
    attr_merge_lists (new_entity->attributes_, temp_list);
    free_attr_list (temp_list);

    new_entity->set_data(msg_float->edata.data, msg_float->edata.data_size);
    pdsTrace_out (pdsdVerbose, "submitting float data event");
    new_entity->send_float_data_event();

    return_msg->entity_id = objectId::make_pds_entity_id (target_domain, new_entity);
    return_msg->fullname = NULL;
    return_msg->options = 0;
  
    pdsTrace_out (pdsdVerbose, "pdsd handle_create_entity exit");
    return;
  }
  else
  {
    fprintf(stderr, "Error: the create entity was not one of the three types, should not be possible\n");
    return_msg->options = -1;
    return_msg->fullname = NULL;
    //    return_msg->entity_id = { { '\0' } };
    make_entity_id_null (&(return_msg->entity_id));  
    return;
  }
    /* 
     *  The message contains a data package.  Copy the data
     *  to the entity.
     */
  /*pdsTrace_out (pdsDataVerbose, "new entity: data size = %d, data type = %d",
		msg->edata.data_size, msg->edata.data_type);
  
  if (pdsTrace_on (pdsDataVerbose)) {
    char *c;
    switch (msg->edata.data_type) {
    case Attr_String:
      c = (char*)malloc (msg->edata.data_size + 1);
      memset (c, 0, msg->edata.data_size + 1);
      strncpy (c, (char*)msg->edata.data, msg->edata.data_size);
      pdsTrace_out (pdsDataVerbose, "data = [%s]", c);
      free (c);
      break;
    case Attr_Int4:
      pdsTrace_out (pdsDataVerbose, "data = [%d]", *((int*)msg->edata.data));
      break;
    default:
      pdsTrace_out (pdsDataVerbose, "data dump not supported yet for this type");
      break;
    }
  }*/ //FIXME:Commented out code, need to fix it for new architecture later
  
  return;
}

static void
handle_remove_entity (void* in_msg, void* out_msg, CMrpc_options opt)
{
  WriterGuard guard;
  remove_entity_msg_ptr msg = static_cast<remove_entity_msg_ptr> (in_msg);
  return_status_msg_ptr return_msg = static_cast<return_status_msg_ptr> (out_msg);
  pdsTrace_out (pdsdVerbose, "pdsd handle_remove_entity enter");

  Domain *target_domain = objectId::get_domain_ptr_from_id (msg->domain_id);

  if (!(pds_is_entity_id_null (&(msg->entity_id))))
    {
      Entity *e = objectId::get_entity_ptr_from_id (msg->entity_id);
      pdsTrace_out (pdsdVerbose, "removing entity %p from domain %p", e, target_domain);
      return_msg->status = target_domain->remove_entity (e);
    }
  else
    {
      Context *c = objectId::get_context_ptr_from_id (msg->context_id);
      pdsTrace_out (pdsdVerbose, "removing binding (%s) from context %p, domain %p", 
                    msg->entity_name, c, target_domain);
      return_msg->status = target_domain->remove_binding (msg->entity_name, msg->context_id);
    }
  
  pdsTrace_out (pdsdVerbose, "pdsd handle_remove_entity exit");
  return;
}

static void
handle_add_aggregate_entity(void * in_msg, void * out_msg)
{
  WriterGuard guard;
  add_bucket_entity_msg_ptr msg = static_cast <add_bucket_entity_msg_ptr> (in_msg);
  return_status_msg_ptr return_msg = static_cast <return_status_msg_ptr> (out_msg);
  
  pdsTrace_out (pdsdVerbose, "pdsd handle_add_aggregate_entity entered");
  
  if (!(pds_is_entity_id_null (&(msg->entity_id))))
  {
    Entity * e = objectId::get_entity_ptr_from_id (msg->entity_id);
    int the_type = msg->entity_agg_type;
    pdsTrace_out (pdsdVerbose, "Setting entity: %s to aggregate type %d", e->get_name(), the_type);
    return_msg->status = e->set_aggregate_stone(msg->cod_function, the_type);
  }
  else
  {
    fprintf(stderr, "Error: pdsd server does not yet handle name lookups for aggregation\n");
    return_msg->status = 0;
  }
  
  return;
}


static void
handle_bind_entity (void* in_msg, void* out_msg, CMrpc_options opt)
{
  WriterGuard guard;
  bind_entity_msg_ptr msg = static_cast <bind_entity_msg_ptr> (in_msg);
  return_status_msg_ptr return_msg = static_cast<return_status_msg_ptr> (out_msg);

  pdsTrace_out (pdsdVerbose, "pdsd handle_bind_entity enter"); 
  Domain *target_domain = objectId::get_domain_ptr_from_id (msg->domain_id);

  pdsTrace_out (pdsdVerbose, "binding entity %p to (%s) in context %p, domain %p", 
                objectId::get_entity_ptr_from_id(msg->entity_id), 
                msg->name,
                objectId::get_context_ptr_from_id (msg->context_id),
                target_domain);

  return_msg->status = target_domain->bind_entity (msg->source_name, msg->entity_id, 
                                                   msg->context_id, msg->name);
  pdsTrace_out (pdsdVerbose, "pdsd handle_bind_entity exit");
  return;
}

static void
handle_unbind (void* in_msg, void* out_msg, CMrpc_options opt)
{
  WriterGuard guard;
  unbind_msg_ptr msg = static_cast<unbind_msg_ptr> (in_msg);
  return_status_msg_ptr return_msg = static_cast<return_status_msg_ptr> (out_msg);
  pdsTrace_out (pdsdVerbose, "pdsd handle_unbind enter");
  Domain *target_domain = objectId::get_domain_ptr_from_id (msg->domain_id);

  pdsTrace_out (pdsdVerbose, "unbinding (%s) from context %p, domain %p", 
                msg->name,
                objectId::get_context_ptr_from_id (msg->context_id),
                target_domain);

  return_msg->status = target_domain->remove_binding (msg->name, msg->context_id);
  pdsTrace_out (pdsdVerbose, "pdsd handle_unbind exit");
  return;
}


static void
handle_resolve (void* in_msg, void* out_msg, CMrpc_options opt)
{
  WriterGuard guard;
  resolve_name_msg_ptr msg = static_cast<resolve_name_msg_ptr> (in_msg);
  resolution_result_msg_ptr return_msg = static_cast<resolution_result_msg_ptr> (out_msg);

  Context *initial_context;
  Context *target_context;
  Domain *target_domain;
  Entity *target_entity;

  pdsTrace_out (pdsdVerbose, "pdsd handle_resolve enter");

  return_msg->status = -1;
  make_context_id_null (&(return_msg->context_id));
  make_entity_id_null (&(return_msg->entity_id));  

  target_domain = objectId::get_domain_ptr_from_id (msg->domain_id);

  initial_context = objectId::get_context_ptr_from_id (msg->context_id);
  pdsTrace_out (pdsdVerbose, "resolving (%s) from context %p, domain %p",
                msg->name, initial_context, target_domain);

  target_entity = target_domain->resolve_entity (msg->name, initial_context);

  if (target_entity != NULL) {
    pdsTrace_out (pdsdVerbose, "found entity %p", target_entity);
    //  we found an entity with the name.
    return_msg->status = 1;
    return_msg->entity_id = objectId::make_pds_entity_id (target_domain, target_entity);
  } else { 
    pdsTrace_out (pdsdVerbose, "no entity, trying for context");
    target_context = target_domain->resolve_context (msg->name, initial_context);
    if (target_context != NULL) {
      pdsTrace_out (pdsdVerbose, "found context %p", target_context);
      // we found a context with the name.
      return_msg->status = 1;
      // need to assign context id
      return_msg->context_id = objectId::make_pds_context_id (target_domain, target_context);
    }
  }
 
  pdsTrace_out (pdsdVerbose, "pdsd handle_resolve exit");
  return;
}

static void
handle_get_root_context (void* in_msg, void* out_msg, CMrpc_options opt)
{
  ReaderGuard guard;
  get_root_context_msg_ptr msg = static_cast<get_root_context_msg_ptr> (in_msg);
  return_context_id_msg_ptr return_msg = static_cast<return_context_id_msg_ptr> (out_msg);

  pdsTrace_out (pdsdVerbose, "pdsd handle_get_root_context enter");

  Domain *target_domain = objectId::get_domain_ptr_from_id (msg->domain_id);
  pdsTrace_out (pdsdVerbose, "returning root context %p from domain %p", 
                target_domain->get_root_context(), target_domain);

  return_msg->context_id = objectId::make_pds_context_id (target_domain, target_domain->get_root_context());
  pdsTrace_out (pdsdVerbose, "pdsd handle_get_root_context exit");
  return;
}

static void
handle_context_attributes_cleanup (CManager cm, void* msg)
{
  WriterGuard guard;
  context_attributes_msg_ptr return_attr_msg = static_cast<context_attributes_msg_ptr> (msg);
  free (return_attr_msg->encoded_attr_list);
}


static void
handle_context_attributes (void* in_msg, void* out_msg, CMrpc_options options)
{
  WriterGuard guard;
  Context *initial_context = NULL;
  Context *target_context = NULL;
  context_attributes_msg_ptr msg = static_cast<context_attributes_msg_ptr> (in_msg);
  context_attributes_msg_ptr return_attr_msg = static_cast<context_attributes_msg_ptr> (out_msg);
  return_status_msg_ptr status_msg = static_cast<return_status_msg_ptr> (status_msg);

  pdsTrace_out (pdsdVerbose, "pdsd handle_context_attributes enter");

  Domain *target_domain = objectId::get_domain_ptr_from_id (msg->domain_id);
  
  initial_context = objectId::get_context_ptr_from_id (msg->context_id);

  pdsTrace_out (pdsdVerbose, "using domain %p, initial context %p, name (%s)", 
                target_domain, initial_context, msg->name);

  if (msg->name == NULL) {
    /* the context ID should be used directly */
    target_context = initial_context;
  } else {
    /*  resolve the name using the context ID as an initial context  */
    if (msg->options & CreateObjIfNotFound)
      target_context = target_domain->resolve_or_create_context (msg->name, initial_context);
    else
      target_context = target_domain->resolve_context (msg->name, initial_context);
  }

  if (msg->operation == OP_GET_ATTRS) {
    /*
     *  Getting attributes
     */
    pdsTrace_out (pdsdVerbose, "getting attributes");
    return_attr_msg->encoded_attr_list = attr_list_to_string (target_context->attributes_);
  } else if (msg->operation == OP_SET_ATTRS) {
    pdsTrace_out (pdsdVerbose, "setting attributes");
    /*
     *  Setting attributes
     */
    attr_list tmp = attr_list_from_string (msg->encoded_attr_list);
    attr_merge_lists (target_context->attributes_, tmp);
    free_attr_list (tmp);
  }

  CMrpc_clear_option(options, CMrpc_call_cleanup_function);
  status_msg->status = 1;
  
  pdsTrace_out (pdsdVerbose, "pdsd handle_context_attributes exit");
  return;
}  

static void
handle_domain_attributes_cleanup (CManager cm, void* msg)
{
  WriterGuard guard;
  domain_attributes_msg_ptr return_attr_msg = static_cast<domain_attributes_msg_ptr> (msg);
  free (return_attr_msg->encoded_attr_list);
}

static void
handle_domain_attributes (void* in_msg, void* out_msg, CMrpc_options options)
{
  WriterGuard guard;
  domain_attributes_msg_ptr msg = static_cast<domain_attributes_msg_ptr> (in_msg);
  domain_attributes_msg_ptr return_attr_msg = static_cast<domain_attributes_msg_ptr> (out_msg);
  return_status_msg_ptr status_msg = static_cast<return_status_msg_ptr> (status_msg);
  pdsTrace_out (pdsdVerbose, "pdsd handle_domain_attributes enter");

  Domain *target_domain = objectId::get_domain_ptr_from_id (msg->domain_id);
  pdsTrace_out (pdsdVerbose, "using domain %p", target_domain);

  if (msg->operation == OP_GET_ATTRS) {
    pdsTrace_out (pdsdVerbose, "getting attrs");
    /*
     *  Getting attributes
     */
    return_attr_msg->encoded_attr_list = attr_list_to_string (target_domain->attributes_);
  } else if (msg->operation == OP_SET_ATTRS) {
    pdsTrace_out (pdsdVerbose, "setting attrs");
    /*
     *  Setting attributes
     */
    attr_list tmp = attr_list_from_string (msg->encoded_attr_list);
    
    attr_merge_lists (target_domain->attributes_, tmp);
    free_attr_list (tmp);
  }

  CMrpc_clear_option(options, CMrpc_call_cleanup_function);
  status_msg->status = 1;
  pdsTrace_out (pdsdVerbose, "pdsd handle_domain_attributes exit");
  return;
}  

  
static void
handle_evpath_msg (void* in_msg, void* out_msg, CMrpc_options opt)
{
  //printf("Received the message on the server side...\n");
  WriterGuard guard;
  evpath_op_msg_ptr msg = static_cast<evpath_op_msg_ptr> (in_msg);
  evpath_op_msg_ptr return_msg = static_cast<evpath_op_msg_ptr> (out_msg);
  Proactive *p;
  Context *c;
  Domain *d = objectId::get_domain_ptr_from_id (msg->domain_id);

  pdsTrace_out (pdsdVerbose, "pdsd handle_echannel_msg enter");
  pdsTrace_out (pdsdVerbose, "using domain %p", d);
  //printf("Before we check to see if the domain is NULL\n");
  if (d == NULL)
    {
      return_msg->stone = 0;
      return_msg->operation = OP_RESULT_ERROR;
      return_msg->name = NULL;
      return;
    }

  c = objectId::get_context_ptr_from_id (msg->context_id);
  pdsTrace_out (pdsdVerbose, "using context %p", c);

  int which_event = msg->options;

  switch (msg->operation)
    {
    case OP_GET_ENTITY_STONE:
      pdsTrace_out (pdsdVerbose, "resolving entity name (%s)", msg->name);
      p = d->resolve_or_create_name (msg->name, entity_binding, c, true);
      /*
       *  should never return NULL - either it finds an entity and returns it,
       *  or creates a placeholder (specifying true prevents 
       *  creation of a real entity at this point) and returns that.
       *  so i feel safe in asserting here - if it's NULL, it's not just a
       *  bug, it's a design fault.
       */
      assert (p != NULL);
      break;
    case OP_GET_DOMAIN_STONE:
      pdsTrace_out (pdsdVerbose, "looking for domain %p channel", d);
      p = d; // Can use domain pointer directly
      break;
    case OP_GET_CONTEXT_STONE:
      pdsTrace_out (pdsdVerbose, "resolving context name (%s)", msg->name);
      p = d->resolve_or_create_name (msg->name, context_binding, c, true);
      break;
    }
  //printf("After the switch statement\n");

  attr_list c_attrs = attr_list_from_string( msg->contact_attrs );

  pdsTrace_out (pdsdVerbose, "Trying to add the correct target...");
  return_msg->stone = p->add_target( c_attrs, msg->stone, which_event );
  pdsTrace_out (pdsdVerbose, "Returned from add_target.");
  free_attr_list( c_attrs );
  return_msg->operation = OP_RESULT_OK;
  return_msg->name = NULL;

  pdsTrace_out (pdsdVerbose, "returning stone ID (%d)", return_msg->stone);
  pdsTrace_out (pdsdVerbose, "pdsd handle_echannel_msg exit");
  return;
}


static void
handle_entity_attributes_cleanup (CManager cm, void* msg)
{
  WriterGuard guard;
  entity_attributes_msg_ptr return_attr_msg = static_cast<entity_attributes_msg_ptr> (msg);
  free (return_attr_msg->encoded_attr_list);
}

static void
handle_entity_attributes (void* in_msg, void* out_msg, CMrpc_options options)
{
  WriterGuard guard;
  entity_attributes_msg_ptr msg = static_cast<entity_attributes_msg_ptr> (in_msg);
  entity_attributes_msg_ptr return_attr_msg = static_cast<entity_attributes_msg_ptr> (out_msg);
  return_status_msg_ptr return_msg = static_cast<return_status_msg_ptr> (out_msg);

  Entity *target_entity = NULL;

  pdsTrace_out (pdsdVerbose, "pdsd handle_entity_attributes begin");
  /*
   *  Find the entity pointer.  One of three possibilities:  we
   *  are supplied it directly, we are given a context ID and a 
   *  relative name, or we are given a full name to resolve.
   */
  if (!(pds_is_entity_id_null (&(msg->entity_id)))) {
    /*
     *  got an entity id.  use it.
     */
    target_entity = objectId::get_entity_ptr_from_id (msg->entity_id);
    pdsTrace_out (pdsdVerbose, "using entity ID, entity %p", target_entity);
  } else {
    Context *c = objectId::get_context_ptr_from_id (msg->context_id);
    Domain *dom = objectId::get_domain_ptr_from_id (msg->domain_id);
    assert (dom != NULL);
    
    if (msg->options & CreateObjIfNotFound) {
      pdsTrace_out (pdsdVerbose, "resolve/create entity using domain %p, context %p, name (%s)",
                    dom, c, msg->entity_name);
      target_entity = dom->resolve_or_create_entity (msg->entity_name, c);
    } else {
      pdsTrace_out (pdsdVerbose, "resolve entity using domain %p, context %p, name (%s)",
                    dom, c, msg->entity_name);
      target_entity = dom->resolve_entity (msg->entity_name, c);
    }
    pdsTrace_out (pdsdVerbose, "entity is %p", target_entity);
  }

  if (msg->operation == OP_GET_ATTRS) {
    pdsTrace_out (pdsdVerbose, "getting attributes");
    return_attr_msg->entity_name = NULL;
    return_attr_msg->options = 0;
    
    if (target_entity == NULL) {
      return_attr_msg->encoded_attr_list = NULL;
      return_attr_msg->status = -1;
    } else {
      /*
       *  Get the attributes from the entity and return them
       */
      return_attr_msg->encoded_attr_list = attr_list_to_string (target_entity->attributes_);
      return_attr_msg->status = 1;
    }
  } else if (msg->operation == OP_SET_ATTRS) {
    pdsTrace_out (pdsdVerbose, "setting attributes");
    /* 
     *  The message contains an attribute list.  Set the
     *  attributes in the entity
     */
    if (target_entity == NULL) {
      return_msg->status = -1;
    } else {
      attr_list tmp = attr_list_from_string (msg->encoded_attr_list);
      attr_merge_lists (target_entity->attributes_, tmp);
      free_attr_list (tmp);
      return_msg->status = 1;
      
      target_entity->send_attr_event();
    }

    CMrpc_clear_option(options, CMrpc_call_cleanup_function);
  }

  pdsTrace_out (pdsdVerbose, "pdsd handle_entity_attributes exit");
  return;
}


template <typename T, typename P>
static void
handle_entity_data (T in_msg, void* out_msg, CMrpc_options opt, unsigned short which_data)
{
  WriterGuard guard;
  T return_data_msg = static_cast<T> (out_msg);
  return_status_msg_ptr return_msg = static_cast<return_status_msg_ptr> (out_msg);
  char *c = NULL;
  Entity *target_entity = NULL;
  Domain *d = NULL;
  int dataFlag;

  pdsTrace_out (pdsdVerbose, "pdsd handle_entity_data enter");
  
  /*
   *  Find the entity pointer.  One of three possibilities:  we
   *  are supplied it directly, we are given a context ID and a 
   *  relative name, or we are given a full name to resolve.
   */
  if (!(pds_is_entity_id_null (&(in_msg->entity_id)))) {
    /*
     *  got an entity id.  use it.
     */
    target_entity = objectId::get_entity_ptr_from_id (in_msg->entity_id);
    pdsTrace_out (pdsdVerbose, "using entity ID, entity %p", target_entity);

  } else {

    Context *c = objectId::get_context_ptr_from_id (in_msg->context_id);
    d = objectId::get_domain_ptr_from_id (in_msg->domain_id);
    assert (d != NULL);
    
    if (in_msg->options & CreateObjIfNotFound) {
      pdsTrace_out (pdsdVerbose, "resolve/create entity using domain %p, context %p, name (%s)",
                    d, c, in_msg->entity_name);
      target_entity = d->resolve_or_create_entity (in_msg->entity_name, c);
    } else {
      pdsTrace_out (pdsdVerbose, "resolve entity using domain %p, context %p, name (%s)",
                    d, c, in_msg->entity_name);
      target_entity = d->resolve_entity (in_msg->entity_name, c);
    }
  }

  if (in_msg->edata.data_size == 0) {
    return_data_msg->entity_name = NULL;
    return_data_msg->options = in_msg->options;
    
    pdsTrace_out (pdsdVerbose, "getting entity data (data_size in in_msg == 0)");
    
    if (target_entity == NULL) {
      pdsTrace_out (pdsdVerbose, "target_entity is NULL");
      return_data_msg->edata.data = NULL;
      return_data_msg->edata.data_size = 0;
      return_data_msg->status = -1;
    } else {
      /*
       *  Get the data from the entity and return it
       */
      pdsTrace_out (pdsdVerbose, "using entity %p", target_entity);
      
      dataFlag = (in_msg->options & GetDataBuffer)
        || (in_msg->options & GetDataSize);

      if(which_data == ENTITY_DATA_CHANGE_CHAR)
      { 
        if (((in_msg->options & GetDataBuffer) || !dataFlag) && target_entity->get_char_data()) {
          return_data_msg->edata.data = (P) target_entity->get_char_data()->data;
        } else {
          return_data_msg->edata.data = NULL;
        }
        
        if (((in_msg->options & GetDataSize) || !dataFlag) && target_entity->get_char_data()) {
          return_data_msg->edata.data_size = target_entity->get_char_data()->data_size;
        } else {
          return_data_msg->edata.data_size = 0;
        }
        
      }
      else if(which_data == ENTITY_DATA_CHANGE_INT)
      {
        if (((in_msg->options & GetDataBuffer) || !dataFlag) && target_entity->get_int_data()) {
          return_data_msg->edata.data = (P) target_entity->get_int_data()->data;
        } else {
          return_data_msg->edata.data = NULL;
        }
        
        if (((in_msg->options & GetDataSize) || !dataFlag) && target_entity->get_int_data()) {
          return_data_msg->edata.data_size = target_entity->get_int_data()->data_size;
        } else {
          return_data_msg->edata.data_size = 0;
        }
        
      }
      else if(which_data == ENTITY_DATA_CHANGE_FLOAT)
      {
        if (((in_msg->options & GetDataBuffer) || !dataFlag) && target_entity->get_float_data()) {
          return_data_msg->edata.data = (P) target_entity->get_float_data()->data;
        } else {
          return_data_msg->edata.data = NULL;
        }
        
        if (((in_msg->options & GetDataSize) || !dataFlag) && target_entity->get_float_data()) {
          return_data_msg->edata.data_size = target_entity->get_float_data()->data_size;
        } else {
          return_data_msg->edata.data_size = 0;
        }
        
      }
    }
  } else {
    pdsTrace_out (pdsdVerbose, "setting entity data for entity %p", target_entity);
    /* 
     *  The message contains a data package.  Copy the data
     *  to the entity.
     */
    if (target_entity == NULL){
      return_msg->status = -1;
    } else {

      // Set the data
      //printf("The incoming data is: %f\n", (*in_msg->edata.data));

      target_entity->set_data (in_msg->edata.data,
                               in_msg->edata.data_size);

      //Send the appropriate event
      pdsTrace_out (pdsdVerbose, "submitting data event");
      if(which_data == ENTITY_DATA_CHANGE_CHAR)
        target_entity->send_char_data_event();
      if(which_data == ENTITY_DATA_CHANGE_INT)
        target_entity->send_int_data_event();
      if(which_data == ENTITY_DATA_CHANGE_FLOAT)
        target_entity->send_float_data_event();
      
      return_msg->status = 1;
    }
  }
  
  pdsTrace_out (pdsdVerbose, "pdsd handle_entity_data exit");
  
  return;
}

static void
handle_match_domains_cleanup (CManager cm, void* msg)
{
  WriterGuard guard;
  matching_domains_msg_ptr rv = static_cast<matching_domains_msg_ptr> (msg);
  free (rv->domain_list);
}

static void
handle_match_domains (void* in_msg, void* out_msg, CMrpc_options opt)
{
  WriterGuard guard;
  create_domain_msg_ptr msg = static_cast<create_domain_msg_ptr> (in_msg);
  matching_domains_msg_ptr rv = static_cast<matching_domains_msg_ptr> (out_msg);

  vector<Domain*> matches;

  pdsTrace_out (pdsdVerbose, "pdsd handle_match_domains enter");  

  Domain::find_matching_domains (msg->domain_name,
                                 msg->domain_type,
                                 msg->domain_version,
                                 msg->application_name,
                                 matches);

  /*
   *  matches now contains all matching domains.  package them up for transport
   *  back to the client
   */
  pdsTrace_out (pdsdVerbose, "found %d matches", matches.size());
  rv->domain_count = matches.size();
  rv->domain_list = (pds_domain_id_t*) malloc (rv->domain_count * sizeof (pds_domain_id_t));
  for (unsigned int j = 0; j < matches.size(); j++) {
    rv->domain_list[j] = objectId::make_pds_domain_id (matches[j]);
  }

  pdsTrace_out (pdsdVerbose, "pdsd handle_match_domains exit");
  return;
}


static void
handle_get_binding_list_cleanup (CManager cm, void* msg)
{
  ReaderGuard guard;
  binding_list_msg_ptr rv = static_cast<binding_list_msg_ptr> (msg);
  free (rv->binding_list);
}

static void
handle_get_binding_list (void* in_msg, void* out_msg, CMrpc_options opt)
{
  ReaderGuard guard;
  Context *target_context;
  resolve_name_msg_ptr msg = static_cast<resolve_name_msg_ptr> (in_msg);
  binding_list_msg_ptr rv = static_cast<binding_list_msg_ptr> (out_msg);
  
  pdsTrace_out (pdsdVerbose, "pdsd handle_get_binding_list enter");

  Domain *target_domain = objectId::get_domain_ptr_from_id (msg->domain_id);  
  Context *initial_context = objectId::get_context_ptr_from_id (msg->context_id);

  /*
   *  if the name is NULL, that means we need to use the initial context
   *  as the target context (called from a _by_id function)
   */
  if (msg->name == NULL) {
    pdsTrace_out (pdsdVerbose, "using domain %p, target_context %p, no name given (NULL)",
                  target_domain, initial_context);
    target_context = initial_context;
  } else {
    pdsTrace_out (pdsdVerbose, "using domain %p, initial_context %p, resolving name (%s)",
                  target_domain, initial_context, msg->name);
    target_context = target_domain->resolve_context (msg->name, initial_context);
  }

  pdsTrace_out (pdsdVerbose, "resolved context %p", target_context);

  if (target_context == NULL) {
    rv->binding_count = 0;
    rv->binding_list = NULL;
    rv->context_name = NULL;
  } else {
    int ecount = target_context->entity_count();
    int ccount = target_context->context_count();
    //      char *tmp = NULL;
    //      Context *parent = NULL;
    
    pdsTrace_out (pdsdVerbose, "found %d bindings (%dC, %dE) in context %p", 
                  ecount + ccount, ccount, ecount, target_context);
    
    rv->binding_count = 0;
    if (msg->options == 0 || msg->options & OnlyEntityBindings)
      rv->binding_count += ecount;
    if (msg->options == 0 || msg->options & OnlyContextBindings)
      rv->binding_count += ccount;
    rv->binding_list = (char**) malloc (rv->binding_count * sizeof (char*));
    
    // fill the binding list
    const bindingMap& bmap = target_context->get_name_map();
    bindingMap::const_iterator bi;
    int i = 0;
    
    for (bi = bmap.begin(); bi != bmap.end(); bi++) {
      if (((*bi).second.second)->is_placeholder()) continue;
      if (msg->options == 0 
          ||
          ((msg->options & OnlyEntityBindings) && (*bi).second.first == entity_binding)
          ||
          ((msg->options & OnlyContextBindings) && (*bi).second.first == context_binding)) {
        rv->binding_list[i] = const_cast<char*> ((*bi).first);
        pdsTrace_out (pdsDataVerbose, "adding binding (%s) to list",
                      rv->binding_list[i]);
        ++i;
      }
    }
      
    //      parent = target_domain->get_context_of_name (msg->name, tmp, initial_context);
    //free (tmp);
    rv->context_name = NULL;
    //
    //  I'm of the opinion now that get_full_name() is a bad idea.  Unless I want 
    // to get rid of the concept of symlinks, which haven't really been explored yet,
    // I can't support it in a good fashion (which may say more about me than it does
    // about the problem).
    //
    //      rv->context_name = strdup ((target_context->get_full_name (parent)).c_str());
    
    //      rv->context_name = target_context->get_full_name();
  }
  
  pdsTrace_out (pdsdVerbose, "pdsd handle_get_binding_list exit");
  return;
}


static void
handle_match_entities_cleanup (CManager cm, void* msg)
{
  WriterGuard guard;
  matching_entities_msg_ptr rv = static_cast<matching_entities_msg_ptr> (msg);
  free (rv->entity_list);
}


/*
static void
handle_match_entities (void* in_msg, void* out_msg, CMrpc_options opt)
{
  WriterGuard guard;
  create_entity_msg_ptr msg = static_cast<create_entity_msg_ptr> (in_msg);
  matching_entities_msg_ptr return_msg = static_cast<matching_entities_msg_ptr> (out_msg);

  vector<const Entity*> matches;
  attr_list al;

  pdsTrace_out (pdsdVerbose, "pdsd handle_match_entities enter");
  Domain *target_domain = objectId::get_domain_ptr_from_id (msg->domain_id);
  Context *target_context = target_domain->resolve_context (msg->name);

  pdsTrace_out (pdsdVerbose, "using domain %p, resolved context %c using name (%s)",
                target_domain, target_context, msg->name);

  al = attr_list_from_string (msg->encoded_attr_list);

  if (target_context != NULL) {
*/  /*
     *  we only have to consider entities that are bound in the given context
     */
/*  const bindingMap& bmap = target_context->get_name_map();
    bindingMap::const_iterator bi;
    
    for (bi = bmap.begin(); bi != bmap.end(); bi++) {
      binding_type bt = ((*bi).second).first;
      if (bt == entity_binding) {
        Entity *e = dynamic_cast<Entity*> ((*bi).second.second);
        assert (e != NULL);
        if (attr_list_subset (al, e->attributes_)) {
          pdsTrace_out (pdsdVerbose, "matched entity %p", e);
          matches.push_back (e);
        }
      }
    }
  } else {
*/  /*
     *  The given context was empty, so we have to consider all entities in the domain
     */
/*  list<Entity*>::iterator ei2;
    
    for (ei2 = target_domain->entity_list_.begin();
         ei2 != target_domain->entity_list_.end();
         ei2++)	{
      if (attr_list_subset (al, (*ei2)->attributes_)) {
        pdsTrace_out (pdsdVerbose, "matched entity %p", *ei2);
        matches.push_back (*ei2);
      }
    }
  }
*/
  /*
   *  We've found all the matches, now pack them up and send them back
   */
/*
  return_msg->entity_count = matches.size();
  return_msg->entity_list = 
    (pds_entity_id_t*) malloc (return_msg->entity_count * sizeof (pds_entity_id_t));
  vector<const Entity*>::iterator mi = matches.begin();
  for (int c = 0; mi != matches.end(); c++, mi++) {
    pds_entity_id_t e_id = objectId::make_pds_entity_id (target_domain, *mi);
    ID_TRACE_OUT (pdsDataVerbose, "adding entity ID ", e_id.id);
    return_msg->entity_list[c] = e_id;
  }

  free_attr_list (al);
  pdsTrace_out (pdsdVerbose, "pdsd handle_match_entities exit");

  return;
}
*/

static void
handle_load_from_URL (void* in_msg, void* out_msg, CMrpc_options opt)
{
  WriterGuard guard;
  load_from_URL_msg_ptr msg = static_cast<load_from_URL_msg_ptr> (in_msg);
  return_status_msg_ptr return_msg = static_cast<return_status_msg_ptr> (out_msg);

  pdsTrace_out (pdsdVerbose, "pdsd handle_load_from_url enter");
  pdsTrace_out (pdsdVerbose, "loading from URL (%s)", msg->url);

  return_msg->status = 0;//bulk_load_from_URL (msg->url);

  pdsTrace_out (pdsdVerbose, "pdsd handle_load_from_url exit");
  return;
}
static void
handle_xpath_query(void* in_msg, void* out_msg, CMrpc_options opt)
{
  ReaderGuard guard;
  xpath_query_results_msg_ptr result = (xpath_query_results_msg_ptr) out_msg;
  xpath_query_request_msg_ptr request = (xpath_query_request_msg_ptr) in_msg;
  result->name_list = 0;
  result->name_count = 0;
  //xpath_query(request->xpath, request->domain_id, result);
}

static void
handle_open_service(void* in_msg, void* out_msg, CMrpc_options opt)
{
  open_service_msg_ptr msg = static_cast<open_service_msg_ptr> (in_msg);
  open_service_msg_ptr return_msg = static_cast<open_service_msg_ptr> (out_msg);

  pdsTrace_out (pdsdVerbose, "pdsd handle_open_service enter");
  pdsTrace_out (pdsdVerbose, "using address (%lx) and port (%d)",
                objectId::global_server_address,
                objectId::global_server_port);

  return_msg->addr = htonl(objectId::global_server_address);
  return_msg->port = htons(objectId::global_server_port);
  return_msg->broker = 1;  // indicates address and port set
  
  pdsTrace_out (pdsdVerbose, "pdsd handle_open_service exit");

  return;
}

extern "C" void
Chandle_shutdown_server(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_shutdown_server(in_msg, out_msg, opt);
}
extern "C" void
Chandle_shutdown_server_cleanup( CManager cm, void* msg )
{
  handle_shutdown_server_cleanup( cm, msg );
}

extern "C" void
Chandle_entity_char_data(void* in_msg, void* out_msg, CMrpc_options opt)
{
  entity_char_data_msg_ptr msg = static_cast<entity_char_data_msg_ptr> (in_msg);
  handle_entity_data<entity_char_data_msg_ptr, unsigned char *>(msg, out_msg, opt, ENTITY_DATA_CHANGE_CHAR);
}

extern "C" void
Chandle_entity_int_data(void* in_msg, void* out_msg, CMrpc_options opt)
{
  entity_int_data_msg_ptr msg = static_cast<entity_int_data_msg_ptr> (in_msg);
  handle_entity_data<entity_int_data_msg_ptr, int *>(msg, out_msg, opt, ENTITY_DATA_CHANGE_INT);
}

extern "C" void
Chandle_entity_float_data(void* in_msg, void* out_msg, CMrpc_options opt)
{
  entity_float_data_msg_ptr msg = static_cast<entity_float_data_msg_ptr> (in_msg);
  handle_entity_data<entity_float_data_msg_ptr, float *>(msg, out_msg, opt, ENTITY_DATA_CHANGE_FLOAT);
}

extern "C" void
Chandle_entity_attributes(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_entity_attributes(in_msg, out_msg, opt);
}

extern "C" void
Chandle_entity_attributes_cleanup(CManager cm, void *msg)
{
  handle_entity_attributes_cleanup(cm, msg);
}

extern "C" void
Chandle_open_domain(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_open_domain(in_msg, out_msg, opt);
}

extern "C" void
Chandle_create_domain(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_create_domain(in_msg, out_msg, opt);
}

extern "C" void
Chandle_remove_domain(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_remove_domain(in_msg, out_msg, opt);
}

extern "C" void
Chandle_match_domains(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_match_domains(in_msg, out_msg, opt);
}

extern "C" void
Chandle_match_domains_cleanup(CManager cm, void *msg)
{
  handle_match_domains_cleanup(cm, msg);
}

extern "C" void
Chandle_create_context(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_create_context(in_msg, out_msg, opt);
}

extern "C" void
Chandle_bind_context(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_bind_context(in_msg, out_msg, opt);
}

extern "C" void
Chandle_unbind_context_by_id(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_unbind_context_by_id(in_msg, out_msg, opt);
}


extern "C" void
Chandle_get_root_context(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_get_root_context(in_msg, out_msg, opt);
}

extern "C" void
Chandle_remove_context(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_remove_context(in_msg, out_msg, opt);
}

/*FIXME: Need to add this functionality back in when the time comes
extern "C" void
Chandle_match_entities(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_match_entities(in_msg, out_msg, opt);
}
*/

extern "C" void
Chandle_create_char_entity(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_create_entity(in_msg, out_msg, opt, ENTITY_DATA_CHANGE_CHAR);
}

extern "C" void
Chandle_create_int_entity(void * in_msg, void * out_msg, CMrpc_options opt)
{
  handle_create_entity(in_msg, out_msg, opt, ENTITY_DATA_CHANGE_INT);
}

extern "C" void
Chandle_create_float_entity(void * in_msg, void * out_msg, CMrpc_options opt)
{
  handle_create_entity(in_msg, out_msg, opt, ENTITY_DATA_CHANGE_FLOAT);
}

extern "C" void
Chandle_remove_entity(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_remove_entity(in_msg, out_msg, opt);
}

extern "C" void
Chandle_bind_entity(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_bind_entity(in_msg, out_msg, opt);
}

extern "C" void
Chandle_unbind(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_unbind(in_msg, out_msg, opt);
}

extern "C" void
Chandle_resolve(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_resolve(in_msg, out_msg, opt);
}

extern "C" void
Chandle_match_entities_cleanup(CManager cm, void *msg)
{
  handle_match_entities_cleanup(cm, msg);
}

extern "C" void
Chandle_domain_attributes(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_domain_attributes(in_msg, out_msg, opt);
}

extern "C" void
Chandle_domain_attributes_cleanup(CManager cm, void *msg)
{
  handle_domain_attributes_cleanup(cm, msg);
}

extern "C" void
Chandle_get_binding_list(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_get_binding_list(in_msg, out_msg, opt);
}

extern "C" void
Chandle_get_binding_list_cleanup(CManager cm, void *msg)
{
  handle_get_binding_list_cleanup(cm, msg);
}

extern "C" void
Chandle_context_attributes_cleanup(CManager cm, void* msg)
{
  handle_context_attributes_cleanup(cm, msg);
}

extern "C" void
Chandle_context_attributes(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_context_attributes(in_msg, out_msg, opt);
}

extern "C" void
Chandle_evpath_msg(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_evpath_msg(in_msg, out_msg, opt);
}

extern "C" void
Chandle_load_from_URL(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_load_from_URL(in_msg, out_msg, opt);
}

extern "C" void
Chandle_xpath_query(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_xpath_query(in_msg, out_msg, opt);
}

extern "C" void
Chandle_open_service(void* in_msg, void* out_msg, CMrpc_options opt)
{
  handle_open_service(in_msg, out_msg, opt);
}

extern "C" void
Chandle_aggregate_service(void* in_msg, void * out_msg, CMrpc_options opt)
{
  handle_add_aggregate_entity(in_msg, out_msg);
}

void
register_handlers (CManager cm)
{
  CMrpc_register_rpc_handler (cm, CREATE_DOMAIN_RPC_NAME, 
			      create_domain_msg_formats, return_domain_id_msg_formats,
			      Chandle_create_domain, NULL, NULL);

  CMrpc_register_rpc_handler (cm, REMOVE_DOMAIN_RPC_NAME, 
			      remove_domain_msg_formats, return_status_msg_formats,
			      Chandle_remove_domain, NULL, NULL);

  CMrpc_register_rpc_handler (cm, FIND_MATCHING_DOMAINS_RPC_NAME, 
			      create_domain_msg_formats, matching_domains_msg_formats,
                               Chandle_match_domains, NULL,
                               Chandle_match_domains_cleanup);

  CMrpc_register_rpc_handler (cm, SEND_DOMAIN_ATTRIBUTES_RPC_NAME, 
			      domain_attributes_msg_formats, return_status_msg_formats,
                               Chandle_domain_attributes, NULL,
                               Chandle_domain_attributes_cleanup);

  CMrpc_register_rpc_handler (cm, CREATE_CONTEXT_RPC_NAME, 
			      create_context_msg_formats, return_context_id_msg_formats,
			      Chandle_create_context, NULL, NULL);

  CMrpc_register_rpc_handler (cm, BIND_CONTEXT_RPC_NAME, 
			      bind_context_msg_formats,
			      return_status_msg_formats,
			      Chandle_bind_context, NULL, NULL);
  
  CMrpc_register_rpc_handler (cm, UNBIND_CONTEXT_BY_ID_RPC_NAME, 
			      unbind_context_msg_formats,
			      return_status_msg_formats,
			      Chandle_unbind_context_by_id, NULL, NULL);

  CMrpc_register_rpc_handler (cm, GET_ROOT_CONTEXT_RPC_NAME,
			      get_root_context_msg_formats,
			      return_context_id_msg_formats,
			      Chandle_get_root_context, NULL, NULL);

  CMrpc_register_rpc_handler (cm, REMOVE_CONTEXT_RPC_NAME, 
			      remove_context_msg_formats, return_status_msg_formats,
			      Chandle_remove_context, NULL, NULL);

  CMrpc_register_rpc_handler (cm, GET_BINDING_LIST_RPC_NAME, 
			      resolve_name_msg_formats, binding_list_msg_formats,
                               Chandle_get_binding_list, NULL,
                               Chandle_get_binding_list_cleanup);

  CMrpc_register_rpc_handler (cm, CREATE_ENTITY_CHAR_RPC_NAME, 
			      create_entity_char_msg_formats, return_entity_id_msg_formats,
			      Chandle_create_char_entity, NULL, NULL);

  CMrpc_register_rpc_handler (cm, CREATE_ENTITY_INT_RPC_NAME,
            create_entity_int_msg_formats, return_entity_id_msg_formats,
            Chandle_create_int_entity, NULL, NULL);

  CMrpc_register_rpc_handler (cm, CREATE_ENTITY_FLOAT_RPC_NAME,
            create_entity_float_msg_formats, return_entity_id_msg_formats,
            Chandle_create_int_entity, NULL, NULL);

  CMrpc_register_rpc_handler (cm, REMOVE_ENTITY_RPC_NAME, 
			      remove_entity_msg_formats,
			      return_status_msg_formats,
			      Chandle_remove_entity, NULL, NULL);

  CMrpc_register_rpc_handler (cm, BIND_ENTITY_RPC_NAME, 
			      bind_entity_msg_formats, return_status_msg_formats,
			      Chandle_bind_entity, NULL, NULL);

  CMrpc_register_rpc_handler (cm, UNBIND_ENTITY_RPC_NAME, 
			      unbind_msg_formats, return_status_msg_formats,
			      Chandle_unbind, NULL, NULL);

  CMrpc_register_rpc_handler (cm, RESOLVE_NAME_RPC_NAME, 
			      resolve_name_msg_formats, resolution_result_msg_formats,
			      Chandle_resolve, NULL, NULL);

  CMrpc_register_rpc_handler (cm, EVPATH_OP_RPC_NAME, 
			      evpath_op_msg_formats,
			      evpath_op_msg_formats,
			      Chandle_evpath_msg, NULL, NULL);

  CMrpc_register_rpc_handler (cm, GET_ENTITY_ATTRIBUTES_RPC_NAME, 
			      entity_attributes_msg_formats, entity_attributes_msg_formats,
			      Chandle_entity_attributes, NULL,
			      Chandle_entity_attributes_cleanup);
  
  CMrpc_register_rpc_handler (cm, SET_ENTITY_ATTRIBUTES_RPC_NAME, 
			      entity_attributes_msg_formats, return_status_msg_formats,
			      Chandle_entity_attributes, NULL,
			      NULL);

  CMrpc_register_rpc_handler (cm, GET_ENTITY_CHAR_DATA_RPC_NAME, 
			      entity_char_data_msg_formats, entity_char_data_msg_formats,
			      Chandle_entity_char_data, NULL, NULL);

  CMrpc_register_rpc_handler (cm, GET_ENTITY_INT_DATA_RPC_NAME, 
			      entity_int_data_msg_formats, entity_int_data_msg_formats,
			      Chandle_entity_int_data, NULL, NULL);

  CMrpc_register_rpc_handler (cm, GET_ENTITY_FLOAT_DATA_RPC_NAME, 
			      entity_float_data_msg_formats, entity_float_data_msg_formats,
			      Chandle_entity_float_data, NULL, NULL);

  CMrpc_register_rpc_handler (cm, SET_ENTITY_CHAR_DATA_RPC_NAME, 
			      entity_char_data_msg_formats, return_status_msg_formats,
			      Chandle_entity_char_data, NULL, NULL);

  CMrpc_register_rpc_handler (cm, SET_ENTITY_INT_DATA_RPC_NAME, 
			      entity_int_data_msg_formats, return_status_msg_formats,
			      Chandle_entity_int_data, NULL, NULL);

  CMrpc_register_rpc_handler (cm, SET_ENTITY_FLOAT_DATA_RPC_NAME, 
			      entity_float_data_msg_formats, return_status_msg_formats,
			      Chandle_entity_float_data, NULL, NULL);

/*  CMrpc_register_rpc_handler (cm, FIND_MATCHING_ENTITIES_RPC_NAME, 
			      create_entity_msg_formats, matching_entities_msg_formats,
			      Chandle_match_entities, NULL,
			      Chandle_match_entities_cleanup);  
*/

  CMrpc_register_rpc_handler (cm, GET_CONTEXT_ATTRIBUTES_RPC_NAME, 
			      context_attributes_msg_formats, context_attributes_msg_formats,
			      Chandle_context_attributes, NULL,
			      Chandle_context_attributes_cleanup);
  
  CMrpc_register_rpc_handler (cm, SET_CONTEXT_ATTRIBUTES_RPC_NAME, 
			      context_attributes_msg_formats, return_status_msg_formats,
			      Chandle_context_attributes, NULL,
			      NULL);

  CMrpc_register_rpc_handler (cm, LOAD_FROM_URL_RPC_NAME,
			      load_from_URL_msg_formats, return_status_msg_formats,
			      Chandle_load_from_URL, NULL, NULL);

  CMrpc_register_rpc_handler (cm, OPEN_DOMAIN_RPC_NAME,
			      create_domain_msg_formats, return_domain_id_msg_formats,
			      Chandle_open_domain, NULL, NULL);
  
  CMrpc_register_rpc_handler (cm, SHUTDOWN_SERVER_RPC_NAME,
			      shutdown_server_msg_formats, return_status_msg_formats,
			      Chandle_shutdown_server, NULL, 
			      Chandle_shutdown_server_cleanup);

  CMrpc_register_rpc_handler (cm, QUERY_BY_XPATH_RPC_NAME,
			      xpath_query_request_msg_formats, xpath_query_request_msg_formats,
			      Chandle_xpath_query, NULL, NULL);
  
  CMrpc_register_rpc_handler (cm, OPEN_SERVICE_RPC_NAME,
			      open_service_msg_formats,
			      open_service_msg_formats,
			      Chandle_open_service, NULL, NULL);
  
  CMrpc_register_rpc_handler (cm, ADD_AGGREGATE_ENTITY_RPC,
            add_bucket_entity_msg_formats, return_status_msg_formats,
            Chandle_aggregate_service, NULL, NULL);

  return;
}


/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.34  2008/01/01 00:38:21  pmw
 *  Changes to make PDS work with the new FFS libraries
 *
 *  Revision 1.32  2007/12/04 21:06:24  pmw
 *  Merge changes from evpath branch.  Also probably includes
 *  Darwin fixes.  Compiles on isleroyale and marquesas, I think
 *  I'm misconfigured somehow for building on gondor.
 *
 *  Revision 1.31.2.2  2007/01/09 01:37:30  pmw
 *  various changes including a fix for the delete context server hang
 *
 *  Revision 1.31.2.1  2007/01/05 22:40:40  pmw
 *  interim commit for evpath changes, new branch
 *
 *  Revision 1.31  2005/02/24 19:28:30  pmw
 *  Everything should now be using string encoding for attr lists.  I
 *  removed all the ifdefs.
 *
 *  Revision 1.30  2005/02/22 07:18:21  pmw
 *  If there's any justice, this has knocked the cross-platform
 *  connection bug.  The open_service exchange actually sends
 *  the address and port with PBIO integer encoding, so in some
 *  cases there's an extra byte-swap happening.  Fixed by performing
 *  byte-swapping manually for just that case BEFORE the addr/port
 *  is stored in pdsagent's list of services.
 *
 *  Revision 1.29  2004/11/06 22:23:35  mansour
 *  Added a bind_context_by_name API. Also fixed the XML logic to use the correct bind name.
 *
 *  Revision 1.28  2004/11/05 22:02:57  mansour
 *  Improve PDS performance by using R/W lock instead of a mutex and also cache the XML document instead of re-creating it on each query.
 *
 *  Revision 1.27  2004/11/03 06:57:55  mansour
 *  Added APIs for context binding. The XML dump routine expands bounded contexts in
 *  to a full tree and the load routine loads it as separate contexts.
 *
 *  Revision 1.26  2004/10/14 21:17:33  pmw
 *  Changes to support the CMrpc renaming.
 *
 *  Revision 1.25  2004/04/27 20:59:25  pmw
 *  Added RPC for open_service to simplify service_from_ID handling on the
 *  client.  The way I set that up at first was a little braindead, and failed
 *  when one side was weakly hostnamed (via DHCP, for example).  The new RPC
 *  asks the server what address/port combination it intends to use and saves
 *  those.
 *
 *  Revision 1.24  2003/12/11 23:50:59  eisen
 *  Try this without the #ifndef __GNUC__
 *
 *  Revision 1.23  2003/11/09 03:57:35  mansour
 *  Add wrapper API for building XPath queries
 *
 *  Revision 1.22  2003/11/03 18:13:40  mansour
 *  register new RPC call
 *
 *  Revision 1.21  2003/10/31 05:40:05  mansour
 *  Added support for xpath queries
 *
 *  Revision 1.20  2003/10/21 16:03:09  eisen
 *  Changes to avoid warnings and errors and generally make this compile with
 *  SUNWspro7 CC with the -library=stlport4 argument.  Using this correctly
 *  requires setting CXX=CC, CXXFLAGS=-library=stlport4,
 *  CXXCPP="CC -E -library=stlport4".  Chaos is setup to do this now.
 *
 *  Revision 1.19  2003/10/05 12:10:34  pmw
 *  Changes to make utils a C module instead of C++, allowing people to
 *  link against its functions more easily
 *
 *  Revision 1.18  2003/04/07 04:25:48  mansour
 *  Added a thread to delete stale contexts.
 *
 *  Revision 1.17  2003/02/12 20:50:07  eisen
 *  Tweaks to kill warnings on Irix compilers.
 *
 *  Revision 1.16  2003/01/29 03:34:54  mansour
 *  no real changes.
 *
 *  Revision 1.15  2003/01/25 13:12:43  mansour
 *  Fix a bug with pds_remove_context.
 *
 *  Revision 1.14  2003/01/24 19:31:28  eisen
 *  Some tweaks to make pds compile with Forte CC (version 7 with the
 *  -library=stlport4 argument).
 *
 *  Revision 1.13  2002/11/16 04:10:22  pmw
 *  added functionality to selectively retrieve entity data/type/size information
 *  using the get_entity_data calls.  New options masks have been added to pdsdefs.h
 *  to make this work.
 *
 *  Revision 1.12  2002/11/05 19:33:27  pmw
 *  I think this fixes a bug in handle_remove_context. not sure until
 *  mohamed lets me know...
 *
 *  Revision 1.11  2002/10/31 23:29:53  pmw
 *  2 fixes:
 *  - single character context names (in particular "/") crashed
 *  - fixed memory stomping bug in get_binding_list when only asking for
 *  entity or context bindings
 *
 *  Revision 1.10  2002/10/30 17:58:59  pmw
 *  Fixed context creation bug when used with null_pds_context_id
 *
 *  Revision 1.9  2002/10/30 05:01:42  pmw
 *  fixed binding count in get_binding_list handler
 *
 *  Revision 1.8  2002/10/30 03:52:59  pmw
 *  added parameter to get_binding_list functions to optionally get only entities
 *  or contexts
 *
 *  Revision 1.7  2002/10/28 20:44:54  pmw
 *  Fixed bug in get_binding_list handler - used wrong context ID if no
 *  context name was given
 *
 *  Revision 1.6  2002/08/07 18:51:46  eisen
 *  Widespread changes to modify how attribute handling works.  Stop using
 *  attr_atom_from_string() to get atom values.  Instead, those are set by the
 *  application.  Stop using get_attr() (api has changes).  Stop using
 *  xmit_object and start using the ATL-supplied encode/decode routines.
 *
 *  Revision 1.5  2002/08/05 22:49:33  pmw
 *  Changes to support gcc 3.0 and xerces-c 2.0
 *
 *  Revision 1.4  2002/04/09 19:13:30  pmw
 *  got rid of get_full_name() nonsense.  now I just use the name the
 *  caller gave me and if it's not fully qualified, well, life is tough, you
 *  oughta know the name since you used it to get a context ID to begin with.
 *
 *  Revision 1.3  2002/03/28 04:47:55  pmw
 *  changes to support different full-naming scheme for entities
 *
 *  Revision 1.2  2001/10/15 19:58:14  pmw
 *  added new trace framework, shamelessly copied from Greg
 *
 *  Revision 1.1  2001/10/09 21:37:56  pmw
 *  configuration changes
 *
 *  Revision 1.11  2001/10/07 22:19:16  pmw
 *  Added domain_change event structures so that Proactive class is always correctly
 *  initialized.
 *
 *  Revision 1.10  2001/09/23 22:55:54  pmw
 *  Contexts now support attributes of their own.  Practically, this is supported
 *  by having ContextBindable host the attribute list instead of Entity/Context.  Also
 *  added the usual functions on the client side and handlers for getting/setting
 *  this attr list for Contexts.
 *
 *  Revision 1.9  2001/09/21 22:02:35  pmw
 *  attributes handled correctly now for entities that already exist during handle_create_entity
 *
 *  Revision 1.8  2001/09/10 03:55:43  pmw
 *  client lib is now libpdsagent, and is compiled entirely with gcc.  libpdsutil no
 *  longer exists.
 *
 *  Revision 1.7  2001/09/03 21:08:14  pmw
 *  fixed typo bug in context channel ID operation
 *
 *  Revision 1.6  2001/09/03 20:09:10  pmw
 *  changes to deal with string-marshaled attr lists
 *
 *  Revision 1.5  2001/08/27 08:05:53  pmw
 *  fixes
 *
 *  Revision 1.4  2001/07/15 02:29:37  pmw
 *  Fixed context creation problem and added some changes for Contexts to return
 *  their binding lists.
 *
 *  Revision 1.3  2001/07/02 15:07:43  pmw
 *  removed ContextBindable class def and impl to their own source files
 *
 *  Revision 1.2  2001/07/02 04:02:03  pmw
 *  interim checkin for changes to make Contexts also support events.  compiles
 *  but tests probably do not run.
 *
 *  Revision 1.1.1.1  2000/12/13 22:14:22  pmw
 *  imported sources
 *
 *  Revision 1.23  2000/12/06 01:57:01  pmw
 *  Fixed problems with memory leakage and poor server shutdown mechanics
 *
 *  Revision 1.22  2000/12/05 00:24:20  pmw
 *  Added server_shutdown message
 *
 *  Revision 1.21  2000/11/29 04:37:36  pmw
 *  changed channel-handling again, Entity now performs most of its own
 *  change event sends
 *
 *  Revision 1.20  2000/11/28 22:49:30  pmw
 *  1) moved CManager stuff to Proactive class, since other classes
 *  don't need to deal with it any longer
 *  2) moved object ID management out of Domain and into objectId.
 *
 *  Revision 1.19  2000/11/27 23:44:42  pmw
 *  Not setting message fields to NULL causes crashes on Linux... nice compiler
 *
 *  Revision 1.18  2000/11/22 21:42:36  pmw
 *  proactivity based on entity events now appears to work correctly.  In particular,
 *  creation events based on the prior registration of placeholder entities and their
 *  later "creation" as real entities works.
 *
 *  Revision 1.17  2000/11/21 21:35:28  pmw
 *  Domain change events now work correctly.  moved event transmission
 *  responsibilities to Proactive class.
 *
 *  Revision 1.16  2000/11/21 00:23:54  pmw
 *  Since I didn't have enough to do, a little code reorganization to keep
 *  things from getting dramatically out of hand.  Fixed a couple of
 *  bugs-in-waiting.
 *
 *  Revision 1.15  2000/11/20 05:08:53  pmw
 *  added pds_open_domain call.  semantics are such that a domain id is always
 *  returned, either because one matching the criteria was found or a new one
 *  was created because no matches were found.
 *
 *  Revision 1.14  2000/11/19 23:28:30  pmw
 *  wp_server now supports bulk load of data with -f command-line option.  Also added
 *  command message so bulk loads can happen on-line.
 *
 *  Revision 1.13  2000/10/30 22:44:15  pmw
 *  basic test program works after lots of changes.
 *  ** server still leaks memory; entries are not removed
 *  from global_object_map when contexts or entities are
 *  destroyed.
 *
 *  Revision 1.12  2000/10/25 21:44:48  pmw
 *  Compiles after major revisions to support multiple WP services on
 *  client-side.  This should eventually allow a client to use services by
 *  linking in the library directly or by communicating using CMRPC.
 *
 *  Revision 1.11  2000/09/19 21:12:11  pmw
 *  revised for new cmrpc handler interface
 *
 *  Revision 1.10  2000/04/10 22:03:14  pmw
 *  Added cleanup handlers to eliminate handler memory leaks
 *
 *  Revision 1.9  2000/04/10 20:00:11  pmw
 *  completed port to CM from DataExchange
 *
 *  Revision 1.8  2000/01/06 04:59:04  pmw
 *  changed id scheme so java side is simpler
 *
 *  Revision 1.7  2000/01/05 17:14:32  pmw
 *  server passes basic tests
 *
 *  Revision 1.6  1999/12/21 18:47:54  pmw
 *  Too many changes - really shouldn't have done this many
 *
 *  Revision 1.5  1999/12/09 22:35:13  pmw
 *  added client_priv header, RCS comments
 *
 *
 */
