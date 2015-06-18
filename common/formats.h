#ifndef _FORMATS_H_
#define _FORMATS_H_

/*
 *
 *  $Id: formats.h,v 1.17 2008-01-01 00:38:17 pmw Exp $
 *
 */

#include "ffs.h"
#include "evpath.h"
#include "agent/pdsdefs.h"

/*
 *  trace types
 */
extern const unsigned long pdsengineVerbose;
extern const unsigned long pdsagentVerbose;
extern const unsigned long pdsdVerbose;
extern const unsigned long pdsDataVerbose;

extern int
pdsTrace_on (const unsigned long trace_type);
extern void
pdsTrace_out(const unsigned long trace_type, char const *format, ...);
#define ID_TRACE_OUT(T,STR,X) pdsTrace_out ((T), "%s [%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c]", (STR), (X)[0],(X)[1],(X)[2],(X)[3],(X)[4],(X)[5],(X)[6],(X)[7],(X)[8],(X)[9],(X)[10],(X)[11],(X)[12],(X)[13],(X)[14],(X)[15],(X)[16],(X)[17],(X)[18],(X)[19],(X)[20],(X)[21],(X)[22],(X)[23],(X)[24],(X)[25],(X)[26],(X)[27],(X)[28],(X)[29],(X)[30],(X)[31]);

/*
 * Define source handle identifiers for different data types on the server
 * and client
 */

#define ENTITY_CREATE_DESTROY 1
#define ENTITY_BIND_UNBIND 2
#define ENTITY_DATA_CHANGE 3
#define ENTITY_ATTR_CHANGE 4
#define CONTEXT_BIND_UNBIND 5
#define CONTEXT_CREATE_DESTROY 6
#define DOMAIN_CHANGE 7



/*
 *  CM_RPC rpc name strings
 */

#define OPEN_SERVICE_RPC_NAME            "open service"
#define BIND_ENTITY_RPC_NAME             "bind entity"
#define RESOLVE_NAME_RPC_NAME            "resolve name"
#define CREATE_DOMAIN_RPC_NAME           "create domain"
#define OPEN_DOMAIN_RPC_NAME             "open domain"
#define REMOVE_DOMAIN_RPC_NAME           "remove domain"
#define SEND_DOMAIN_ATTRIBUTES_RPC_NAME  "send domain attributes"
#define CREATE_CONTEXT_RPC_NAME          "create context"
#define BIND_CONTEXT_RPC_NAME      "bind context"
#define UNBIND_CONTEXT_BY_ID_RPC_NAME    "unbind context"
#define REMOVE_ENTITY_RPC_NAME           "remove entity"
#define UNBIND_ENTITY_RPC_NAME           "unbind entity"
#define GET_ROOT_CONTEXT_RPC_NAME        "get root context"
#define REMOVE_CONTEXT_RPC_NAME          "remove context"
#define CREATE_ENTITY_RPC_NAME           "create entity"
#define EVPATH_OP_RPC_NAME               "evpath operation"
#define SET_ENTITY_ATTRIBUTES_RPC_NAME   "set entity attributes"
#define GET_ENTITY_ATTRIBUTES_RPC_NAME   "get entity attributes"
#define SET_ENTITY_DATA_RPC_NAME         "set entity data"
#define GET_ENTITY_DATA_RPC_NAME         "get entity data"
#define SET_CONTEXT_ATTRIBUTES_RPC_NAME  "set context attributes"
#define GET_CONTEXT_ATTRIBUTES_RPC_NAME  "get context attributes"
#define FIND_MATCHING_DOMAINS_RPC_NAME   "matching domains"
#define FIND_MATCHING_ENTITIES_RPC_NAME  "matching entities"
#define GET_BINDING_LIST_RPC_NAME        "get binding list"
#define LOAD_FROM_URL_RPC_NAME           "load from URL"
#define SHUTDOWN_SERVER_RPC_NAME         "shutdown server"
#define QUERY_BY_XPATH_RPC_NAME          "xpath query"
#define ADD_AGGREGATE_ENTITY_RPC         "aggregate entity"
/*
 *  EChannel message operation codes
 */
#define OP_GET_DOMAIN_STONE 101
#define OP_GET_ENTITY_STONE 102
#define OP_GET_CONTEXT_STONE 103
#define OP_RESULT_OK 1
#define OP_RESULT_ERROR 0

