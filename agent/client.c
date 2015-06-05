/*
 *
 *  $Id: client.c,v 1.30 2008-09-24 12:39:15 eisen Exp $
 *  
 */

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <netinet/in.h> /* for struct in_addr */
#include <netdb.h> /* for gethostbyname */

#include "atl.h"
#include "evpath.h"
#include "cmrpc.h"

#include "config.h"
#include "pds.h"
#include "common/formats.h"

pds_entity_id_t null_pds_entity_id = { { '\0' } };
pds_context_id_t null_pds_context_id = { { '\0' } };
pds_domain_id_t null_pds_domain_id = { { '\0'}  };
pds_entity_data_t null_pds_entity_data = { NULL, 0, Attr_Undefined };

typedef struct _pds_service_struct
{
  in_addr_t address;
  unsigned short port;
  attr_list contact_list;
} pds_service_struct;

typedef struct _pds_service_list_node
{
  pds_service service;
  struct _pds_service_list_node* next;
} pds_service_list_node;

static pds_service_list_node open_services = { (pds_service)0, (pds_service_list_node*)0 };

static CManager client_cm;

static void register_formats();
static int formats_registered = 0;

/*static int service_counter = 0;*/

static void
register_formats()
{

    /*
     * groupings don't mean anything, they're just for readability
     */

  if (client_cm == NULL) client_cm = CManager_create();

  /* 
   *  try to fork a comm thread.  If threads have not been initialized, 
   *  this will fail will no ill effects.  If they have, then that thread 
   *  will handle the network for PDS communication.
   */
  CMfork_comm_thread(client_cm);

  CMrpc_register_rpc_request (client_cm, CREATE_DOMAIN_RPC_NAME, NULL,
                              create_domain_msg_formats, return_domain_id_msg_formats);

  CMrpc_register_rpc_request (client_cm, REMOVE_DOMAIN_RPC_NAME, NULL,
                              remove_domain_msg_formats, 
                              return_status_msg_formats);
  CMrpc_register_rpc_request (client_cm, FIND_MATCHING_DOMAINS_RPC_NAME, NULL, 
                             create_domain_msg_formats, 
                             matching_domains_msg_formats);

  CMrpc_register_rpc_request (client_cm, SEND_DOMAIN_ATTRIBUTES_RPC_NAME, NULL, 
                             domain_attributes_msg_formats, 
                             return_status_msg_formats);

  CMrpc_register_rpc_request (client_cm, CREATE_CONTEXT_RPC_NAME, NULL, 
                             create_context_msg_formats, 
                             return_context_id_msg_formats);


  CMrpc_register_rpc_request (client_cm, GET_ROOT_CONTEXT_RPC_NAME, NULL, 
                             get_root_context_msg_formats, 
                             return_context_id_msg_formats);

  CMrpc_register_rpc_request (client_cm, REMOVE_CONTEXT_RPC_NAME, NULL, 
                             remove_context_msg_formats, 
                             return_status_msg_formats);

  CMrpc_register_rpc_request (client_cm, GET_BINDING_LIST_RPC_NAME, NULL, 
                             resolve_name_msg_formats, 
                             binding_list_msg_formats);

  CMrpc_register_rpc_request (client_cm, CREATE_ENTITY_RPC_NAME, NULL, 
                             create_entity_msg_formats, 
                             return_entity_id_msg_formats);

  CMrpc_register_rpc_request (client_cm, REMOVE_ENTITY_RPC_NAME, NULL, 
                             remove_entity_msg_formats, 
                             return_status_msg_formats);


  CMrpc_register_rpc_request (client_cm, BIND_ENTITY_RPC_NAME, NULL, 
                             bind_entity_msg_formats, 
                             return_status_msg_formats);

  CMrpc_register_rpc_request (client_cm, UNBIND_ENTITY_RPC_NAME, NULL, 
                             unbind_msg_formats, 
                             return_status_msg_formats);

  CMrpc_register_rpc_request (client_cm, RESOLVE_NAME_RPC_NAME, NULL, 
                             resolve_name_msg_formats, 
                             resolution_result_msg_formats);

  CMrpc_register_rpc_request (client_cm, EVPATH_OP_RPC_NAME, NULL, 
                             evpath_op_msg_formats, 
                             evpath_op_msg_formats);

  CMrpc_register_rpc_request (client_cm, GET_ENTITY_ATTRIBUTES_RPC_NAME, NULL, 
                             entity_attributes_msg_formats, 
                             entity_attributes_msg_formats);


  CMrpc_register_rpc_request (client_cm, SET_ENTITY_ATTRIBUTES_RPC_NAME, NULL, 
                             entity_attributes_msg_formats, 
                             return_status_msg_formats);

  CMrpc_register_rpc_request (client_cm, GET_ENTITY_DATA_RPC_NAME, NULL, 
                             entity_data_msg_formats, 
                             entity_data_msg_formats);

  CMrpc_register_rpc_request (client_cm, SET_ENTITY_DATA_RPC_NAME, NULL, 
                             entity_data_msg_formats, 
                             return_status_msg_formats);

  CMrpc_register_rpc_request (client_cm, GET_CONTEXT_ATTRIBUTES_RPC_NAME, NULL, 
                              context_attributes_msg_formats, 
                              context_attributes_msg_formats);


  CMrpc_register_rpc_request (client_cm, SET_CONTEXT_ATTRIBUTES_RPC_NAME, NULL, 
                              context_attributes_msg_formats, 
                              return_status_msg_formats);

  CMrpc_register_rpc_request (client_cm, FIND_MATCHING_ENTITIES_RPC_NAME, NULL, 
                             create_entity_msg_formats, 
                             matching_entities_msg_formats);

  CMrpc_register_rpc_request (client_cm, LOAD_FROM_URL_RPC_NAME, NULL, 
                              load_from_URL_msg_formats,
                              return_status_msg_formats);

  CMrpc_register_rpc_request (client_cm, OPEN_DOMAIN_RPC_NAME, NULL,
			      create_domain_msg_formats,
			      return_domain_id_msg_formats);

  CMrpc_register_rpc_request (client_cm, SHUTDOWN_SERVER_RPC_NAME, NULL,
                              shutdown_server_msg_formats,
                              return_status_msg_formats);
			      
  CMrpc_register_rpc_request (client_cm, QUERY_BY_XPATH_RPC_NAME, NULL,
                              xpath_query_request_msg_formats,
                              xpath_query_results_msg_formats);

  CMrpc_register_rpc_request (client_cm, OPEN_SERVICE_RPC_NAME, NULL,
                              open_service_msg_formats,
                              open_service_msg_formats);

  CMrpc_register_rpc_request (client_cm, BIND_CONTEXT_RPC_NAME, NULL,
                             bind_context_msg_formats,
                             return_status_msg_formats);
  
  CMrpc_register_rpc_request (client_cm, UNBIND_CONTEXT_BY_ID_RPC_NAME, NULL,
                             unbind_context_msg_formats,
                             return_status_msg_formats);
  
  
  formats_registered = 1;
}


