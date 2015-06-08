/*
 *
 *  $Id: basic2.c,v 1.21 2008-09-24 12:39:17 eisen Exp $
 *
 */
#include "config.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "agent/pds.h"
#include "common/formats.h"


#include <string.h>
#include <stdlib.h>
#include <stdio.h>

pds_service directory;
pds_domain_id_t domain_id;
pds_context_id_t root_context_id;
CManager cm;

void generate_channel_events();
void create_channel_objects();
void my_connect(char * host, char * port);
void create_entity_int4();
void create_entity_int8();
void create_entity_string();
void create_entity_opaque();
void create_context();
void browse_entity();
void browse_context();
void browse_all();
void remove_entity();
void remove_context();
void remove_root_context();
void create_entity_string_2();
void register_domain();
void register_entity();
void register_context();
void xpath_query();
void bind_context_by_id();
void bind_context_by_name();
int main(int argc, char* argv[])
{
    if (argc < 2) {
        printf("Usage: %s <pds host> <pds port>\n", argv[0]);
        return -1;
    }
    cm = CManager_create();
    my_connect(argv[1], argv[2]);
    register_domain();
    xpath_query();
printf("--------------------------\n");
    bind_context_by_id();
printf("--------------------------\n");
    bind_context_by_name();
printf("--------------------------\n");
    create_context();
    create_entity_int4();
    xpath_query();
    create_entity_int8();
    create_entity_string();
    create_entity_string_2();
    xpath_query();
    create_entity_opaque();
    xpath_query();
/*    browse_context();*/
/*    browse_entity();*/
/*    browse_all();*/
/*    remove_entity();*/
/*    remove_context();*/
/*    remove_root_context();*/
/*    create_channel_objects();*/
/*    register_context(); */
/*    register_entity();*/

printf("************* END ********\n");
	/*
    generate_channel_events();
	
	*/
    CMrun_network (cm);
    return pds_service_close (directory);
}
/***********************************************/
void my_connect(char * host, char * port)
{
    attr_list contact_attrs;

    contact_attrs = create_attr_list();
    set_attr (contact_attrs, attr_atom_from_string("PDS_IP_HOST"), Attr_String, strdup (host) );
    set_attr (contact_attrs, attr_atom_from_string("PDS_IP_PORT"), Attr_Int4, (attr_value*)atol(port));

    dump_attr_list(contact_attrs);

    directory = pds_service_open (contact_attrs);
    printf("opened directory service\n");


    domain_id = pds_open_domain (directory,
                                 "1",
                                 "1",
                                 1,
                                 "1");
    printf("opened domain\n");

    root_context_id = pds_get_root_context (domain_id);
    printf("Got root context.\n");

}
/*********************************************/
void create_entity_opaque()
{

    pds_entity_data_t e_data;

    attr_list attrs;
    char * buffer;
    long size;
    FILE *file =fopen("basic2", "rb");

    attrs = create_attr_list();
    
    (void) pds_create_entity (domain_id,
                  "/opaque_entity",
                  null_pds_context_id,
                  0,
                  attrs);
		  
    /* set the opaque value to a REALLY long string*/

    if (file) {
        fseek(file, 0L, SEEK_END);
        size = ftell(file);
        rewind(file);
    }
    else {
        printf("Could not open file\n");
        return;
    }

    buffer = (char*)malloc(size);
    /* read(file, buffer, size);  -- this can't be right (fd instead of FILE*?) - pmw */
    fread( buffer, size, 1, file );
    fclose(file);
 
    e_data.data = (unsigned char*)buffer;
    e_data.data_size = size;
    e_data.data_type = Attr_Opaque;
    
    
    pds_set_entity_data(domain_id, "/opaque_entity", null_pds_context_id, &e_data, 0);
    printf("created opaque entity, size %lu\n", size);
}