#define OP_MATCH_DOMAINS 201

#define OP_GET_ATTRS 301
#define OP_SET_ATTRS 302

/*
 * Each structure has an options int associated with it.  options serves
 *  as a bit mask which can have different interpretations depending on
 *  the message.
 */
#define CreateObjIfNotFound 0x1
#define GenericFailure 0x2


typedef struct _open_service_msg_struct
{
  unsigned long addr;
  unsigned short port;
  unsigned long broker;
  int options;
} open_service_msg, *open_service_msg_ptr;

typedef struct _binding_list_msg_struct
{
  char **binding_list;
  int binding_count;
  char* context_name;
  int options;
} binding_list_msg, *binding_list_msg_ptr;

typedef struct _get_binding_list_msg_struct
{
  pds_domain_id_t domain_id;
  pds_context_id_t context_id;
  int options;
} get_binding_list_msg, *get_binding_list_msg_ptr;

typedef struct _matching_domains_msg_struct
{
  pds_domain_id_t *domain_list;
  int domain_count;
  int options;
} matching_domains_msg, *matching_domains_msg_ptr;

typedef struct _matching_entities_msg_struct
{
  pds_entity_id_t *entity_list;
  int entity_count;
  int options;
} matching_entities_msg, *matching_entities_msg_ptr;

typedef struct _create_domain_msg_struct
{
  const char *domain_name;
  const char *domain_type;
  int domain_version;
  const char *application_name;
  int options;
} create_domain_msg, *create_domain_msg_ptr;

typedef struct _create_context_msg_struct
{
  pds_domain_id_t domain_id;
  pds_context_id_t context_id;
  const char *c_name;
  char *encoded_attr_list;
  int options;
} create_context_msg, *create_context_msg_ptr;

typedef struct _bind_context_msg_struct
{
  pds_domain_id_t domain_id;
  pds_context_id_t source_context_id;
  const char *source_context_name;
  const char *link_name;
} bind_context_msg, *bind_context_msg_ptr;

typedef struct _unbind_context_msg_struct
{
  pds_domain_id_t domain_id;
  const char *name;
} unbind_context_msg, *unbind_context_msg_ptr;


typedef struct _remove_domain_msg_struct
{
  pds_domain_id_t domain_id;
  int options;
} remove_domain_msg, *remove_domain_msg_ptr;

typedef struct _return_domain_id_msg_struct
{
  pds_domain_id_t domain_id;
  int options;
} return_domain_id_msg, *return_domain_id_msg_ptr;

typedef struct _return_status_msg_struct
{
  int status;
  int options;
} return_status_msg, *return_status_msg_ptr;

typedef struct _return_entity_id_msg_struct
{
  pds_entity_id_t entity_id;
  const char *fullname;
  int options;
} return_entity_id_msg, *return_entity_id_msg_ptr;

typedef struct _create_entity_msg_struct
{
  pds_domain_id_t domain_id;
  pds_context_id_t context_id;
  const char *name;
  pds_entity_data_t edata;
  char *encoded_attr_list;
  int options;
} create_entity_msg, *create_entity_msg_ptr;

typedef struct _remove_entity_msg_struct
{
  pds_domain_id_t domain_id;
  pds_context_id_t context_id;
  pds_entity_id_t entity_id;
  const char *entity_name;
  int options;
} remove_entity_msg, *remove_entity_msg_ptr;

typedef struct _add_bucket_entity_msg_struct
{
  pds_entity_id_t entity_id;
  char * cod_function;
  unsigned int entity_agg_type;
} add_bucket_entity_msg, * add_bucket_entity_msg_ptr;


typedef struct _entity_attributes_msg_struct
{
  int status;
  int operation;
  pds_domain_id_t domain_id;
  pds_context_id_t context_id;
  pds_entity_id_t entity_id;
  const char *entity_name;
  char *encoded_attr_list;
  int options;
} entity_attributes_msg, *entity_attributes_msg_ptr;



typedef struct _entity_data_msg_struct
{
  int status;
  int operation;
  pds_domain_id_t domain_id;
  pds_context_id_t context_id;
  pds_entity_id_t entity_id;
  const char *entity_name;
  pds_entity_data_t edata;
  int options;
} entity_data_msg, *entity_data_msg_ptr;


typedef struct _return_context_id_msg_struct
{
  pds_context_id_t context_id;
  int options;
} return_context_id_msg, *return_context_id_msg_ptr;

