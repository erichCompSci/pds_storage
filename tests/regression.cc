#include "config.h"

#include <string>

extern "C" {
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TYPES
#include <sys/types.h>
#endif
#include <sys/wait.h>
  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "agent/pds.h"
  
#include "cmdline.h"
}

using namespace std;  
  
pid_t server_pid;

int main (int argc, char *argv[])
{
  struct gengetopt_args_info args_info;
  
  if ( cmdline_parser ( argc, argv, &args_info ) != 0 )
    exit(1);
  
  string hostname = args_info.hostname_given ? args_info.hostname_arg : "localhost";
  bool quiet = args_info.quiet_flag;
  
  if (args_info.regression_flag) {

    /* 
     * we are a regression test, spawn the server
     */

    server_pid = fork();
    if (server_pid == 0) {
      const char * const pdsd_argv[4] = {
	"pdsd",
	"-no_fork",
	"-quiet",
	NULL };
      /* exec the server */
      if (execv ( "../server/pdsd", const_cast<char * const *>(pdsd_argv) ) == -1) {
	perror( "execv() failed" );
      }
    }

    /* wait a few to let him get started */
    sleep(5);
  }

  pds_service wps;
  pds_domain_id_t new_domain_id;
  pds_context_id_t cid1, cid2;
  pds_entity_id_t eid1;
  attr_list attrs, attrs2, contact_attrs;
  char *pds_host;
  int val1 = 99, val2 = 98;
  char const *str = "This is the first test string I will store.";
  pds_entity_data_t tt;
  char **bindings;
  int i2;
  atom_t VAL1_ATOM, VAL2_ATOM;

  if (cmdline_parser (argc, argv, &args_info) != 0) exit(1);
  if (args_info.hostname_given) printf ("hostname is %s\n", args_info.hostname_arg);

  tt.data = (unsigned char*)str;
  tt.data_size = strlen (str);
  tt.data_type = Attr_String;

  pds_host = getenv ("PDS_SERVER_HOST");
  if (pds_host == NULL) pds_host = getenv ("HOSTNAME");
  if (pds_host == NULL) {
      char hostname[128];
      if (gethostname(&hostname[0], sizeof(hostname)) == 0) {
	  pds_host = strdup(hostname);
      }
  }

  contact_attrs = create_attr_list();
  set_attr (contact_attrs,
            attr_atom_from_string("IP_HOST"),
            Attr_String,
            strdup (pds_host));
  set_attr (contact_attrs,
            attr_atom_from_string("IP_PORT"),
            Attr_Int4,
            (attr_value*)8848);

  if ((wps = pds_service_open (contact_attrs)) == NULL)
    {
      fprintf (stderr, "Couldn't init PDS client-side (is pdsd running?)\n");
      exit (1);
    }

  new_domain_id = pds_open_domain (wps, 
                                  "newDomain",
                                  "newDomaintype",
                                  1,
                                  "wp-test");


  cid1 = pds_get_root_context (new_domain_id);
  if (!quiet) printf ("Got root context...\n");
  
  attrs = create_attr_list();
  if (!quiet) printf ("created attr list...\n");

  VAL1_ATOM = attr_atom_from_string("PDS_TEST_VAL1");
  VAL2_ATOM = attr_atom_from_string("PDS_TEST_VAL2");
  add_int_attr (attrs, VAL1_ATOM, val1);
  add_int_attr (attrs, VAL2_ATOM, val2);
  if (!quiet) printf ("[ added attributes ]");
  fflush (0);

  eid1 = pds_create_entity (new_domain_id, "newEntity", null_pds_context_id, &tt, attrs);
  if (!quiet) printf ("[ created entity %s]", eid1.id);
  fflush (0);

  attrs2 = create_attr_list();
  pds_get_entity_attributes_by_id (new_domain_id, eid1, attrs2);
  if (!quiet) printf ("[ retrieved entity attributes ]"); fflush (0);

  if (!quiet) dump_attr_list (attrs2);

  bindings = pds_get_binding_list_id (new_domain_id, cid1, OnlyEntityBindings);
  i2 = 0;
  for (i2 = 0; bindings[i2] != NULL; i2++)
    {
      if (!quiet) printf ("got binding of root context [%s]\n", bindings[i2]);
      free(bindings[i2]);
    }
  free(bindings);

  pds_remove_entity (new_domain_id, "newEntity", null_pds_context_id);
  if (!quiet) printf ("[ removed entity ]"); fflush (0);

  cid2 = pds_create_context (new_domain_id, "newContext", cid1);

  cid2 = pds_create_context (new_domain_id, "new3Context/child", cid1);
  cid2 = pds_create_context (new_domain_id, "gatech/coc", cid1);
  cid2 = pds_create_context (new_domain_id, "new2Context", cid1);
  if (!quiet) printf ("created new context... \n");

  bindings = pds_get_binding_list_id (new_domain_id, cid1, OnlyContextBindings);
  i2 = 0;
  for (i2 = 0; bindings[i2] != NULL; i2++)
    {
      if (!quiet) printf ("got binding of root context [%s]\n", bindings[i2]);
      free(bindings[i2]);
    }
  free(bindings);

  if (!quiet) printf( "Checking for bindings in created context... " );
  bindings = pds_get_binding_list_id( new_domain_id, cid2, OnlyEntityBindings );
  if (!quiet) printf( "done\n" );
  
  if (!quiet) printf ("removing context... "); fflush(0);
  pds_remove_context_by_id (new_domain_id, cid2);
  printf( "done\n" );
  
  if (pds_remove_domain (new_domain_id))
    {
      if (!quiet) printf ("domain removal success\n");
    }
  else
    {
      if (!quiet) printf ("domain removal failure\n");
    }

  //  pds_service_close (wps);
  pds_shutdown_server ( wps );

  fprintf(stderr, "mark1\n");
  free_attr_list (attrs);
  free_attr_list (attrs2);
  free_attr_list (contact_attrs);

  int exit_state;
  waitpid( server_pid, &exit_state, WNOHANG );
  
  return 0;
}