/*********************************************/
void create_entity_int8()
{

    pds_entity_data_t e_data;

    attr_list attrs;



    e_data.data = (unsigned char *)"5678";
    e_data.data_size = 4;    
    e_data.data_type = Attr_Int8;
    
    attrs = create_attr_list();
    
    (void) pds_create_entity (domain_id,
                  "/int8_entity",
                  null_pds_context_id,
                  &e_data,
                  attrs);

    pds_set_entity_data(domain_id, "/int8_entity", null_pds_context_id, &e_data, 0);
    printf("created int8 entity\n");
}
/*********************************************/
void create_entity_int4()
{

    pds_entity_data_t e_data;

    attr_list attrs;



    e_data.data = (unsigned char *)"1234";
    e_data.data_size = 4;    
    e_data.data_type = Attr_Int4;
    
    attrs = create_attr_list();
    
    (void) pds_create_entity (domain_id,
                  "/int4_entity",
                  null_pds_context_id,
                  0,
                  attrs);
    pds_set_entity_data(domain_id, "/int4_entity", null_pds_context_id, &e_data, 0);
    printf("created int4 entity\n");
}
/*********************************************/
void create_entity_string()
{

    pds_entity_data_t e_data;

    attr_list attrs;
    char* str = "A test string";


    e_data.data = (unsigned char*)str;
    e_data.data_size = strlen(str);    
    e_data.data_type = Attr_String;
    
    attrs = create_attr_list();
    
    (void) pds_create_entity (domain_id,
                  "/string_entity_1",
                  null_pds_context_id,
                  0,
                  attrs);
    pds_set_entity_data(domain_id, "/string_entity_1", null_pds_context_id, &e_data, 0);
    printf("created string entity\n");

}
/*********************************************/
void create_entity_string_2()
{

    pds_entity_data_t e_data;

    attr_list attrs;
    char* str = "A test string";


    e_data.data = (unsigned char*)str;
    e_data.data_size = strlen(str);
    e_data.data_type = Attr_String;

    attrs = create_attr_list();

    (void) pds_create_entity (domain_id,
                  "/string_entity_2",
                  null_pds_context_id,
                  &e_data,
                  attrs);
    pds_set_entity_data(domain_id, "/string_entity_2", null_pds_context_id, &e_data, 0);
}