typedef struct _remove_context_msg_struct
{
  pds_domain_id_t domain_id;
  pds_context_id_t context_id;
  const char *c_name;
  int options;
} remove_context_msg, *remove_context_msg_ptr;

typedef struct _bind_entity_msg_struct
{
  pds_domain_id_t domain_id;
  pds_context_id_t context_id;
  pds_context_id_t source_context;
  pds_entity_id_t entity_id;
  const char *name;
  const char *source_name;
  int options;
} bind_entity_msg, *bind_entity_msg_ptr;


typedef struct _unbind_msg_struct
{
  pds_domain_id_t domain_id;
  pds_context_id_t context_id;
  pds_entity_id_t entity_id;
  const char *name;
  int options;
} unbind_msg, *unbind_msg_ptr;


typedef struct _resolve_name_msg_struct
{
  pds_domain_id_t domain_id;
  pds_context_id_t context_id;
  const char *name;
  int options;
} resolve_name_msg, *resolve_name_msg_ptr;

typedef struct _resolution_result_msg_struct
{
  int status;
  int level;
  pds_entity_id_t entity_id;
  pds_context_id_t context_id;
  int options;
} resolution_result_msg, *resolution_result_msg_ptr;
  

typedef struct _domain_attributes_msg_struct
{
  int operation;
  pds_domain_id_t domain_id;
  char *encoded_attr_list;
  int options;
} domain_attributes_msg, *domain_attributes_msg_ptr;

typedef struct _context_attributes_msg_struct
{
  int operation;
  pds_domain_id_t domain_id;
  pds_context_id_t context_id;
  const char *name;
  char *encoded_attr_list;
  int options;
} context_attributes_msg, *context_attributes_msg_ptr;

typedef struct _get_root_context_msg_struct
{
  pds_domain_id_t domain_id;
  int options;
} get_root_context_msg, *get_root_context_msg_ptr;

typedef struct _evpath_op_msg_struct
{
  int operation;
  int stone;
  char *contact_attrs;
  pds_domain_id_t domain_id;
  const char* name;
  pds_context_id_t context_id;
  int options;
} evpath_op_msg, *evpath_op_msg_ptr;

typedef struct _load_from_URL_msg_struct
{
  const char *url;
  pds_domain_id_t domain_id;
  pds_context_id_t context_id;
  int options;
} load_from_URL_msg, *load_from_URL_msg_ptr;

typedef struct _shutdown_server_msg_struct
{
  int options;
} shutdown_server_msg, *shutdown_server_msg_ptr;

typedef struct _xpath_query_request_msg_struct
{
  char* xpath;
  pds_domain_id_t domain_id;
} xpath_query_request_msg, *xpath_query_request_msg_ptr;

typedef struct _xpath_query_results_msg_struct
{
  char **name_list;
  int name_count;
} xpath_query_results_msg, *xpath_query_results_msg_ptr;

extern FMField pds_entity_id_t_flds[];
extern FMField pds_context_id_t_flds[];
extern FMField pds_domain_id_t_flds[];
extern FMField pds_entity_data_t_flds[];
extern FMStructDescRec pds_entity_data_t_formats[];

