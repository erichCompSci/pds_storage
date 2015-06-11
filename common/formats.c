/*
 *
 *  $Id: formats.c,v 1.11 2008-01-01 00:38:16 pmw Exp $
 *
 */
#include "config.h"

#include "atl.h"

#include "formats.h"

#define FMFieldList_Terminator {(char*)0, (char*)0, 0, 0}
#define FMStructDescList_Terminator {NULL, NULL}

FMField pds_entity_id_t_flds[] =
{
  { "id", "char[32]", sizeof (unsigned char),
    FMOffset (pds_entity_id_t_ptr, id) },
  {(char*) 0, (char *) 0, 0, 0}
};

FMField pds_domain_id_t_flds[] =
{
  { "id", "char[32]", sizeof (unsigned char),
    FMOffset (pds_domain_id_t_ptr, id) },
  {(char*) 0, (char *) 0, 0, 0}
};

FMField pds_context_id_t_flds[] =
{
  { "id", "char[32]", sizeof (unsigned char),
    FMOffset (pds_context_id_t_ptr, id) },
  {(char*) 0, (char *) 0, 0, 0}
};



FMField open_service_msg_flds[] = 
  {
    { "addr", "integer", sizeof (unsigned long),
      FMOffset (open_service_msg_ptr, addr)},
    { "port", "integer", sizeof (unsigned short),
      FMOffset (open_service_msg_ptr, port)},
    { "broker", "integer", sizeof (unsigned long),
      FMOffset (open_service_msg_ptr, broker)},
    { "options", "integer", sizeof (int),
      FMOffset (open_service_msg_ptr, options)},
    {(char *) 0, (char *) 0, 0, 0}
  };

FMStructDescRec open_service_msg_formats[] = 
  {
    { "open_service_msg", open_service_msg_flds, sizeof(open_service_msg),
      NULL },
    FMStructDescList_Terminator
  };

FMField binding_list_msg_flds[] =
{
  { "binding_list", "string[binding_count]", sizeof (char*),
    FMOffset (binding_list_msg_ptr, binding_list)},
  { "binding_count", "integer", sizeof (int),
    FMOffset (binding_list_msg_ptr, binding_count) },
  { "context_name", "string", sizeof (char*),
    FMOffset (binding_list_msg_ptr, context_name) },
  { "options", "integer", sizeof (int),
    FMOffset (binding_list_msg_ptr, options) },
  {(char *) 0, (char *) 0, 0, 0}
};

FMStructDescRec binding_list_msg_formats[] = 
  {
    { "binding_list_msg", binding_list_msg_flds, sizeof(binding_list_msg),
      NULL },
    FMStructDescList_Terminator
  };

FMField xpath_query_results_msg_flds[] =
{
  { "name_list", "string[name_count]", sizeof (char*),
    FMOffset (xpath_query_results_msg_ptr, name_list)},
  { "name_count", "integer", sizeof (int),
    FMOffset (xpath_query_results_msg_ptr, name_count) },
  {(char *) 0, (char *) 0, 0, 0}
};

FMStructDescRec xpath_query_results_msg_formats[] = 
  {
    { "xpath_query_results_msg", xpath_query_results_msg_flds, sizeof(xpath_query_results_msg),
      NULL },
    FMStructDescList_Terminator
  };