CManager
pds_get_CManager()
{
  return client_cm;
}


int
pds_request (char* rpc_name, pds_service pds, void* in_msg, void* out_msg)
{
  CMrpc_call_rpc (client_cm, rpc_name, pds->contact_list, in_msg, out_msg, 0);
  return 1;
}


pds_service
pds_service_open (attr_list contact_list)
{
  char* hostname;
  struct hostent *hostptr;
  struct in_addr *p;
  pds_service_list_node *p1;
  pds_service_list_node *q = NULL;
  unsigned int port_attr = 0;
  unsigned short net_order_port_attr = 0;
  in_addr_t host_addr = 0;
  open_service_msg in_msg, out_msg;
  pds_service_struct *new_service = NULL;
  static atom_t PDS_IP_PORT = -1;
  static atom_t PDS_IP_HOST = -1;

  if (! formats_registered)
    register_formats();

  /*
   *  first make sure that we don't already have a service
   *  set up for this contact list
   */

  if (PDS_IP_PORT == -1) {
      PDS_IP_PORT = attr_atom_from_string("IP_PORT");
      PDS_IP_HOST = attr_atom_from_string("IP_HOST");
  }
  if (!query_attr (contact_list, PDS_IP_PORT,
                   NULL, (attr_value*) &port_attr))
    {
      port_attr = PDS_SERVER_PORT;
    }

  /*
   *  All port numbers stored for server identification purposes
   *  are stored in network byte-order.  We also always generate IDs
   *  with object references in network byte-order.  This isn't a concern
   *  for exchanges through a particular pds_service, since we use PBIO underneath
   *  to handle heterogeneity issues.  However, if someone transmits an object
   *  id in some out-of-band manner, we'd be stuck.  so, we do a bit of extra
   *  work to make sure we're always consistent in the way we look at this 
   *  particular bit pattern.
   */
  net_order_port_attr = htons (port_attr);

  if (!query_attr (contact_list, PDS_IP_HOST,
                   NULL, (attr_value*) &hostname))
    {
      hostname = getenv ("PDS_SERVER_HOST");
    }

  /*
   *  find the server address 32-bit value.  we don't have to convert
   *  this since gethostbyname always returns the value in network
   *  order.
   */
  /*  host_addr = ++service_counter; */

  hostptr = gethostbyname (hostname);
  p = (struct in_addr*) hostptr->h_addr_list[0];
  host_addr = p->s_addr;

  for (q = open_services.next; q != NULL; q = q->next)
    {    
	; /* intentional empty statement */
      if (q->service->address == host_addr && q->service->port == net_order_port_attr)
        break;
    }
  
  if (q != NULL) return q->service;

  /*
   *  have to open a new one
   */
  new_service = (pds_service) malloc (sizeof (pds_service_struct));
  /* so that someone doesn't nuke the contact list out from under us */
  add_ref_attr_list (contact_list);
  new_service->contact_list = contact_list;

  pds_request (OPEN_SERVICE_RPC_NAME, new_service, &in_msg, &out_msg);

  /* 
   *  convert the values to network order since they're sent with
   *  integer PBIO encoding
   */

  new_service->address = htonl(out_msg.addr);
  new_service->port = htons(out_msg.port);

  p1 = (pds_service_list_node*) malloc (sizeof (pds_service_list_node));
  p1->service = new_service;
  p1->next = open_services.next;
  open_services.next = p1;

  return new_service;
}