extern FMField unbind_msg_flds[];
extern FMStructDescRec unbind_msg_formats[];
extern FMField open_service_msg_flds[];
extern FMStructDescRec open_service_msg_formats[];
extern FMField bind_entity_msg_flds[];
extern FMStructDescRec bind_entity_msg_formats[];
extern FMField bind_context_msg_flds[];
extern FMStructDescRec bind_context_msg_formats[];
extern FMField unbind_context_msg_flds[];
extern FMStructDescRec unbind_context_msg_formats[];
extern FMField resolution_result_msg_flds[];
extern FMStructDescRec resolution_result_msg_formats[];
extern FMField resolve_name_msg_flds[];
extern FMStructDescRec resolve_name_msg_formats[];
extern FMField create_context_msg_flds[];
extern FMStructDescRec create_context_msg_formats[];
extern FMField create_entity_msg_flds[];
extern FMStructDescRec create_entity_msg_formats[];
extern FMField remove_domain_msg_flds[];
extern FMStructDescRec remove_domain_msg_formats[];
extern FMField remove_entity_msg_flds[];
extern FMStructDescRec remove_entity_msg_formats[];
extern FMField add_bucket_entity_msg_flds[];
extern FMStructDescRec add_bucket_entity_msg_formats[];
extern FMField remove_context_msg_flds[];
extern FMStructDescRec remove_context_msg_formats[];
extern FMField return_status_msg_flds[];
extern FMStructDescRec return_status_msg_formats[];
extern FMField return_domain_id_msg_flds[];
extern FMStructDescRec return_domain_id_msg_formats[];
extern FMField create_domain_msg_flds[];
extern FMStructDescRec create_domain_msg_formats[];
extern FMField return_entity_id_msg_flds[];
extern FMStructDescRec return_entity_id_msg_formats[];
extern FMField return_context_id_msg_flds[];
extern FMStructDescRec return_context_id_msg_formats[];
extern FMField domain_attributes_msg_flds[];
extern FMStructDescRec domain_attributes_msg_formats[];
extern FMField context_attributes_msg_flds[];
extern FMStructDescRec context_attributes_msg_formats[];
extern FMField get_root_context_msg_flds[];
extern FMStructDescRec get_root_context_msg_formats[];
extern FMField return_string_msg_flds[];
extern FMStructDescRec return_string_msg_formats[];
extern FMField evpath_op_msg_flds[];
extern FMStructDescRec evpath_op_msg_formats[];
extern FMField entity_attributes_msg_flds[];
extern FMStructDescRec entity_attributes_msg_formats[];
extern FMField entity_data_msg_flds[];
extern FMStructDescRec entity_data_msg_formats[];
extern FMField matching_domains_msg_flds[];
extern FMStructDescRec matching_domains_msg_formats[];
extern FMField matching_entities_msg_flds[];
extern FMStructDescRec matching_entities_msg_formats[];
extern FMField get_binding_list_msg_flds[];
extern FMStructDescRec get_binding_list_msg_formats[];
extern FMField binding_list_msg_flds[];
extern FMStructDescRec binding_list_msg_formats[];
extern FMField load_from_URL_msg_flds[];
extern FMStructDescRec load_from_URL_msg_formats[];
extern FMField shutdown_server_msg_flds[];
extern FMStructDescRec shutdown_server_msg_formats[];
extern FMField entity_change_event_flds[];
extern FMStructDescRec entity_change_event_formats[];
extern FMField context_change_event_flds[];
extern FMStructDescRec context_change_event_formats[];
extern FMField domain_change_event_flds[];
extern FMStructDescRec domain_change_event_formats[];
extern FMField xpath_query_results_msg_flds[];
extern FMStructDescRec xpath_query_results_msg_formats[];
extern FMField xpath_query_request_msg_flds[];
extern FMStructDescRec xpath_query_request_msg_formats[];

extern FMField entity_data_change_ntf_flds[];
extern FMStructDescRec entity_data_change_ntf_formats[];
extern FMField entity_u_bind_change_ntf_flds[];
extern FMStructDescRec entity_u_bind_change_ntf_formats[];
extern FMField entity_exist_change_ntf_flds[];
extern FMStructDescRec entity_exist_change_ntf_formats[];

extern FMField context_u_bind_change_ntf_flds[];
extern FMStructDescRec context_u_bind_change_ntf_formats[];
extern FMField context_exist_change_ntf_flds[];
extern FMStructDescRec context_exist_change_ntf_formats[];

extern handler_tag_FMStructDescRec entity_DescRecs[];
extern handler_tag_FMStructDescRec context_DescRecs[];
extern handler_tag_FMStructDescRec domain_DescRecs[];


