/*
 *
 *  $Id: client_entity.c,v 1.9 2008-01-01 00:38:15 pmw Exp $
 *
 */

#include "../config.h"

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include "atl.h"
#include "evpath.h"
#include "pds.h"
#include "common/formats.h"


extern pds_entity_id_t null_pds_entity_id;
extern pds_context_id_t null_pds_context_id;
extern pds_domain_id_t null_pds_domain_id;
extern pds_entity_data_t null_pds_entity_data;

extern int
pds_request (const char* rpc_name, pds_service pds, void* in_msg, void* out_msg);
extern pds_service
pds_service_of_id (unsigned char id[PDS_ID_SIZE]);


/*
 *  Entity API
 */



pds_entity_id_t
pds_create_entity (pds_domain_id_t d_id,
		  const char *newname,
                  pds_context_id_t c_id,
		  pds_entity_data_t *edata,
		  attr_list attribute_list)
{
  create_entity_msg msg;
  return_entity_id_msg return_msg;
  pds_entity_id_t new_id;
  pds_service wps;

  msg.encoded_attr_list = attr_list_to_string (attribute_list);
  msg.domain_id = d_id;
  msg.name = newname;
  msg.context_id = c_id;
  msg.options = 0;

  if (edata != NULL)
    msg.edata = *edata;
  else
    msg.edata = null_pds_entity_data;

  wps = pds_service_of_id (d_id.id);
  pds_request (CREATE_ENTITY_RPC_NAME, wps, &msg, &return_msg);

  free (msg.encoded_attr_list);
  new_id = return_msg.entity_id;
  return new_id;
}


static int
send_remove_entity_msg (remove_entity_msg_ptr msg, pds_service pds)
{
  return_status_msg return_msg;
  pds_request (REMOVE_ENTITY_RPC_NAME, pds, msg, &return_msg);
  return return_msg.status;
}

int
pds_remove_entity (pds_domain_id_t d_id, const char *name, pds_context_id_t c_id)
{
  remove_entity_msg msg;
  pds_service wps;

  msg.domain_id = d_id;
  msg.context_id = c_id;
  msg.entity_id = null_pds_entity_id;
  msg.entity_name = name;
  wps = pds_service_of_id (d_id.id);
  return send_remove_entity_msg (&msg, wps);
}
  

int
pds_remove_entity_by_id (pds_domain_id_t d_id, pds_entity_id_t e_id)
{
  remove_entity_msg msg;
  pds_service wps;

  msg.domain_id = d_id;
  msg.context_id = null_pds_context_id;
  msg.entity_name = NULL;
  msg.entity_id = e_id;
  wps = pds_service_of_id (d_id.id);

  return send_remove_entity_msg (&msg, wps);
}


static int
send_bind_entity_msg (bind_entity_msg_ptr msg, pds_service pds)
{
  return_status_msg return_msg;
  pds_request (BIND_ENTITY_RPC_NAME, pds, msg, &return_msg);
  return return_msg.status;
}

int
pds_bind_entity_to_name (pds_domain_id_t d_id, 
			const char *source_name,
                        pds_context_id_t source_context,
			const char *new_name,
                        pds_context_id_t new_name_context)
{
  bind_entity_msg msg;
  pds_service wps = pds_service_of_id (d_id.id);
  
  msg.domain_id = d_id;
  msg.context_id = new_name_context;
  msg.source_context = source_context;
  msg.entity_id = null_pds_entity_id;
  msg.name = new_name;
  msg.source_name = source_name;
 
  return send_bind_entity_msg (&msg, wps);
}


int
pds_bind_entity_to_name_by_id (pds_domain_id_t d_id, 
                              pds_entity_id_t e_id, 
                              const char *name,
                              pds_context_id_t new_context)
{
  bind_entity_msg msg;
  pds_service wps = pds_service_of_id (d_id.id);
  
  msg.domain_id = d_id;
  msg.context_id = new_context;
  msg.entity_id = e_id;
  msg.name = name;
  msg.source_name = NULL;
  msg.source_context = null_pds_context_id;

  return send_bind_entity_msg (&msg, wps);
}