int
pds_service_close (pds_service_struct* wps)
{
  pds_service_list_node *p, *q;
      
  for (p = open_services.next, q = &open_services; 
       p != NULL && p->service != wps; 
       p = p->next, q = q->next)
    {
	; /* intentional empty statement */
    }
  if (p != NULL)
    {
      q->next = p->next;
      free (p);
    }

  free_attr_list (wps->contact_list);
  free (wps);
  return 1;
}


pds_service
pds_service_of_id (unsigned char id[PDS_ID_SIZE])
{
  /*
   *  Extract an in_addr_t and an in_port_t from the
   *  front of the ID.  this should be the server address and port.
   *  Use these two values to locate a pds_service in the list.
   */
  in_addr_t id_addr;
  in_port_t id_port;
  pds_service_list_node *p;
  int found = 0;
/*  pds_service found_service = NULL;*/
  unsigned char* idp = id;

  id_addr = *((in_addr_t *)idp);

  //  memcpy (&id_addr, idp, sizeof (unsigned long));
  idp += sizeof (in_addr_t);
  memcpy (&id_port, idp, sizeof (in_port_t));

  p = open_services.next;
  if (p != NULL) found = (p->service->address == id_addr && p->service->port == id_port);
  for (;p != NULL && !found; p = p->next)
    {
      pds_service w = p->service;
      fprintf (stderr, "address (%x), port (%d)\n", id_addr, id_port);
      fflush (stderr);
      found = (w->address == id_addr && w->port == id_port);
/*      if (found) found_service = w;*/
    }

  assert (p != NULL);
  return p->service;
}


int
pds_shutdown_server (pds_service wps)
{
  shutdown_server_msg msg;
  return_status_msg return_msg;
  pds_request (SHUTDOWN_SERVER_RPC_NAME, wps, &msg, &return_msg);
  return return_msg.status;
}

/*
 *
 *  API Implementation
 *
 */


pds_domain_id_t
pds_create_domain (pds_service wps,
                  const char *domain_name,
		  const char *domain_type,
		  int domain_version,
		  const char *application_name)
{
  create_domain_msg msg;
  return_domain_id_msg return_msg;
  pds_domain_id_t new_domain_id;

  msg.domain_name = domain_name;
  msg.domain_type = domain_type;
  msg.application_name = application_name;
  msg.domain_version = domain_version;
  msg.options = 0;

  pds_request (CREATE_DOMAIN_RPC_NAME, wps, &msg, &return_msg);

  new_domain_id = return_msg.domain_id;
  
  return new_domain_id;  
}

pds_domain_id_t
pds_open_domain (pds_service wps,
		const char *domain_name,
		const char *domain_type,
		int domain_version,
		const char *application_name)
{
  create_domain_msg msg;
  return_domain_id_msg return_msg;

  msg.domain_name = domain_name;
  msg.domain_type = domain_type;
  msg.application_name = application_name;
  msg.domain_version = domain_version;
  msg.options = 0;

  pds_request (OPEN_DOMAIN_RPC_NAME, wps, &msg, &return_msg);

  ID_TRACE_OUT(pdsagentVerbose, "domain id is ", return_msg.domain_id.id);

  return return_msg.domain_id;
}