/*********************************************/
void bind_context_by_id()
{

    pds_context_id_t c2 = pds_create_context (domain_id, 
                        "/1/2",
                        root_context_id);
    xpath_query();
    pds_context_id_t c4 = pds_create_context (domain_id, 
                        "/1/2/3/4",
                        root_context_id);
    xpath_query();
    //pds_context_id_t c5 = pds_create_context (domain_id, 
    //                    "1/2/3/5",
    //                    root_context_id);
    pds_bind_context_by_id(domain_id, c2, "/1/b");
    xpath_query();
    pds_bind_context_by_id(domain_id, c2, "/a/b");
    xpath_query();
    (void) c4;
    pds_unbind_context_from_name(domain_id, "/1/b");
    xpath_query();
    pds_unbind_context_from_name(domain_id, "/a/b");
    xpath_query();
    //pds_bind_context_by_id(domain_id, c5, "/a/b/d");

}
/*********************************************/
void bind_context_by_name()
{

    pds_context_id_t c2 = pds_create_context (domain_id, 
                        "/11/22",
                        root_context_id);
    pds_context_id_t c4 = pds_create_context (domain_id, 
                        "/11/22/33/44",
                        root_context_id);
    //pds_context_id_t c5 = pds_create_context (domain_id, 
    //                    "1/2/3/5",
    //                    root_context_id);
    (void) c4;
    (void) c2;
    pds_bind_context_by_name(domain_id, "/11/22", "/11/bb");
    pds_bind_context_by_name(domain_id, "/11/22", "/aa/bb");
    xpath_query();
    pds_unbind_context_from_name(domain_id, "/11/bb");
    pds_unbind_context_from_name(domain_id, "/aa/bb");
    xpath_query();
    //pds_bind_context_by_id(domain_id, c5, "/a/b/d");

}
/*********************************************/
void create_context()
{
#if 0
    (void) pds_create_context (domain_id, 
                                           "child_context",
                                            root_context_id);

    (void) pds_create_context (domain_id, 
                                           "child_context2",
                                            root_context_id);
#endif
    (void) pds_create_context (domain_id, 
                                           "mansour/mohamed",
                                            root_context_id);
    (void) pds_create_context (domain_id, 
                                           "/gatech/coc",
                                            null_pds_context_id);
    printf("created child context\n");
}
/*********************************************/
void dump_bindings(char** bindings)
{
printf("[--------\n");
  int i;
  for (i=0; bindings[i] !=0; i++) { 
    printf("\t%s\n", bindings[i]);
    free(bindings[i]);
  }

  free(bindings);
printf("--------]\n");
}
/*********************************************/
void browse_context_path(char*path)
{
  char** bindings;
  int i;

  printf("browsing context %s\n", path);
  bindings = pds_get_binding_list(domain_id, path, OnlyContextBindings);

  for (i=0; bindings[i] !=0; i++) { 
    char buffer[1000];
    strcpy(buffer, path);
    strcat(buffer, "/");
    strcat(buffer, bindings[i]);
    browse_context_path(buffer);
    free(bindings[i]);
  }
  free (bindings);
}
/*********************************************/
void browse_context()
{
  char** bindings;
  int i;

  printf("browsing contexts\n");
  bindings = pds_get_binding_list(domain_id, "/", OnlyContextBindings);

  for (i=0; bindings[i] !=0; i++) { 
    char buffer[1000];
    strcpy(buffer, "/");
    strcat(buffer, bindings[i]);
    browse_context_path(buffer);
    free(bindings[i]);
  }

  free(bindings);
}
/*********************************************/
void browse_entity()
{
  char** bindings;

 printf("browsing entities\n");
  bindings = pds_get_binding_list_id(domain_id, root_context_id, OnlyEntityBindings);

   dump_bindings(bindings);
}
/*********************************************/
void browse_all()
{
  char** bindings;

  printf("browsing contexts and entities\n");
  bindings = pds_get_binding_list_id(domain_id, root_context_id, 0);

   dump_bindings(bindings);

}
/*********************************************/
void remove_entity()
{
    attr_list attrs;
    pds_entity_id_t e_id;

    printf("removing entity\n");
    pds_remove_entity(domain_id, "/opaque_entity", null_pds_context_id);
    pds_remove_entity(domain_id, "string_entity_1", root_context_id);
   
    attrs = create_attr_list();
    e_id = pds_create_entity(domain_id, "/test_entity", null_pds_context_id, 0, attrs);
    pds_remove_entity_by_id(domain_id, e_id);
}
/*********************************************/
void remove_context()
{
    pds_context_id_t c_id;
    printf("removing context\n");
    pds_remove_context(domain_id, "/mansour", null_pds_context_id);
    c_id = pds_create_context(domain_id, "/l1/l2/l3", null_pds_context_id);
    pds_remove_context_by_id(domain_id, c_id);
}
/*********************************************/
void remove_root_context()
{
    printf("remove root context\n");
    pds_remove_context(domain_id, "/", null_pds_context_id);
    pds_create_context(domain_id, "/", null_pds_context_id);
}
/*********************************************/
int
domain_event_handler (CManager cm, void* event, void* client_data, attr_list event_list)
{
  pds_domain_change_event* evt = 0;


  evt = (pds_domain_change_event*) event;
  printf("--------------------------\n");
  printf("domain event handler called\n");  
  printf("%d\n", evt->type);
  printf("[%d][%s][%s]\n", evt->event_type, 
         evt->event_desc ? evt->event_desc:"", 
         evt->event_fullname ? evt->event_fullname : "" );
  return 1;
}
/*********************************************/
int
entity_event_handler (CManager cm, void* event, void* client_data, attr_list event_list)
{
  pds_entity_change_event* evt = 0;


  evt = (pds_entity_change_event*) event;
  printf("--------------------------\n");
  printf("entity event handler called\n");  
  printf("%d\n", evt->type);
  printf("[%s][%s]\n", evt->desc, evt->fullname);
  return 1;
}
/*********************************************/
int
context_event_handler (CManager cm, void* event, void* client_data, attr_list event_list)
{
  pds_context_change_event* evt = 0;


  evt = (pds_context_change_event*) event;
  printf("--------------------------\n");  
  printf("context event handler called\n");  
  printf("%d\n", evt->type);
  printf("[%s][%s]\n", evt->desc, evt->fullname);
  return 1;
}
/*********************************************/
void register_domain()
{
 
    printf("registering for domain changes\n");

    pds_register_for_domain_changes(cm, domain_id, domain_event_handler, 0);
}
/*********************************************/
void register_entity()
{

    printf("registering for entity changes\n");

    pds_register_for_entity_changes_by_channel(cm, domain_id, "/test_events/register_entity_1", null_pds_context_id, entity_event_handler, 0, ENTITY_CREATE_DESTROY);

/****** cases that migh cause the server to crash*/
/*    pds_register_for_entity_changes(cm, domain_id, "/test_change/register_entity_2", null_pds_context_id, event_handler, 0);*/
/*    pds_register_for_entity_changes(cm, domain_id, "/test_change/level2/register_entity_2", null_pds_context_id, event_handler, 0);*/
}
/*********************************************/
void register_context()
{
    printf("registering for context changes\n");
    pds_register_for_context_changes_by_channel(cm, domain_id, "/", null_pds_context_id, context_event_handler, 0, CONTEXT_CREATE_DESTROY);
    pds_register_for_context_changes_by_channel(cm, domain_id, "/test_events", null_pds_context_id, context_event_handler, 0, CONTEXT_CREATE_DESTROY);
    pds_register_for_context_changes_by_channel(cm, domain_id, "/test_events/newContext", null_pds_context_id, context_event_handler, 0, CONTEXT_CREATE_DESTROY);
    pds_register_for_context_changes_by_channel(cm, domain_id, "/test_events/test_new_context", null_pds_context_id, context_event_handler, 0, CONTEXT_CREATE_DESTROY);
/****** cases that migh cause the server to crash*/
/*    pds_register_for_context_changes(cm, domain_id, "/", null_pds_context_id, event_handler, 0);*/
/*    pds_register_for_context_changes(cm, domain_id, "/gatech1", null_pds_context_id, event_handler, 0);*/
/*    pds_register_for_context_changes(cm, domain_id, "/gatech2/coc1", null_pds_context_id, event_handler, 0);*/
}
/*********************************************/
void create_channel_objects() 
{
    attr_list  attrs = create_attr_list();

    pds_create_context (domain_id,
                        "/test_events",
                         null_pds_context_id);
    pds_create_entity (domain_id,
                  "/test_events/register_entity_1",
                  null_pds_context_id,
                  0,
                  attrs);
}
/*********************************************/
void generate_channel_events()
{
    attr_list  attrs = create_attr_list();

    printf("generating channel events\n");

    /* create a new context from root */
    pds_create_context (domain_id, "/test_new_context", null_pds_context_id);
    pds_create_entity (domain_id, "/test_new_entity", null_pds_context_id, 0, attrs);
    pds_remove_entity (domain_id, "/test_new_entity", null_pds_context_id);
    pds_remove_context(domain_id, "/test_new_context", null_pds_context_id);    

    pds_create_context(domain_id, "/test_events/newContext", null_pds_context_id);
    pds_create_entity (domain_id, "/test_events/newContext/test_new_entity", null_pds_context_id, 0, attrs);
    pds_remove_entity (domain_id, "/test_events/newContext/test_new_entity", null_pds_context_id);
    pds_remove_context(domain_id, "/test_events/newContext", null_pds_context_id);    
    pds_remove_context(domain_id, "/test_events", null_pds_context_id);    
}

/*********************************************/
void xpath_query()
{
  char** list = pds_query_by_xpath(domain_id, "//PDSContext");
  dump_bindings(list);
}


/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.20  2008/07/01 13:35:43  chaos
 *  Fix notreached warning.  Use CMrun_network instead of poll in an infinite loop.
 *
 *  Revision 1.19  2007/12/04 21:06:26  pmw
 *  Merge changes from evpath branch.  Also probably includes
 *  Darwin fixes.  Compiles on isleroyale and marquesas, I think
 *  I'm misconfigured somehow for building on gondor.
 *
 *  Revision 1.18.2.2  2007/01/25 22:05:01  pmw
 *  now correctly gets local contact information by CMlisten() if necessary in do_registration()
 *
 *  Revision 1.18.2.1  2007/01/05 22:40:42  pmw
 *  interim commit for evpath changes, new branch
 *
 *
 */