int
pds_unbind_entity_from_name (pds_domain_id_t d_id,
			    const char *name,
			    pds_context_id_t c_id)

{
  unbind_msg msg;
  return_status_msg return_msg;
  pds_service wps = pds_service_of_id (d_id.id);
  
  msg.domain_id = d_id;
  msg.context_id = c_id;
  msg.name = name;

  pds_request (UNBIND_ENTITY_RPC_NAME, wps, &msg, &return_msg);

  return return_msg.status;
}  


pds_entity_id_t
pds_resolve_entity_name (pds_domain_id_t d_id,
                        const char *name,
                        pds_context_id_t c_id)

{
  resolve_name_msg msg;
  resolution_result_msg return_msg;
  pds_service wps;
  
  wps = pds_service_of_id (d_id.id);
  msg.domain_id = d_id;
  msg.context_id = c_id;
  msg.name = name;

  pds_request (RESOLVE_NAME_RPC_NAME, wps, &msg, &return_msg);

  return return_msg.entity_id;
}  


static EVaction
do_registration (int operation,
                 CManager cm,
		 pds_domain_id_t domain,
		 const char* name,
		 pds_context_id_t context,
                 FMStructDescRec *format_list,
		 EVSimpleHandlerFunc func_ptr,
		 void* client_data)
{
  evpath_op_msg msg, return_msg;
  EVstone new_stone;
  EVaction action;
  attr_list contact_attrs = NULL;

  pds_service wps = pds_service_of_id (domain.id);
  msg.domain_id = domain;
  msg.context_id = context;
  msg.name = name;
  msg.stone = 0;
  msg.operation = operation;

  contact_attrs = CMget_contact_list( cm );
  if (contact_attrs == NULL) {
    CMlisten( cm );
    contact_attrs = CMget_contact_list( cm );
  }
  msg.contact_attrs = attr_list_to_string( contact_attrs );

  new_stone = EValloc_stone( cm );
  msg.stone = new_stone;

  pds_request (EVPATH_OP_RPC_NAME, wps, &msg, &return_msg);
  
  if (return_msg.operation != OP_RESULT_OK) {
    EVfree_stone( cm, new_stone );
    return -1;
  }
  
  action = EVassoc_terminal_action( cm, new_stone, format_list, func_ptr, client_data );

  /*
   *  return the sink handle to the caller
   */
  return action;
}

EVaction
pds_register_for_domain_changes (CManager cm,
                                 pds_domain_id_t d_id,
                                 EVSimpleHandlerFunc func_ptr,
                                 void* client_data)
{
  return do_registration (OP_GET_DOMAIN_STONE,
                          cm,
			  d_id,
			  NULL, /* no name */
			  null_pds_context_id,
                          domain_change_event_formats,
			  func_ptr,
			  client_data);
}


EVaction
pds_register_for_entity_changes (CManager cm,
                                 pds_domain_id_t d_id,
                                 const char* name,
                                 pds_context_id_t c_id,
                                 EVSimpleHandlerFunc func_ptr,
                                 void *client_data)
{
  return do_registration (OP_GET_ENTITY_STONE,
                          cm,
			  d_id,
			  name,
			  c_id,
                          entity_change_event_formats,
			  func_ptr,
			  client_data);
}


EVaction
pds_register_for_context_changes (CManager cm,
                                  pds_domain_id_t d_id,
                                  const char* name,
                                  pds_context_id_t c_id,
                                  EVSimpleHandlerFunc func_ptr,
                                  void *client_data)
{
  return do_registration (OP_GET_CONTEXT_STONE,
                          cm,
			  d_id,
			  name,
			  c_id,
                          context_change_event_formats,
			  func_ptr,
			  client_data);
}

int
pds_cancel_changes_registration (EVaction action)
{
#ifdef BLAH
  EChannel ec = sink_handle_map[sink_handle];

  if (ec == NULL)
    {
      /* we inadvertently added an entry to the map with the lookup */
      sink_handle_map.erase (sink_handle);
      return -1;
    }
#endif
  
  /*  EChannel_destroy (ec);*/
  return 1;
}