int
pds_remove_domain (pds_domain_id_t id)
{
  remove_domain_msg msg;
  return_status_msg return_msg;
  
  msg.domain_id = id;
  
  pds_request (REMOVE_DOMAIN_RPC_NAME, pds_service_of_id (id.id), &msg, &return_msg);

  return return_msg.status;
}  


int
pds_find_matching_domains (pds_service wps,
                          const char *domain_name,
			  const char *domain_type,
			  int domain_version,
			  const char *application_name,
                          pds_domain_id_t **matches)
{
  create_domain_msg msg;
  matching_domains_msg return_msg;
  int i;

  msg.domain_name = domain_name;
  msg.domain_type = domain_type;
  msg.application_name = application_name;
  msg.domain_version = domain_version;
  
  pds_request (FIND_MATCHING_DOMAINS_RPC_NAME, wps, &msg, &return_msg);

  *matches = (pds_domain_id_t*) calloc (return_msg.domain_count, sizeof (pds_domain_id_t));
  for (i = 0; i < return_msg.domain_count; i++)
    {
      *matches[i] = return_msg.domain_list[i];
    }

  free (return_msg.domain_list);
  return return_msg.domain_count;
}



int
pds_set_domain_attrs (pds_domain_id_t d_id, attr_list list)
{
  domain_attributes_msg msg;
  return_status_msg return_msg;
  pds_service wps;

  msg.domain_id = d_id;
  msg.encoded_attr_list = attr_list_to_string (list);


  msg.operation = OP_SET_ATTRS;

  wps = pds_service_of_id (d_id.id);
  pds_request (SEND_DOMAIN_ATTRIBUTES_RPC_NAME, wps, &msg, &return_msg);

  free (msg.encoded_attr_list);

  return return_msg.status;
}

int
pds_get_domain_attrs (pds_domain_id_t d_id, attr_list al)
{
  domain_attributes_msg msg;
  domain_attributes_msg return_msg;
  pds_service wps;
  attr_list temp_list;
  int rc;

  msg.domain_id = d_id;

  msg.encoded_attr_list = NULL;

  msg.operation = OP_GET_ATTRS;

  wps = pds_service_of_id (d_id.id);
  pds_request (SEND_DOMAIN_ATTRIBUTES_RPC_NAME, wps, &msg, &return_msg);
  temp_list = attr_list_from_string (msg.encoded_attr_list);
  attr_merge_lists (al, temp_list);
  rc = attr_count (temp_list);
  free_attr_list (temp_list);
  return rc;
}


/*
 *  Context API
 */
static
int
send_create_context_request (create_context_msg_ptr request,
                             pds_service pds,
                             pds_context_id_t *new_id)
{
  return_context_id_msg return_msg;

  pds_request (CREATE_CONTEXT_RPC_NAME, pds, request, &return_msg);

  *new_id = return_msg.context_id;
  return return_msg.options;
}

pds_context_id_t
pds_create_context (pds_domain_id_t d_id, const char *name, pds_context_id_t c_id)
{
  create_context_msg msg;
  pds_context_id_t new_id;
  pds_service wps;

  msg.encoded_attr_list = NULL;

  msg.domain_id = d_id;
  msg.context_id = c_id;
  msg.c_name = name;

  wps = pds_service_of_id (d_id.id);

  send_create_context_request (&msg, wps, &new_id);

  return new_id;
}


pds_context_id_t
pds_get_root_context (pds_domain_id_t d_id)
{
  get_root_context_msg msg;
  return_context_id_msg return_msg;
  pds_context_id_t root_id;
  pds_service wps = pds_service_of_id (d_id.id);  

  msg.domain_id = d_id;
  msg.options = 0;
  pds_request (GET_ROOT_CONTEXT_RPC_NAME, wps, &msg, &return_msg);
  root_id = return_msg.context_id;

  return root_id;
}

int 
pds_remove_context (pds_domain_id_t d_id, const char *name, pds_context_id_t name_context)
{
  remove_context_msg msg;
  return_status_msg return_msg;
  pds_service wps = pds_service_of_id (d_id.id);
  
  msg.domain_id = d_id;
  msg.context_id = name_context;
  msg.c_name = name;
  
  pds_request (REMOVE_CONTEXT_RPC_NAME, wps, &msg, &return_msg);
  return return_msg.status;
}

int
pds_remove_context_by_id (pds_domain_id_t d_id, pds_context_id_t c_id)
{
  remove_context_msg msg;
  return_status_msg return_msg;
  pds_service wps;

  wps = pds_service_of_id (d_id.id);
  msg.domain_id = d_id;
  msg.context_id = c_id;
  msg.c_name = NULL;

  pds_request (REMOVE_CONTEXT_RPC_NAME, wps, &msg, &return_msg);

  return return_msg.status;
}