/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.15  2007/12/04 21:06:16  pmw
 *  Merge changes from evpath branch.  Also probably includes
 *  Darwin fixes.  Compiles on isleroyale and marquesas, I think
 *  I'm misconfigured somehow for building on gondor.
 *
 *  Revision 1.14.2.1  2007/01/06 08:53:19  pmw
 *  *** empty log message ***
 *
 *  Revision 1.14  2006/10/11 02:01:25  chaos
 *  Change so we don't have include of evpath.h in cmrpc.h
 *
 *  Revision 1.13  2005/02/24 19:28:31  pmw
 *  Everything should now be using string encoding for attr lists.  I
 *  removed all the ifdefs.
 *
 *  Revision 1.12  2004/11/06 22:23:33  mansour
 *  Added a bind_context_by_name API. Also fixed the XML logic to use the correct bind name.
 *
 *  Revision 1.11  2004/11/03 06:59:36  mansour
 *  Added APIs for context binding. The XML dump routine expands bounded contexts in
 *  to a full tree and the load routine loads it as separate contexts.
 *
 *  Revision 1.10  2004/10/14 21:17:30  pmw
 *  Changes to support the CMrpc renaming.
 *
 *  Revision 1.9  2004/04/27 20:59:24  pmw
 *  Added RPC for open_service to simplify service_from_ID handling on the
 *  client.  The way I set that up at first was a little braindead, and failed
 *  when one side was weakly hostnamed (via DHCP, for example).  The new RPC
 *  asks the server what address/port combination it intends to use and saves
 *  those.
 *
 *  Revision 1.8  2003/10/31 05:38:55  mansour
 *  Added xpath query support
 *
 *  Revision 1.7  2003/10/21 16:03:07  eisen
 *  Changes to avoid warnings and errors and generally make this compile with
 *  SUNWspro7 CC with the -library=stlport4 argument.  Using this correctly
 *  requires setting CXX=CC, CXXFLAGS=-library=stlport4,
 *  CXXCPP="CC -E -library=stlport4".  Chaos is setup to do this now.
 *
 *  Revision 1.6  2003/01/24 19:31:27  eisen
 *  Some tweaks to make pds compile with Forte CC (version 7 with the
 *  -library=stlport4 argument).
 *
 *  Revision 1.5  2002/10/30 04:42:00  pmw
 *  typos...
 *
 *  Revision 1.4  2002/10/30 03:52:59  pmw
 *  added parameter to get_binding_list functions to optionally get only entities
 *  or contexts
 *
 *  Revision 1.3  2002/08/07 18:51:44  eisen
 *  Widespread changes to modify how attribute handling works.  Stop using
 *  attr_atom_from_string() to get atom values.  Instead, those are set by the
 *  application.  Stop using get_attr() (api has changes).  Stop using
 *  xmit_object and start using the ATL-supplied encode/decode routines.
 *
 *  Revision 1.2  2001/10/15 19:58:00  pmw
 *  added new trace framework, shamelessly copied from Greg
 *
 *  Revision 1.1  2001/10/09 21:37:54  pmw
 *  configuration changes
 *
 *  Revision 1.9  2001/10/07 22:19:15  pmw
 *  Added domain_change event structures so that Proactive class is always correctly
 *  initialized.
 *
 *  Revision 1.8  2001/09/23 22:55:53  pmw
 *  Contexts now support attributes of their own.  Practically, this is supported
 *  by having ContextBindable host the attribute list instead of Entity/Context.  Also
 *  added the usual functions on the client side and handlers for getting/setting
 *  this attr list for Contexts.
 *
 *  Revision 1.7  2001/09/10 03:55:43  pmw
 *  client lib is now libpdsagent, and is compiled entirely with gcc.  libpdsutil no
 *  longer exists.
 *
 *  Revision 1.6  2001/09/03 20:09:10  pmw
 *  changes to deal with string-marshaled attr lists
 *
 *  Revision 1.5  2001/08/30 21:02:37  pmw
 *  partial changes for string-marshaled attr lists
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
 *  Revision 1.1.1.1  2000/12/13 22:14:21  pmw
 *  imported sources
 *
 *  Revision 1.12  2000/12/05 00:24:19  pmw
 *  Added server_shutdown message
 *
 *  Revision 1.11  2000/11/21 00:23:51  pmw
 *  Since I didn't have enough to do, a little code reorganization to keep
 *  things from getting dramatically out of hand.  Fixed a couple of
 *  bugs-in-waiting.
 *
 *  Revision 1.10  2000/11/20 05:08:53  pmw
 *  added pds_open_domain call.  semantics are such that a domain id is always
 *  returned, either because one matching the criteria was found or a new one
 *  was created because no matches were found.
 *
 *  Revision 1.9  2000/11/19 23:28:29  pmw
 *  pds_server now supports bulk load of data with -f command-line option.  Also added
 *  command message so bulk loads can happen on-line.
 *
 *  Revision 1.8  2000/05/15 21:15:48  pmw
 *  minor changes
 *
 *  Revision 1.7  2000/04/10 20:00:11  pmw
 *  completed port to CM from DataExchange
 *
 *
 */

#endif 