static int
send_get_attrs_msg (entity_attributes_msg_ptr msg, attr_list attributes, pds_service pds)
{
  entity_attributes_msg return_msg;
  attr_list temp_list;

  msg->operation = OP_GET_ATTRS;

  pds_request (GET_ENTITY_ATTRIBUTES_RPC_NAME, pds, msg, &return_msg);

  if (return_msg.status == -1)
    return -1;

  temp_list = attr_list_from_string (return_msg.encoded_attr_list);
  attr_merge_lists (attributes, temp_list);
  free_attr_list (temp_list);
  
  return 1;
}

int
pds_get_entity_attributes (pds_domain_id_t d_id,
                          const char *name,
                          pds_context_id_t c_id,
                          attr_list attributes)
{
  entity_attributes_msg msg;
  pds_service wps = pds_service_of_id (d_id.id);

  msg.encoded_attr_list = NULL;
  msg.domain_id = d_id;
  msg.context_id = c_id;
  msg.entity_id = null_pds_entity_id;
  msg.entity_name = name;

  return send_get_attrs_msg (&msg, attributes, wps);
}


int
pds_get_entity_attributes_by_id (pds_domain_id_t d_id,
                                pds_entity_id_t e_id,
                                attr_list attributes)
{
  entity_attributes_msg msg;
  pds_service wps = pds_service_of_id (d_id.id);

  msg.encoded_attr_list = NULL;
  msg.domain_id = d_id;
  msg.context_id = null_pds_context_id;
  msg.entity_id = e_id;
  msg.entity_name = NULL;

  return send_get_attrs_msg (&msg, attributes, wps);
}


static int
send_set_attrs_msg (entity_attributes_msg_ptr msg, attr_list attributes, pds_service pds)
{
  return_status_msg return_msg;

  msg->encoded_attr_list = attr_list_to_string (attributes);
  msg->operation = OP_SET_ATTRS;
  pds_request (SET_ENTITY_ATTRIBUTES_RPC_NAME, pds, msg, &return_msg);

  free (msg->encoded_attr_list);
  return return_msg.status;
}
  

int
pds_set_entity_attributes (pds_domain_id_t d_id,
                          const char *name,
                          pds_context_id_t c_id,
                          attr_list attributes,
                          int opt)
{
  entity_attributes_msg msg;
  pds_service wps = pds_service_of_id (d_id.id);

  msg.domain_id = d_id;
  msg.entity_id = null_pds_entity_id;
  msg.context_id = c_id;
  msg.entity_name = name;
  msg.options = opt;

  return send_set_attrs_msg (&msg, attributes, wps);
}


int
pds_set_entity_attributes_by_id (pds_domain_id_t d_id,
                                pds_entity_id_t e_id,
                                attr_list attributes,
                                int opt)
{
  entity_attributes_msg msg;
  pds_service wps = pds_service_of_id (d_id.id);

  msg.domain_id = d_id;
  msg.entity_id = e_id;
  msg.entity_name = NULL;
  msg.context_id = null_pds_context_id;

  return send_set_attrs_msg (&msg, attributes, wps);
}


static int
send_get_data_msg (entity_data_msg_ptr msg, pds_entity_data_t *edata, pds_service pds)
{
  entity_data_msg return_msg;

  pds_request (GET_ENTITY_DATA_RPC_NAME, pds, msg, &return_msg);

  if (return_msg.status == -1)
    return -1;

  *edata = return_msg.edata;

  return 1;
}


int
pds_get_entity_data (pds_domain_id_t d_id,
                    const char *name,
                    pds_context_id_t c_id,
                    pds_entity_data_t *edata,
                    int opt_mask)
{
  entity_data_msg msg;
  pds_service wps = pds_service_of_id (d_id.id);

  msg.domain_id = d_id;
  msg.context_id = c_id;
  msg.edata = null_pds_entity_data;
  msg.entity_id = null_pds_entity_id;
  msg.entity_name = name;
  msg.options = opt_mask;

  return send_get_data_msg (&msg, edata, wps);
}


