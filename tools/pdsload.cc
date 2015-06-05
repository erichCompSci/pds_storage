#include <stdlib.h>
#include <stdio.h>
#include <string.h>
extern "C"
{
#include "atl.h"
#include "evpath.h"
#include "cmrpc.h"
}

#include "agent/pds.h"
#include "common/formats.h"

CManager server_cm;

int main (int argc, char *argv[])
{
  load_from_URL_msg msg;
  return_status_msg return_msg;
  attr_list contact_attrs;
  int args_ok = 0;

  if (argc < 2)
    {
      fprintf (stderr, "usage: pdsload -u url\n");
      exit (1);
    }

  while (argv[1] && (argv[1][0] == '-')) 
    {
      if (argv[1][1] == 'u') 
        {
          msg.url = argv[2];
          args_ok = 1;
        } 
      else if (argv[1][1] == 's') 
        {

        } 
      else if (argv[1][1] == 'q') 
        {

        } 
      else if (argv[1][1] == 'v') 
        {

        } 
      else if (argv[1][1] == 'n') 
        {

        }
      argv++;
    }

  if (!args_ok)
    {
      fprintf (stderr, "usage: pdsload -u url\n");
      exit (1);
    }

  contact_attrs = create_attr_list();
  set_attr (contact_attrs,
            attr_atom_from_string("PDS_IP_HOST"),
            Attr_String,
            strdup (getenv("HOSTNAME")));
  set_attr (contact_attrs,
            attr_atom_from_string("PDS_IP_PORT"),
            Attr_Int4,
            (attr_value*)8848);

  server_cm = CManager_create();

  CMrpc_call_anon_rpc (server_cm, 
		       LOAD_FROM_URL_RPC_NAME,
		       contact_attrs,
		       load_from_URL_msg_formats,
		       &msg,
		       return_status_msg_formats,
		       &return_msg,
		       0);


  CManager_close (server_cm);
  free_attr_list (contact_attrs);

  return 0;
}

  