static 
char**
send_binding_list_request (resolve_name_msg_ptr request, pds_service pds)
{
  binding_list_msg return_msg;
  char **ret_list;
  int i;
  
  pds_request (GET_BINDING_LIST_RPC_NAME, pds, request, &return_msg);

  /* our good friend calloc sets the allocated memory to NULLs,
   * providing a terminator for the array
   */
  ret_list = (char**) calloc (return_msg.binding_count + 1, sizeof (char*));
  for (i = 0; i < return_msg.binding_count; i++)
    {
      ret_list[i] = strdup (return_msg.binding_list[i]);
    }

  return ret_list;
}



char**
pds_get_binding_list (pds_domain_id_t d_id, const char* name, int opt_mask)
{
  resolve_name_msg request_msg;
  pds_service wps = pds_service_of_id (d_id.id);

  request_msg.domain_id = d_id;
  request_msg.context_id = null_pds_context_id;
  request_msg.name = (char*) name;
  request_msg.options = opt_mask;

  return send_binding_list_request (&request_msg, wps);
}


char**
pds_get_binding_list_id (pds_domain_id_t d_id, pds_context_id_t c_id, int opt_mask)
{
  resolve_name_msg request_msg;
  pds_service wps = pds_service_of_id (d_id.id);

  request_msg.domain_id = d_id;
  request_msg.context_id = c_id;
  request_msg.name = NULL;
  request_msg.options = opt_mask;

  return send_binding_list_request (&request_msg, wps);
}


int
pds_set_context_attrs (pds_domain_id_t d_id, pds_context_id_t c_id, const char* name,
                       attr_list list)
{
  context_attributes_msg msg;
  return_status_msg return_msg;
  pds_service wps;

  msg.domain_id = d_id;
  msg.context_id = c_id;
  msg.name = name;

  msg.encoded_attr_list = attr_list_to_string (list);

  msg.operation = OP_SET_ATTRS;

  wps = pds_service_of_id (d_id.id);
  pds_request (SET_CONTEXT_ATTRIBUTES_RPC_NAME, wps, &msg, &return_msg);

  free (msg.encoded_attr_list);

  return return_msg.status;
}

int
pds_get_context_attrs (pds_domain_id_t d_id, pds_context_id_t c_id, const char* name, 
                       attr_list al)
{
  context_attributes_msg msg;
  context_attributes_msg return_msg;
  pds_service wps;
  attr_list temp_list;
  int rc;

  msg.domain_id = d_id;
  msg.context_id = c_id;
  msg.name = name;

  msg.encoded_attr_list = NULL;
  msg.operation = OP_GET_ATTRS;

  wps = pds_service_of_id (d_id.id);
  pds_request (GET_CONTEXT_ATTRIBUTES_RPC_NAME, wps, &msg, &return_msg);

  temp_list = attr_list_from_string (msg.encoded_attr_list);
  attr_merge_lists (al, temp_list);
  rc = attr_count (temp_list);
  free_attr_list (temp_list);
  return rc;
}

 static
 int
 send_bind_context_request (bind_context_msg_ptr request,
                            pds_service pds,
                            int * ret_status)
 {

   return_status_msg return_msg;

   pds_request (BIND_CONTEXT_RPC_NAME, pds, request, &return_msg);

   return return_msg.status;
 }

 static
 int
 send_unbind_context_request (unbind_context_msg_ptr request,
                              pds_service pds,
                              int * ret_status)
 {

   return_status_msg return_msg;

   pds_request (UNBIND_CONTEXT_BY_ID_RPC_NAME, pds, request, &return_msg);

   return return_msg.status;
 }


 int
 pds_bind_context_by_id (pds_domain_id_t d_id,
                         pds_context_id_t source_context,
                         const char *link_name)
 {
   bind_context_msg msg;
   pds_service wps;
   int ret_status;

   msg.domain_id = d_id;
   msg.source_context_id = source_context;
   msg.source_context_name = 0;
   msg.link_name = link_name;

   wps = pds_service_of_id (d_id.id);

   send_bind_context_request (&msg, wps, &ret_status);

   return ret_status;
 }
 int
 pds_unbind_context_from_name (pds_domain_id_t d_id, const char *name)
 {
   unbind_context_msg msg;
   pds_service wps;
   int ret_status;

   msg.domain_id = d_id;
   msg.name = name;

   wps = pds_service_of_id (d_id.id);

   send_unbind_context_request (&msg, wps, &ret_status);

   return ret_status;
 }

 int
 pds_bind_context_by_name (pds_domain_id_t d_id,
                         const char* context_name,
                         const char *link_name)
 {
   bind_context_msg msg;
   pds_service wps;
   int ret_status;

   msg.domain_id = d_id;
   msg.source_context_id = null_pds_context_id;
   msg.source_context_name = context_name;
   msg.link_name = link_name;

   wps = pds_service_of_id (d_id.id);

   send_bind_context_request (&msg, wps, &ret_status);

   return ret_status;
 }