int
pds_get_entity_data_by_id (pds_domain_id_t d_id,
                          pds_entity_id_t e_id,
                          pds_entity_data_t *edata,
                          int opt_mask)
{
  entity_data_msg msg;
  pds_service wps = pds_service_of_id (d_id.id);

  msg.domain_id = d_id;
  msg.context_id = null_pds_context_id;
  msg.edata = null_pds_entity_data;
  msg.entity_id = e_id;
  msg.entity_name = NULL;
  msg.options = opt_mask;

  return send_get_data_msg (&msg, edata, wps);
}


static int
send_set_data_msg (entity_data_msg_ptr msg, pds_entity_data_t *edata, pds_service pds)
{
  return_status_msg return_msg;

  if (edata != NULL)
    msg->edata = *edata;
  else
    msg->edata = null_pds_entity_data;

  pds_request (SET_ENTITY_DATA_RPC_NAME, pds, msg, &return_msg);
  return return_msg.status;
}


int
pds_set_entity_data (pds_domain_id_t d_id,
                    const char *name,
                    pds_context_id_t c_id,
                    pds_entity_data_t *edata,
                    int opt_mask)
{
  entity_data_msg msg;
  pds_service wps = pds_service_of_id (d_id.id);
  
  msg.domain_id = d_id;
  msg.entity_id = null_pds_entity_id;
  msg.context_id = c_id;
  msg.entity_name = name;
  msg.options = opt_mask;

  return send_set_data_msg (&msg, edata, wps);
}


int
pds_set_entity_data_by_id (pds_domain_id_t d_id,
                          pds_entity_id_t e_id,
                          pds_entity_data_t *edata,
                          int opt_mask)
{
  entity_data_msg msg;
  pds_service wps = pds_service_of_id (d_id.id);

  msg.domain_id = d_id;
  msg.entity_id = e_id;
  
  msg.entity_name = NULL;
  msg.context_id = null_pds_context_id;
  msg.options = opt_mask;

  return send_set_data_msg (&msg, edata, wps);
}


pds_entity_id_t*
pds_find_matching_entities (pds_domain_id_t d_id,
                           const char* name,
                           pds_context_id_t c_id,
			   attr_list attributes)
{
  create_entity_msg msg;
  matching_entities_msg return_msg;
  pds_entity_id_t *list;
  int i;
  pds_service wps = pds_service_of_id (d_id.id);

  msg.encoded_attr_list = attr_list_to_string (attributes);
  msg.domain_id = d_id;
  msg.name = name;

  pds_request (FIND_MATCHING_ENTITIES_RPC_NAME, wps, &msg, &return_msg);
  
  list = (pds_entity_id_t*) calloc (return_msg.entity_count + 1, sizeof (pds_entity_id_t));
  for (i = 0; i < return_msg.entity_count; i++)
    {
      list[i] = return_msg.entity_list[i];
    }
  
  free (msg.encoded_attr_list);
  return list;
}


pds_entity_id_t*
pds_find_matching_entities_by_name (pds_domain_id_t d_id,
				   const char *fullname,
				   attr_list attributes)

{
    /*pds_service wps = pds_service_of_id (d_id.id);*/
  return NULL;
}