FMField xpath_query_request_msg_flds[] =
{
  { "xpath", "string", sizeof (char*),
    FMOffset (xpath_query_request_msg_ptr, xpath)},
  { "domain_id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (xpath_query_request_msg_ptr, domain_id)},
  {(char *) 0, (char *) 0, 0, 0}
};

FMStructDescRec xpath_query_request_msg_formats[] = 
  {
    { "xpath_query_request_msg", xpath_query_request_msg_flds, sizeof(xpath_query_request_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    FMStructDescList_Terminator
  };
/* Erich's code */
FMField entity_data_change_ntf_flds[] = 
  {
    { "entity_id", "pds_entity_id_t", sizeof(pds_entity_id_t), FMOffset(pds_entity_data_change_ntf_ptr, entity_id) },
    { "entity_data", "pds_entity_data_t", sizeof(pds_entity_data_t), FMOffset(pds_entity_data_change_ntf_ptr, entity_data) },
    { NULL, NULL, 0, 0 }
  };

FMStructDescRec entity_data_change_ntf_formats[] =
  {
    { "entity_data_change_event", entity_data_change_ntf_flds, sizeof(pds_entity_data_change_ntf), NULL },
    { "pds_entity_id_t", pds_entity_id_t_flds, sizeof(pds_entity_id_t), NULL },
    { "pds_entity_data_t", pds_entity_data_t_flds, sizeof(pds_entity_data_t), NULL },
    { NULL, NULL }
  };

FMField entity_u_bind_change_ntf_flds[] = 
  {
    { "type", "integer", sizeof(int), FMOffset(pds_entity_u_bind_change_ntf_ptr, type) },
    { "entity_id", "pds_entity_id_t", sizeof(pds_entity_id_t), FMOffset(pds_entity_u_bind_change_ntf_ptr, entity_id) },
    {NULL, NULL, 0, 0 }
  };

FMStructDescRec entity_u_bind_change_ntf_formats[] =
  {
    { "entity_u_bind_change_event", entity_u_bind_change_ntf_flds, sizeof(pds_entity_u_bind_change_ntf), NULL },
    { "pds_entity_id_t", pds_entity_id_t_flds, sizeof(pds_entity_id_t), NULL },
    { NULL, NULL }
  };

FMField entity_exist_change_ntf_flds[] = 
  {
    { "type", "integer", sizeof(int), FMOffset(pds_entity_exist_change_ntf_ptr, type) },
    { "entity_id", "pds_entity_id_t", sizeof(pds_entity_id_t), FMOffset(pds_entity_exist_change_ntf_ptr, entity_id) },
    { "entity_data", "pds_entity_data_t", sizeof(pds_entity_data_t), FMOffset(pds_entity_exist_change_ntf_ptr, entity_data) },
    { NULL, NULL, 0, 0 }
  };

FMStructDescRec entity_exist_change_ntf_formats[] =
  {
    { "entity_exist_change_event", entity_exist_change_ntf_flds, sizeof(pds_entity_exist_change_ntf), NULL },
    { "pds_entity_id_t", pds_entity_id_t_flds, sizeof(pds_entity_id_t), NULL },
    { "pds_entity_data_t", pds_entity_data_t_flds, sizeof(pds_entity_data_t), NULL },
    { NULL, NULL }
  };

FMField context_u_bind_change_ntf_flds[] =
  {
    { "type", "integer", sizeof(int), FMOffset(pds_context_u_bind_change_ntf_ptr, type)  },
    { "fullname", "string", sizeof(char *), FMOffset(pds_context_u_bind_change_ntf_ptr, fullname) }, 
    { "context_id", "pds_context_id_t", sizeof(pds_context_id_t), FMOffset(pds_context_u_bind_change_ntf_ptr, context_id) },
    { NULL, NULL, 0, 0 }
  };

FMStructDescRec context_u_bind_change_ntf_formats[] =
  {
    { "context_u_bind_change_event", context_u_bind_change_ntf_flds, sizeof(pds_context_u_bind_change_ntf), NULL },
    { "pds_context_id_t", pds_context_id_t_flds, sizeof(pds_context_id_t), NULL },
    { NULL, NULL }
  };

FMField context_exist_change_ntf_flds[] =
  {
    { "type", "integer", sizeof(int), FMOffset(pds_context_exist_change_ntf_ptr, type)  },
    { "fullname", "string", sizeof(char *), FMOffset(pds_context_exist_change_ntf_ptr, fullname) },
    { "context_id", "pds_context_id_t", sizeof(pds_context_id_t), FMOffset(pds_context_exist_change_ntf_ptr, context_id) },
    { NULL, NULL, 0, 0 }
  };

FMStructDescRec context_exist_change_ntf_formats[] =
  {
    { "context_u_bind_change_event", context_u_bind_change_ntf_flds, sizeof(pds_context_exist_change_ntf), NULL },
    { "pds_context_id_t", pds_context_id_t_flds, sizeof(pds_context_id_t), NULL },
    { NULL, NULL }
  };


/* Erich's code */

FMField get_binding_list_msg_flds[] = 
{
  { "domain id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (get_binding_list_msg_ptr, domain_id)},
  { "context id", "pds_context_id_t", sizeof (pds_context_id_t),
    FMOffset (get_binding_list_msg_ptr, context_id) },
  { "options", "integer", sizeof (int),
    FMOffset (get_binding_list_msg_ptr, options) },
  
    {(char*) 0, (char*) 0, 0, 0 }
};

FMStructDescRec get_binding_list_msg_formats[] = 
  {
    { "get_binding_list_msg", get_binding_list_msg_flds, sizeof(get_binding_list_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    { "pds_context_id_t", pds_context_id_t_flds, sizeof(pds_context_id_t),
      NULL },
    FMStructDescList_Terminator
  };

FMField pds_entity_data_t_flds[] =
{
  { "data", "char[data_size]", sizeof (unsigned char),
    FMOffset (pds_entity_data_t_ptr, data) },
  { "data_size", "integer", sizeof (int),
    FMOffset (pds_entity_data_t_ptr, data_size) },
  { "data_type", "integer", sizeof (int),
    FMOffset (pds_entity_data_t_ptr, data_type) },

  {(char*) 0, (char*) 0, 0, 0 }
};




FMField matching_domains_msg_flds[] =
{
  { "domain_list", "pds_domain_id_t[domain_count]", sizeof (pds_domain_id_t),
    FMOffset (matching_domains_msg_ptr, domain_list)},
  { "domain_count", "integer", sizeof (int),
    FMOffset (matching_domains_msg_ptr, domain_count) },
  { "options", "integer", sizeof (int),
    FMOffset (matching_domains_msg_ptr, options) },
  
  {(char *) 0, (char *) 0, 0, 0}
};

FMStructDescRec matching_domains_msg_formats[] = 
  {
    { "matching_domains_msg", matching_domains_msg_flds, sizeof(matching_domains_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    FMStructDescList_Terminator
  };

FMField matching_entities_msg_flds[] =
{
  { "entity_list", "pds_entity_id_t[entity_count]", sizeof (pds_entity_id_t),
    FMOffset (matching_entities_msg_ptr, entity_list)},
  { "entity_count", "integer", sizeof (int),
    FMOffset (matching_entities_msg_ptr, entity_count) },
  { "options", "integer", sizeof (int),
    FMOffset (matching_entities_msg_ptr, options) },

  {(char *) 0, (char *) 0, 0, 0}
};

FMStructDescRec matching_entities_msg_formats[] = 
  {
    { "matching_entities_msg", matching_entities_msg_flds, sizeof(matching_entities_msg),
      NULL },
    { "pds_entity_id_t", pds_entity_id_t_flds, sizeof(pds_entity_id_t),
      NULL },
    FMStructDescList_Terminator
  };

FMField return_domain_id_msg_flds[] =
{
  { "domain id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (return_domain_id_msg_ptr, domain_id)},
  { "options", "integer", sizeof (int),
    FMOffset (return_domain_id_msg_ptr, options) },
  
  {(char *) 0, (char *) 0, 0, 0}
};

FMStructDescRec return_domain_id_msg_formats[] = 
  {
    { "return_domain_id_msg", return_domain_id_msg_flds, sizeof(return_domain_id_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    FMStructDescList_Terminator
  };


FMField return_entity_id_msg_flds[] =
{
  { "entity_id", "pds_entity_id_t", sizeof (pds_entity_id_t),
      FMOffset (return_entity_id_msg_ptr, entity_id)},
  { "fullname", "string", sizeof (char*),
    FMOffset (return_entity_id_msg_ptr, fullname)},
  { "options", "integer", sizeof (int),
    FMOffset (return_entity_id_msg_ptr, options) },

  {(char *) 0, (char *) 0, 0, 0}
};

FMStructDescRec return_entity_id_msg_formats[] = 
  {
    { "return_entity_id_msg", return_entity_id_msg_flds, sizeof(return_entity_id_msg),
      NULL },
    { "pds_entity_id_t", pds_entity_id_t_flds, sizeof(pds_entity_id_t),
      NULL },
    FMStructDescList_Terminator
  };


FMField return_context_id_msg_flds[] =
{
  { "context id", "pds_context_id_t", sizeof (pds_context_id_t),
    FMOffset (return_context_id_msg_ptr, context_id)},
  { "options", "integer", sizeof (int),
    FMOffset (return_context_id_msg_ptr, options) },

  {(char *) 0, (char *) 0, 0, 0}
};

FMStructDescRec return_context_id_msg_formats[] = 
  {
    { "return_context_id_msg", return_context_id_msg_flds, sizeof(return_context_id_msg),
      NULL },
    { "pds_context_id_t", pds_context_id_t_flds, sizeof(pds_context_id_t),
      NULL },
    FMStructDescList_Terminator
  };


FMField return_status_msg_flds[] =
{
  { "status", "integer", sizeof (int),
    FMOffset (return_status_msg_ptr, status)},
  { "options", "integer", sizeof (int),
    FMOffset (return_status_msg_ptr, options) },
  
  {(char *) 0, (char *) 0, 0, 0}
};

FMStructDescRec return_status_msg_formats[] = 
  {
    { "return_status_msg", return_status_msg_flds, sizeof(return_status_msg),
      NULL },
    FMStructDescList_Terminator
  };

FMField create_domain_msg_flds[] =
{
  { "domain name", "string", sizeof (char*),
    FMOffset (create_domain_msg_ptr, domain_name)},
  { "domain type", "string", sizeof (char*),
    FMOffset (create_domain_msg_ptr, domain_type)},
  { "domain version", "integer", sizeof (int),
    FMOffset (create_domain_msg_ptr, domain_version)},
  { "application name", "string", sizeof (char*),
    FMOffset (create_domain_msg_ptr, application_name)},
  { "options", "integer", sizeof (int),
    FMOffset (create_domain_msg_ptr, options) },

  {(char *) 0, (char *) 0, 0, 0}
};

FMStructDescRec create_domain_msg_formats[] = 
  {
    { "create_domain_msg", create_domain_msg_flds, sizeof(create_domain_msg),
      NULL },
    FMStructDescList_Terminator
  };


FMField get_root_context_msg_flds[] =
{
  { "domain id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (get_root_context_msg_ptr, domain_id) },
  { "options", "integer", sizeof (int),
    FMOffset (get_root_context_msg_ptr, options) },

  {(char *) 0, (char *) 0, 0, 0}
};

FMStructDescRec get_root_context_msg_formats[] = 
  {
    { "get_root_context_msg", get_root_context_msg_flds, sizeof(get_root_context_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    FMStructDescList_Terminator
  };


FMField remove_domain_msg_flds[] = 
{
  { "domain id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (remove_domain_msg_ptr, domain_id) },
  { "options", "integer", sizeof (int),
    FMOffset (remove_domain_msg_ptr, options) },

  {(char *) 0, (char *) 0, 0, 0}
};

FMStructDescRec remove_domain_msg_formats[] = 
  {
    { "remove_domain_msg", remove_domain_msg_flds, sizeof(remove_domain_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    FMStructDescList_Terminator
  };

FMField remove_entity_msg_flds[] = 
{
  { "domain id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (remove_entity_msg_ptr, domain_id) },
  { "context_id", "pds_context_id_t", sizeof (pds_context_id_t),
    FMOffset (remove_entity_msg_ptr, context_id) },
  { "entity_id", "pds_entity_id_t", sizeof (pds_entity_id_t),
    FMOffset (remove_entity_msg_ptr, entity_id) },
  { "entity_name", "string", sizeof (char*),
    FMOffset (remove_entity_msg_ptr, entity_name) },
  { "options", "integer", sizeof (int),
    FMOffset (remove_entity_msg_ptr, options) },

  {(char *) 0, (char *) 0, 0, 0}
};

FMStructDescRec remove_entity_msg_formats[] = 
  {
    { "remove_entity_msg", remove_entity_msg_flds, sizeof(remove_entity_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    { "pds_entity_id_t", pds_entity_id_t_flds, sizeof(pds_entity_id_t),
      NULL },
    { "pds_context_id_t", pds_context_id_t_flds, sizeof(pds_context_id_t),
      NULL },
    FMStructDescList_Terminator
  };


FMField remove_context_msg_flds[] = 
{
  { "domain id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (remove_context_msg_ptr, domain_id) },
  { "context id", "pds_context_id_t", sizeof (pds_context_id_t),
    FMOffset (remove_context_msg_ptr, context_id) },
  { "name", "string", sizeof(char*), 
   FMOffset (remove_context_msg_ptr, c_name)},
  { "options", "integer", sizeof (int),
    FMOffset (remove_context_msg_ptr, options) },

  {(char *) 0, (char *) 0, 0, 0}
};

FMStructDescRec remove_context_msg_formats[] = 
  {
    { "remove_context_msg", remove_context_msg_flds, sizeof(remove_context_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    { "pds_context_id_t", pds_context_id_t_flds, sizeof(pds_context_id_t),
      NULL },
    FMStructDescList_Terminator
  };

FMField create_context_msg_flds[] = 
{
  { "domain id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (create_context_msg_ptr, domain_id) },
  { "context id", "pds_context_id_t", sizeof (pds_context_id_t),
    FMOffset (create_context_msg_ptr, context_id) },
  { "name", "string", sizeof(char*), 
   FMOffset (create_context_msg_ptr, c_name)},
  { "encoded_attr_list", "string", sizeof (char*),
    FMOffset (create_context_msg_ptr, encoded_attr_list) },
  { "options", "integer", sizeof (int),
    FMOffset (create_context_msg_ptr, options) },

  {(char *) 0, (char *) 0, 0, 0}
};

FMStructDescRec create_context_msg_formats[] = 
  {
    { "create_context_msg", create_context_msg_flds, sizeof(create_context_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    { "pds_context_id_t", pds_context_id_t_flds, sizeof(pds_context_id_t),
      NULL },
    FMStructDescList_Terminator
  };


FMField create_entity_msg_flds[] = 
{
  { "domain id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (create_entity_msg_ptr, domain_id) },
  { "context id", "pds_context_id_t", sizeof (pds_context_id_t),
    FMOffset (create_entity_msg_ptr, context_id) },
  { "name", "string", sizeof(char*), 
   FMOffset (create_entity_msg_ptr, name)},
  { "edata", "pds_entity_data_t", sizeof(pds_entity_data_t), 
   FMOffset (create_entity_msg_ptr, edata)},
  { "encoded_attr_list", "string", sizeof (char*),
    FMOffset (create_entity_msg_ptr, encoded_attr_list) },
  { "options", "integer", sizeof (int),
    FMOffset (create_entity_msg_ptr, options) },
  
  {(char *) 0, (char *) 0, 0, 0}
};

FMStructDescRec create_entity_msg_formats[] = 
  {
    { "create_entity_msg", create_entity_msg_flds, sizeof(create_entity_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    { "pds_context_id_t", pds_context_id_t_flds, sizeof(pds_context_id_t),
      NULL },
    { "pds_entity_data_t", pds_entity_data_t_flds, sizeof(pds_entity_data_t),
      NULL },
    FMStructDescList_Terminator
  };


FMField entity_attributes_msg_flds[] = 
{
  { "status", "integer", sizeof (int),
    FMOffset (entity_attributes_msg_ptr, status) },
  { "operation", "integer", sizeof (int),
    FMOffset (entity_attributes_msg_ptr, operation) },
  { "domain id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (entity_attributes_msg_ptr, domain_id) },
  { "context id", "pds_context_id_t", sizeof (pds_context_id_t),
    FMOffset (entity_attributes_msg_ptr, context_id) },
  { "entity id", "pds_entity_id_t", sizeof (pds_entity_id_t),
      FMOffset (entity_attributes_msg_ptr, entity_id) },
  { "entity_name", "string", sizeof (char*),
    FMOffset (entity_attributes_msg_ptr, entity_name) },
  { "encoded_attr_list", "string", sizeof (char*),
    FMOffset (entity_attributes_msg_ptr, encoded_attr_list) },
  { "options", "integer", sizeof (int),
    FMOffset (entity_attributes_msg_ptr, options) },
  
  {(char *) 0, (char *) 0, 0, 0}
};

FMStructDescRec entity_attributes_msg_formats[] = 
  {
    { "entity_attributes_msg", entity_attributes_msg_flds, sizeof(entity_attributes_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    { "pds_entity_id_t", pds_entity_id_t_flds, sizeof(pds_entity_id_t),
      NULL },
    { "pds_context_id_t", pds_context_id_t_flds, sizeof(pds_context_id_t),
      NULL },
    FMStructDescList_Terminator
  };


FMField entity_data_msg_flds[] = 
{
  { "status", "integer", sizeof (int),
    FMOffset (entity_data_msg_ptr, status) },
  { "operation", "integer", sizeof (int),
    FMOffset (entity_data_msg_ptr, operation) },
  { "domain id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (entity_data_msg_ptr, domain_id) },
  { "context id", "pds_context_id_t", sizeof (pds_context_id_t),
    FMOffset (entity_data_msg_ptr, context_id) },
  { "entity id", "pds_entity_id_t", sizeof (pds_entity_id_t),
    FMOffset (entity_data_msg_ptr, entity_id) },
  { "entity_name", "string", sizeof (char*),
    FMOffset (entity_data_msg_ptr, entity_name) },
  { "edata", "pds_entity_data_t", sizeof(pds_entity_data_t), 
   FMOffset (entity_data_msg_ptr, edata)},
  { "options", "integer", sizeof (int),
    FMOffset (entity_data_msg_ptr, options) },
  
  {(char *) 0, (char *) 0, 0, 0}
};
FMStructDescRec entity_data_msg_formats[] = 
  {
    { "entity_data_msg", entity_data_msg_flds, sizeof(entity_data_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    { "pds_entity_id_t", pds_entity_id_t_flds, sizeof(pds_entity_id_t),
      NULL },
    { "pds_context_id_t", pds_context_id_t_flds, sizeof(pds_context_id_t),
      NULL },
    { "pds_entity_data_t", pds_entity_data_t_flds, sizeof(pds_entity_data_t),
      NULL },
    FMStructDescList_Terminator
  };


FMField unbind_msg_flds[] = 
{
  { "domain id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (unbind_msg_ptr, domain_id) },
  { "context id", "pds_context_id_t", sizeof (pds_context_id_t),
    FMOffset (unbind_msg_ptr, context_id) },
  { "entity_id", "pds_entity_id_t", sizeof (pds_entity_id_t),
    FMOffset (unbind_msg_ptr, entity_id) },
  { "name", "string", sizeof (char*),
    FMOffset (unbind_msg_ptr, name) },
  { "options", "integer", sizeof (int),
    FMOffset (unbind_msg_ptr, options) },

  {(char *) 0, (char *) 0, 0, 0}
};
FMStructDescRec unbind_msg_formats[] = 
  {
    { "unbind_msg", unbind_msg_flds, sizeof(unbind_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    { "pds_entity_id_t", pds_entity_id_t_flds, sizeof(pds_entity_id_t),
      NULL },
    { "pds_context_id_t", pds_context_id_t_flds, sizeof(pds_context_id_t),
      NULL },
    FMStructDescList_Terminator
  };


FMField bind_entity_msg_flds[] =
{
  { "domain id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (bind_entity_msg_ptr, domain_id) },
  { "context id", "pds_context_id_t", sizeof (pds_context_id_t),
    FMOffset (bind_entity_msg_ptr, context_id) },
  { "source_context", "pds_context_id_t", sizeof (pds_context_id_t),
    FMOffset (bind_entity_msg_ptr, source_context) },
  { "entity_id", "pds_entity_id_t", sizeof (pds_entity_id_t),
    FMOffset (bind_entity_msg_ptr, entity_id) },
  { "name", "string", sizeof (char*),
    FMOffset (bind_entity_msg_ptr, name) },
  { "source_name", "string", sizeof (char*),
    FMOffset (bind_entity_msg_ptr, source_name) },
  { "options", "integer", sizeof (int),
    FMOffset (bind_entity_msg_ptr, options) },
  
  {(char *) 0, (char *) 0, 0, 0}
};
FMStructDescRec bind_entity_msg_formats[] = 
  {
    { "bind_entity_msg", bind_entity_msg_flds, sizeof(bind_entity_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    { "pds_entity_id_t", pds_entity_id_t_flds, sizeof(pds_entity_id_t),
      NULL },
    { "pds_context_id_t", pds_context_id_t_flds, sizeof(pds_context_id_t),
      NULL },
    FMStructDescList_Terminator
  };


FMField bind_context_msg_flds[] =
{
  { "domain id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (bind_context_msg_ptr, domain_id) },
  { "source_context id", "pds_context_id_t", sizeof (pds_context_id_t),
    FMOffset (bind_context_msg_ptr, source_context_id) },
  { "source_context_name", "string", sizeof (char*),
    FMOffset (bind_context_msg_ptr, source_context_name) },
  { "link_name", "string", sizeof (char*),
    FMOffset (bind_context_msg_ptr, link_name) },
  {(char *) 0, (char *) 0, 0, 0}
};
FMStructDescRec bind_context_msg_formats[] = 
  {
    { "bind_context_msg", bind_context_msg_flds, sizeof(bind_context_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    { "pds_context_id_t", pds_context_id_t_flds, sizeof(pds_context_id_t),
      NULL },
    FMStructDescList_Terminator
  };

  
FMField unbind_context_msg_flds[] =
{
  { "domain id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (unbind_context_msg_ptr, domain_id) },
  { "name", "string", sizeof (char*),
    FMOffset (unbind_context_msg_ptr, name) },
  {(char *) 0, (char *) 0, 0, 0}
};
FMStructDescRec unbind_context_msg_formats[] = 
  {
    { "unbind_context_msg", unbind_context_msg_flds, sizeof(unbind_context_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    FMStructDescList_Terminator
  };


FMField resolution_result_msg_flds[] =
{
  { "status", "integer", sizeof (int),
    FMOffset (resolution_result_msg_ptr, status) },
  { "level", "integer", sizeof (int),
    FMOffset (resolution_result_msg_ptr, level) },
  { "entity_id", "pds_entity_id_t", sizeof (pds_entity_id_t),
    FMOffset (resolution_result_msg_ptr, entity_id) },
  { "context_id", "pds_context_id_t", sizeof (pds_context_id_t),
    FMOffset (resolution_result_msg_ptr, context_id) },
  { "options", "integer", sizeof (int),
    FMOffset (resolution_result_msg_ptr, options) },

  {(char *) 0, (char *) 0, 0, 0}
};
FMStructDescRec resolution_result_msg_formats[] = 
  {
    { "resolution_result_msg", resolution_result_msg_flds, sizeof(resolution_result_msg),
      NULL },
    { "pds_entity_id_t", pds_entity_id_t_flds, sizeof(pds_entity_id_t),
      NULL },
    { "pds_context_id_t", pds_context_id_t_flds, sizeof(pds_context_id_t),
      NULL },
    FMStructDescList_Terminator
  };

FMField resolve_name_msg_flds[] =
{
  { "domain id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (resolve_name_msg_ptr, domain_id) },
  { "context id", "pds_context_id_t", sizeof (pds_context_id_t),
    FMOffset (resolve_name_msg_ptr, context_id) },
  { "name", "string", sizeof (char*),
    FMOffset (resolve_name_msg_ptr, name) },
  { "options", "integer", sizeof (int),
    FMOffset (resolve_name_msg_ptr, options) },
  
  {(char *) 0, (char *) 0, 0, 0}
};
FMStructDescRec resolve_name_msg_formats[] = 
  {
    { "resolve_name_msg", resolve_name_msg_flds, sizeof(resolve_name_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    { "pds_context_id_t", pds_context_id_t_flds, sizeof(pds_context_id_t),
      NULL },
    FMStructDescList_Terminator
  };

FMField domain_attributes_msg_flds[] =
{
  { "operation", "integer", sizeof (int),
    FMOffset (domain_attributes_msg_ptr, operation) },
  { "domain id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (domain_attributes_msg_ptr, domain_id) },
  { "encoded_attr_list", "string", sizeof (char*),
    FMOffset (domain_attributes_msg_ptr, encoded_attr_list) },
  { "options", "integer", sizeof (int),
    FMOffset (domain_attributes_msg_ptr, options) },

  {(char *) 0, (char *) 0, 0, 0}
};
FMStructDescRec domain_attributes_msg_formats[] = 
  {
    { "domain_attributes_msg", domain_attributes_msg_flds, sizeof(domain_attributes_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    FMStructDescList_Terminator
  };


FMField context_attributes_msg_flds[] =
{
  { "operation", "integer", sizeof (int),
    FMOffset (context_attributes_msg_ptr, operation) },
  { "domain id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (context_attributes_msg_ptr, domain_id) },
  { "context id", "pds_context_id_t", sizeof (pds_context_id_t),
    FMOffset (context_attributes_msg_ptr, context_id) },
  { "name", "string", sizeof (char*),
    FMOffset (context_attributes_msg_ptr, name) },
  { "encoded_attr_list", "string", sizeof (char*),
    FMOffset (context_attributes_msg_ptr, encoded_attr_list) },
  { "options", "integer", sizeof (int),
    FMOffset (context_attributes_msg_ptr, options) },

  {(char *) 0, (char *) 0, 0, 0}
};
FMStructDescRec context_attributes_msg_formats[] = 
  {
    { "context_attributes_msg", context_attributes_msg_flds, sizeof(context_attributes_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    { "pds_context_id_t", pds_context_id_t_flds, sizeof(pds_context_id_t),
      NULL },
    FMStructDescList_Terminator
  };

FMField evpath_op_msg_flds[] =
{
  { "operation", "integer", sizeof (int),
    FMOffset (evpath_op_msg_ptr, operation) },
  { "stone", "integer", sizeof (int),
    FMOffset (evpath_op_msg_ptr, stone) },
  { "contact_attrs", "string", sizeof (char*),
    FMOffset (evpath_op_msg_ptr, contact_attrs) },
  { "domain_id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (evpath_op_msg_ptr, domain_id) },
  { "name", "string", sizeof (char*),
    FMOffset (evpath_op_msg_ptr, name) },
  { "context_id", "pds_context_id_t", sizeof (pds_context_id_t),
    FMOffset (evpath_op_msg_ptr, context_id) },
  { "options", "integer", sizeof (int),
    FMOffset (evpath_op_msg_ptr, options) },

  {(char *) 0, (char *) 0, 0, 0}
};
FMStructDescRec evpath_op_msg_formats[] = 
  {
    { "evpath_op_msg", evpath_op_msg_flds, sizeof(evpath_op_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    { "pds_context_id_t", pds_context_id_t_flds, sizeof(pds_context_id_t),
      NULL },
    FMStructDescList_Terminator
  };



FMField load_from_URL_msg_flds[] =
{
  { "url", "string", sizeof (char*),
    FMOffset (load_from_URL_msg_ptr, url) },
  { "domain_id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (load_from_URL_msg_ptr, domain_id) },
  { "context_id", "pds_context_id_t", sizeof (pds_context_id_t),
    FMOffset (load_from_URL_msg_ptr, context_id) },
  { "options", "integer", sizeof (int),
    FMOffset (load_from_URL_msg_ptr, options) },

  {(char *) 0, (char *) 0, 0, 0}
};
FMStructDescRec load_from_URL_msg_formats[] = 
  {
    { "load_from_URL_msg", load_from_URL_msg_flds, sizeof(load_from_URL_msg),
      NULL },
    { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t),
      NULL },
    { "pds_context_id_t", pds_context_id_t_flds, sizeof(pds_context_id_t),
      NULL },
    FMStructDescList_Terminator
  };

FMField shutdown_server_msg_flds[] = 
{
  { "options", "integer", sizeof (int),
    FMOffset (shutdown_server_msg_ptr, options) },

  {(char *) 0, (char *) 0, 0, 0}
};
FMStructDescRec shutdown_server_msg_formats[] = 
  {
    { "shutdown_server_msg", shutdown_server_msg_flds, sizeof(shutdown_server_msg),
      NULL },
    FMStructDescList_Terminator
  };


/*
 *  Entity change event definition
 */
FMField entity_change_event_flds[] =
{
  { "change type", "integer", sizeof (int),
    FMOffset (pds_entity_change_event_ptr, type) },
  { "description", "string", sizeof (char*),
    FMOffset (pds_entity_change_event_ptr, desc) },
  { "fullname", "string", sizeof (char*),
    FMOffset (pds_entity_change_event_ptr, fullname) },
  { "entity_id", "pds_entity_id_t", sizeof (pds_entity_id_t),
    FMOffset (pds_entity_change_event_ptr, entity_id) },
  { "entity_data", "pds_entity_data_t", sizeof (pds_entity_data_t),
    FMOffset (pds_entity_change_event_ptr, entity_data) },
  
  {(char*) 0, (char *) 0, 0, 0}
};
    
FMStructDescRec entity_change_event_formats[] =
{
  { "entity_change_event", entity_change_event_flds, sizeof(pds_entity_change_event) },
  { "pds_entity_id_t", pds_entity_id_t_flds, sizeof(pds_entity_id_t) },
  { "pds_entity_data_t", pds_entity_data_t_flds, sizeof(pds_entity_data_t) },
  FMStructDescList_Terminator
};

/*
 *  context change event definitions
 */
FMField context_change_event_flds[] =
{
  { "change type", "integer", sizeof (int),
    FMOffset (pds_context_change_event_ptr, type) },
  { "description", "string", sizeof (char*),
    FMOffset (pds_context_change_event_ptr, desc) },
  { "fullname", "string", sizeof (char*),
    FMOffset (pds_context_change_event_ptr, fullname) },
  { "context_id", "pds_context_id_t", sizeof (pds_context_id_t),
    FMOffset (pds_context_change_event_ptr, context_id) },

  {(char*) 0, (char *) 0, 0, 0}
};

FMStructDescRec context_change_event_formats[] =
{
  { "context_change_event", context_change_event_flds, sizeof(pds_context_change_event) },
  { "pds_context_id_t", pds_context_id_t_flds, sizeof(pds_context_id_t)},
  FMStructDescList_Terminator
};


/*
 *  domain change event definitions
 */
FMField domain_change_event_flds[] =
{
  { "change type", "integer", sizeof (int),
    FMOffset (pds_domain_change_event_ptr, type) },

  { "d_name", "string", sizeof (char*),
    FMOffset (pds_domain_change_event_ptr, d_name) },

  { "d_type", "string", sizeof (char*),
    FMOffset (pds_domain_change_event_ptr, d_type) },

  { "d_version", "integer", sizeof (int),
    FMOffset (pds_domain_change_event_ptr, d_version) },

  { "d_app_name", "string", sizeof (char*),
    FMOffset (pds_domain_change_event_ptr, d_app_name) },

  { "domain_id", "pds_domain_id_t", sizeof (pds_domain_id_t),
    FMOffset (pds_domain_change_event_ptr, domain_id) },

  { "event type", "integer", sizeof (int),
    FMOffset (pds_domain_change_event_ptr, event_type) },


  { "event full name", "string", sizeof (char*),
    FMOffset (pds_domain_change_event_ptr, event_fullname) },
  
  { "event desc", "string", sizeof (char*),
    FMOffset (pds_domain_change_event_ptr, event_desc) },

  {(char*) 0, (char *) 0, 0, 0}
};

FMStructDescRec domain_change_event_formats[] =
{
  { "domain_change_event", domain_change_event_flds, sizeof(pds_domain_change_event) },
  { "pds_domain_id_t", pds_domain_id_t_flds, sizeof(pds_domain_id_t) },
  FMStructDescList_Terminator
};

handler_tag_FMStructDescRec entity_DescRecs[] = 
{
  { ENTITY_CREATE_DESTROY, entity_exist_change_ntf_formats},
  { ENTITY_BIND_UNBIND, entity_u_bind_change_ntf_formats},
  { ENTITY_DATA_CHANGE, entity_data_change_ntf_formats }, 
  { 0, NULL }
};

handler_tag_FMStructDescRec context_DescRecs[] =
{
  { CONTEXT_BIND_UNBIND, context_u_bind_change_ntf_formats},
  { CONTEXT_CREATE_DESTROY, context_exist_change_ntf_formats},
  { 0, NULL }
};

handler_tag_FMStructDescRec domain_DescRecs[] =
{
  { DOMAIN_CHANGE, domain_change_event_formats},
  { 0, NULL }
};

/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.9  2007/12/04 21:06:16  pmw
 *  Merge changes from evpath branch.  Also probably includes
 *  Darwin fixes.  Compiles on isleroyale and marquesas, I think
 *  I'm misconfigured somehow for building on gondor.
 *
 *  Revision 1.8.2.1  2007/01/05 22:40:30  pmw
 *  interim commit for evpath changes, new branch
 *
 *  Revision 1.8  2005/02/24 19:28:31  pmw
 *  Everything should now be using string encoding for attr lists.  I
 *  removed all the ifdefs.
 *
 *  Revision 1.7  2004/11/06 22:23:33  mansour
 *  Added a bind_context_by_name API. Also fixed the XML logic to use the correct bind name.
 *
 *  Revision 1.6  2004/11/03 06:59:36  mansour
 *  Added APIs for context binding. The XML dump routine expands bounded contexts in
 *  to a full tree and the load routine loads it as separate contexts.
 *
 *  Revision 1.5  2004/04/27 20:59:24  pmw
 *  Added RPC for open_service to simplify service_from_ID handling on the
 *  client.  The way I set that up at first was a little braindead, and failed
 *  when one side was weakly hostnamed (via DHCP, for example).  The new RPC
 *  asks the server what address/port combination it intends to use and saves
 *  those.
 *
 *  Revision 1.4  2003/10/31 05:38:54  mansour
 *  Added xpath query support
 *
 *  Revision 1.3  2003/01/30 19:15:57  mansour
 *  Add support for domain events
 *
 *  Revision 1.2  2002/08/07 18:51:44  eisen
 *  Widespread changes to modify how attribute handling works.  Stop using
 *  attr_atom_from_string() to get atom values.  Instead, those are set by the
 *  application.  Stop using get_attr() (api has changes).  Stop using
 *  xmit_object and start using the ATL-supplied encode/decode routines.
 *
 *  Revision 1.1  2001/10/09 21:37:54  pmw
 *  configuration changes
 *
 *  Revision 1.7  2001/10/07 22:19:15  pmw
 *  Added domain_change event structures so that Proactive class is always correctly
 *  initialized.
 *
 *  Revision 1.6  2001/09/23 22:55:53  pmw
 *  Contexts now support attributes of their own.  Practically, this is supported
 *  by having ContextBindable host the attribute list instead of Entity/Context.  Also
 *  added the usual functions on the client side and handlers for getting/setting
 *  this attr list for Contexts.
 *
 *  Revision 1.5  2001/09/03 20:09:10  pmw
 *  changes to deal with string-marshaled attr lists
 *
 *  Revision 1.4  2001/08/30 21:02:37  pmw
 *  partial changes for string-marshaled attr lists
 *
 *  Revision 1.3  2001/07/15 02:29:37  pmw
 *  Fixed context creation problem and added some changes for Contexts to return
 *  their binding lists.
 *
 *  Revision 1.2  2001/07/02 04:02:03  pmw
 *  interim checkin for changes to make Contexts also support events.  compiles
 *  but tests probably do not run.
 *
 *  Revision 1.1.1.1  2000/12/13 22:14:21  pmw
 *  imported sources
 *
 *  Revision 1.10  2000/12/05 00:24:19  pmw
 *  Added server_shutdown message
 *
 *  Revision 1.9  2000/11/21 00:23:50  pmw
 *  Since I didn't have enough to do, a little code reorganization to keep
 *  things from getting dramatically out of hand.  Fixed a couple of
 *  bugs-in-waiting.
 *
 *  Revision 1.8  2000/11/19 23:28:28  pmw
 *  pds_server now supports bulk load of data with -f command-line option.  Also added
 *  command message so bulk loads can happen on-line.
 *
 *  Revision 1.7  2000/04/10 20:00:10  pmw
 *  completed port to CM from DataExchange
 *
 *
 */