int
pds_bulk_load_from_URL (pds_service wps, const char* name)
{
  return_status_msg return_msg;
  load_from_URL_msg request_msg;

  request_msg.url = name;
  
  pds_request (LOAD_FROM_URL_RPC_NAME, wps, &request_msg, &return_msg);
  return return_msg.status;
}
/****************************/
static void 
build_criterion(char* buf, char* entity, char*op, char* value)
{
  if (value != 0 && strlen(value) > 0)
    sprintf(buf, "(@name=\"%s\" and @value%s\"%s\")", 
      entity, op, value);
  else
    sprintf(buf, "(@name=\"%s\")", entity);
}
/****************************/
static char*
build_disjunction(pds_query_struct* criteria)
{
  char buf[100];
  int length = 0;
  char* ret;
  int num = 0;
  int i = 0;
  char* prefix="[PDSEntity[";
  char* suffix = "]]";
  char** elements = 0;
  
  while(criteria[num].entity != 0) num++;
  
  elements = (char**) malloc(1+num * sizeof(char*));    
  
  for(i=0; i<num; i++) {
    build_criterion(buf, criteria[i].entity, criteria[i].op, criteria[i].value);
    elements[i] = strdup(buf);
    length += strlen(buf);   
  }
  
  ret = (char*) malloc(length + 1 + strlen(prefix) + strlen(suffix)+ 4*num );
  strcpy(ret, prefix);    
  for (i=0; i<num; i++) {    
    if (i != 0)
      strcat(ret, " or ");
    strcat(ret, elements[i]);
    free(elements[i]);    
  }
  strcat(ret, suffix);    
  free(elements);
  return ret;
}
/****************************/
static char*
build_conjunction(pds_query_struct* criteria)
{
  char buf[100];
  int length = 0;
  char* ret;
  int num = 0;
  int i = 0;
  char* prefix="[PDSEntity[";
  char* suffix = "]]";
  char** elements;
  
  while(criteria[num].entity != 0) num++;
    
  elements = (char**) malloc(1+num * sizeof(char*));    
  
  for(i=0; i<num; i++) {
    build_criterion(buf, criteria[i].entity, criteria[i].op, criteria[i].value);
    elements[i] = (char*) malloc(sizeof(buf) + sizeof(prefix) + sizeof(suffix) + 6);
    strcpy(elements[i], prefix);
    strcat(elements[i], buf);
    strcat(elements[i], suffix);
    length += strlen(elements[i]);
  }
  
  ret = (char*) malloc(length + 1);
  strcpy(ret, "");    
  for (i=0; i<num; i++) {    
    strcat(ret, elements[i]);
    free(elements[i]);    
  }
  free(elements);
  return ret;
}
/****************************/
static char*
build_cnf(pds_query_struct*criteria[], int num)
{
  int i;
  char** elements;
  int length = 0;
  char* ret;
  char* prefix = "//PDSContext";
  char* suffix = "";


  
  elements = (char**) malloc(num * sizeof(char*));
  
  for (i=0; i<num; i++) {
    elements[i] = build_disjunction(criteria[i]);
    length += strlen(elements[i]);
  }

  ret = (char*) malloc(length + strlen(prefix) + strlen(suffix) + 1);
  
  strcpy(ret, prefix);
  for (i=0; i<num; i++) {
    strcat(ret, elements[i]);
    free(elements[i]);
  }
  strcat(ret, suffix);

  free(elements);
  return ret;
}
/****************************/
char* 
pds_build_xpath_query(pds_query_struct* criteria, int join_operator)
{
  char* ret = 0;
  char* temp = 0;
  char* prefix = "//PDSContext";
  switch(join_operator) {
    case PDS_OR_JOIN:
      temp = build_disjunction(criteria);
      break;
    case PDS_AND_JOIN:
      temp = build_conjunction(criteria);
      break;
  }
  ret = (char*) malloc(strlen(temp) + 1 + strlen(prefix));
  strcpy(ret, prefix);
  strcat(ret, temp);
  free(temp);
  return ret;
}
/****************************/
char* 
pds_build_extended_xpath_query(pds_query_struct* criteria[], int num)
{
  return build_cnf(criteria, num);
}
/****************************/
char**
pds_query_by_xpath(pds_domain_id_t d_id, const char* xpath)
{
  xpath_query_request_msg msg;
  xpath_query_results_msg return_msg;
  char **ret_list;
  int i;
  pds_service wps;

  msg.xpath = (char*)xpath;
  msg.domain_id = d_id;

  wps = pds_service_of_id (d_id.id);

  pds_request (QUERY_BY_XPATH_RPC_NAME, wps, &msg, &return_msg);

  /* our good friend calloc sets the allocated memory to NULLs,
   * providing a terminator for the array
   */
  ret_list = (char**) calloc (return_msg.name_count + 1, sizeof (char*));
  for (i = 0; i < return_msg.name_count; i++)
    {
      ret_list[i] = strdup (return_msg.name_list[i]);
    }

  return ret_list;
}