/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.8  2007/12/24 14:40:24  eisen
 *  Partial conversion to FFS-based CMRPC.
 *
 *  Revision 1.7  2007/12/04 21:06:14  pmw
 *  Merge changes from evpath branch.  Also probably includes
 *  Darwin fixes.  Compiles on isleroyale and marquesas, I think
 *  I'm misconfigured somehow for building on gondor.
 *
 *  Revision 1.6.2.2  2007/01/25 22:04:55  pmw
 *  now correctly gets local contact information by CMlisten() if necessary in do_registration()
 *
 *  Revision 1.6.2.1  2007/01/05 22:40:27  pmw
 *  interim commit for evpath changes, new branch
 *
 *  Revision 1.6  2005/12/08 23:37:04  eisen
 *  Fix includes after removing #include of cm.h from echo.h.
 *
 *  Revision 1.5  2005/02/24 19:28:32  pmw
 *  Everything should now be using string encoding for attr lists.  I
 *  removed all the ifdefs.
 *
 *  Revision 1.4  2003/02/04 17:07:51  mansour
 *  rgister the correct event type for domain events
 *
 *  Revision 1.3  2003/02/03 15:30:17  chaos
 *  remove C++-style comments from C files so they compile on elvis
 *
 *  Revision 1.2  2002/08/07 18:51:43  eisen
 *  Widespread changes to modify how attribute handling works.  Stop using
 *  attr_atom_from_string() to get atom values.  Instead, those are set by the
 *  application.  Stop using get_attr() (api has changes).  Stop using
 *  xmit_object and start using the ATL-supplied encode/decode routines.
 *
 *  Revision 1.1  2001/10/09 21:37:53  pmw
 *  configuration changes
 *
 *  Revision 1.1  2001/09/10 03:56:50  pmw
 *  C-language versions of client-side code, now compiled exclusively with gcc.
 *
 *  Revision 1.7  2001/09/07 15:19:36  pmw
 *  forced to remove all c++ structures from client lib.  should have done
 *  this long ago but I'm a c++ head so I keep forgetting.  Need to go back and
 *  compile client libs with only gcc.
 *
 *  Revision 1.6  2001/09/06 15:03:33  pmw
 *  fixed attribute list handling using string encoding
 *
 *  Revision 1.5  2001/09/03 20:09:10  pmw
 *  changes to deal with string-marshaled attr lists
 *
 *  Revision 1.4  2001/07/02 15:20:01  pmw
 *  remove typo
 *
 *  Revision 1.3  2001/07/02 15:07:43  pmw
 *  removed ContextBindable class def and impl to their own source files
 *
 *  Revision 1.2  2001/06/12 03:14:32  pmw
 *  minor fixes
 *
 *  Revision 1.1.1.1  2000/12/13 22:14:20  pmw
 *  imported sources
 *
 *  Revision 1.13  2000/11/29 04:37:36  pmw
 *  changed channel-handling again, Entity now performs most of its own
 *  change event sends
 *
 *  Revision 1.12  2000/11/22 21:42:35  pmw
 *  proactivity based on entity events now appears to work correctly.  In particular,
 *  creation events based on the prior registration of placeholder entities and their
 *  later "creation" as real entities works.
 *
 *  Revision 1.11  2000/11/21 21:35:26  pmw
 *  Domain change events now work correctly.  moved event transmission
 *  responsibilities to Proactive class.
 *
 *  Revision 1.10  2000/11/21 00:23:48  pmw
 *  Since I didn't have enough to do, a little code reorganization to keep
 *  things from getting dramatically out of hand.  Fixed a couple of
 *  bugs-in-waiting.
 *
 *  Revision 1.9  2000/11/19 23:28:27  pmw
 *  pds_server now supports bulk load of data with -f command-line option.  Also added
 *  command message so bulk loads can happen on-line.
 *
 *  Revision 1.8  2000/10/25 21:44:44  pmw
 *  Compiles after major revisions to support multiple WP services on
 *  client-side.  This should eventually allow a client to use services by
 *  linking in the library directly or by communicating using CMRPC.
 *
 *  Revision 1.7  2000/04/10 20:00:07  pmw
 *  completed port to CM from DataExchange
 *
 *  Revision 1.6  2000/01/06 04:59:03  pmw
 *  changed id scheme so java side is simpler
 *
 *  Revision 1.5  2000/01/05 17:14:31  pmw
 *  server passes basic tests
 *
 *  Revision 1.4  1999/12/21 18:47:52  pmw
 *  Too many changes - really shouldn't have done this many
 *
 *  Revision 1.3  1999/12/09 22:35:12  pmw
 *  added client_priv header, RCS comments
 *
 *
 */