/*
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.29  2008/01/01 00:38:15  pmw
 * Changes to make PDS work with the new FFS libraries
 *
 * Revision 1.27  2007/12/04 21:06:14  pmw
 * Merge changes from evpath branch.  Also probably includes
 * Darwin fixes.  Compiles on isleroyale and marquesas, I think
 * I'm misconfigured somehow for building on gondor.
 *
 * Revision 1.26.2.1  2007/01/05 22:40:27  pmw
 * interim commit for evpath changes, new branch
 *
 * Revision 1.26  2006/10/11 12:28:59  chaos
 * cm.h issues
 *
 * Revision 1.25  2006/10/11 02:01:24  chaos
 * Change so we don't have include of evpath.h in cmrpc.h
 *
 * Revision 1.24  2005/08/23 19:52:09  eisen
 * Try to fix some 64/32-bit issues.  Try to use in_addr_t where we're
 * manipulating and representing IP addresses.  (Expect to fix this again for
 * IPv6, or when hell freezes over, whichever comes first.)
 *
 * Revision 1.23  2005/07/19 16:45:06  eisen
 * Hopefully fix the "multiple threads calling" messages from CM.
 *
 * Revision 1.22  2005/02/25 22:17:40  pmw
 * I think this restores the changes that {I | CVS} whacked.  basic2 runs
 * but does not terminate, I'm not sure if this is normal behavior or not.
 *
 * Revision 1.21  2005/02/24 19:28:32  pmw
 * Everything should now be using string encoding for attr lists.  I
 * removed all the ifdefs.
 *
 * Revision 1.20  2005/02/22 07:18:18  pmw
 * If there's any justice, this has knocked the cross-platform
 * connection bug.  The open_service exchange actually sends
 * the address and port with PBIO integer encoding, so in some
 * cases there's an extra byte-swap happening.  Fixed by performing
 * byte-swapping manually for just that case BEFORE the addr/port
 * is stored in pdsagent's list of services.
 *
 * Revision 1.17  2004/10/14 21:17:29  pmw
 * Changes to support the CMrpc renaming.
 *
 * Revision 1.16  2004/04/27 20:59:22  pmw
 * Added RPC for open_service to simplify service_from_ID handling on the
 * client.  The way I set that up at first was a little braindead, and failed
 * when one side was weakly hostnamed (via DHCP, for example).  The new RPC
 * asks the server what address/port combination it intends to use and saves
 * those.
 *
 * Revision 1.15  2003/11/09 21:37:27  chaos
 * Count the exact number of criteria
 *
 * Revision 1.14  2003/11/09 15:43:16  mansour
 * fix one more compile error
 *
 * Revision 1.13  2003/11/09 15:18:28  mansour
 * fix compile errors
 *
 * Revision 1.12  2003/11/09 03:56:36  mansour
 * Add wrapper API for building XPath queries
 *
 * Revision 1.11  2003/11/03 18:11:17  mansour
 * register new RPC call
 *
 * Revision 1.10  2003/10/31 13:15:50  chaos
 * Change to K&R declaration order
 *
 * Revision 1.9  2003/10/31 05:38:30  mansour
 * Added xpath query support
 *
 * Revision 1.8  2003/02/13 13:00:03  eisen
 * Remove C++-style comments from C file
 *
 * Revision 1.7  2003/02/12 20:50:06  eisen
 * Tweaks to kill warnings on Irix compilers.
 *
 * Revision 1.6  2003/02/03 15:30:17  chaos
 * remove C++-style comments from C files so they compile on elvis
 *
 * Revision 1.5  2003/01/25 13:11:44  mansour
 * Initialize the attr_block data member. This fixes a bug with pds_create_context
 *
 * Revision 1.4  2002/10/31 18:20:29  pmw
 * Fixed bug in context name mangling.
 *
 * Revision 1.3  2002/10/30 03:52:50  pmw
 * added parameter to get_binding_list functions to optionally get only entities
 * or contexts
 *
 * Revision 1.2  2002/08/07 18:51:43  eisen
 * Widespread changes to modify how attribute handling works.  Stop using
 * attr_atom_from_string() to get atom values.  Instead, those are set by the
 * application.  Stop using get_attr() (api has changes).  Stop using
 * xmit_object and start using the ATL-supplied encode/decode routines.
 *
 * Revision 1.1  2001/10/09 21:37:53  pmw
 * configuration changes
 *
 * Revision 1.2  2001/09/23 22:55:53  pmw
 * Contexts now support attributes of their own.  Practically, this is supported
 * by having ContextBindable host the attribute list instead of Entity/Context.  Also
 * added the usual functions on the client side and handlers for getting/setting
 * this attr list for Contexts.
 *
 * Revision 1.1  2001/09/10 03:56:49  pmw
 * C-language versions of client-side code, now compiled exclusively with gcc.
 *
 * Revision 1.7  2001/09/07 15:19:36  pmw
 * forced to remove all c++ structures from client lib.  should have done
 * this long ago but I'm a c++ head so I keep forgetting.  Need to go back and
 * compile client libs with only gcc.
 *
 * Revision 1.6  2001/09/06 15:03:33  pmw
 * fixed attribute list handling using string encoding
 *
 * Revision 1.5  2001/09/03 20:09:10  pmw
 * changes to deal with string-marshaled attr lists
 *
 * Revision 1.4  2001/08/30 21:02:37  pmw
 * partial changes for string-marshaled attr lists
 *
 * Revision 1.3  2001/07/02 04:02:03  pmw
 * interim checkin for changes to make Contexts also support events.  compiles
 * but tests probably do not run.
 *
 * Revision 1.2  2001/06/26 19:49:32  pmw
 * fixed several small things on last bughunt.  pdsd.cc now does gen_pthread_init().
 *
 * Revision 1.1.1.1  2000/12/13 22:14:20  pmw
 * imported sources
 *
 * Revision 1.19  2000/12/05 00:24:19  pmw
 * Added server_shutdown message
 *
 * Revision 1.18  2000/11/30 01:47:55  pmw
 * rearranged include files to fix inappropriate inclusions
 *
 * Revision 1.17  2000/11/29 04:37:36  pmw
 * changed channel-handling again, Entity now performs most of its own
 * change event sends
 *
 * Revision 1.16  2000/11/26 23:20:24  pmw
 * fixed missing byte-order conversion in pds_service_open
 *
 * Revision 1.15  2000/11/26 07:35:34  pmw
 * missing include of <netinet/in.h> broke Linux build
 *
 * Revision 1.14  2000/11/21 21:35:25  pmw
 * Domain change events now work correctly.  moved event transmission
 * responsibilities to Proactive class.
 *
 * Revision 1.13  2000/11/20 05:08:52  pmw
 * added pds_open_domain call.  semantics are such that a domain id is always
 * returned, either because one matching the criteria was found or a new one
 * was created because no matches were found.
 *
 * Revision 1.12  2000/11/19 23:28:27  pmw
 * pds_server now supports bulk load of data with -f command-line option.  Also added
 * command message so bulk loads can happen on-line.
 *
 * Revision 1.11  2000/10/30 22:44:14  pmw
 * basic test program works after lots of changes.
 * ** server still leaks memory; entries are not removed
 * from global_object_map when contexts or entities are
 * destroyed.
 *
 * Revision 1.10  2000/10/25 21:44:44  pmw
 * Compiles after major revisions to support multiple WP services on
 * client-side.  This should eventually allow a client to use services by
 * linking in the library directly or by communicating using CMRPC.
 *
 * Revision 1.9  2000/04/10 20:00:07  pmw
 * completed port to CM from DataExchange
 *
 * Revision 1.8  2000/01/06 04:59:03  pmw
 * changed id scheme so java side is simpler
 *
 * Revision 1.7  1999/12/21 18:47:51  pmw
 * Too many changes - really shouldn't have done this many
 *
 * Revision 1.6  1999/12/10 01:21:19  pmw
 * new directory structure for test programs
 *
 * Revision 1.5  1999/12/09 22:35:11  pmw
 * added client_priv header, RCS comments
 *
 *
 */
