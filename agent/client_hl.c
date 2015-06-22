
/*
 * 
 *  $Id: client_hl.c,v 1.4 2007-12-04 21:06:14 pmw Exp $
 *
 */
#include "../config.h"

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <stdio.h>

#include "pds.h"
#include "common/formats.h"

  /*
   *  Need to write:
   *
   *  get/set functions for sets of values
   *
   */

int
pds_lookup_string (pds_domain_id_t domain_id,
                  const char* fullname,
                  char **value)
{
  pds_entity_char_data_t d;
  
  d.data = NULL;
  d.data_size = 0;

  if (pds_get_entity_char_data (domain_id, fullname, null_pds_context_id, &d, 0) != 1)
    {
      value = NULL;
      return -1;
    }

  /* 
   *must malloc an extra character at the end since
   * we don't store the terminating NULL
   */
  *value = (char*) malloc (d.data_size + 1);
  memcpy (*value, d.data, d.data_size);
  *(*value + d.data_size) = '\0';

  return 1;
}


int
pds_bind_string (pds_domain_id_t domain_id,
                const char* fullname,
                const char* value)
{
  pds_entity_char_data_t d;
  int opt_mask = 0;
  
  d.data_size = strlen (value);
  d.data = (unsigned char*) value;

  opt_mask |= CreateObjIfNotFound;

  return pds_set_entity_char_data (domain_id, fullname, null_pds_context_id, &d, opt_mask);
}

int
pds_lookup_int (pds_domain_id_t domain_id,
               const char* fullname,
               int* value)
{
  pds_entity_int_data_t d;

  d.data = NULL;
  d.data_size = 0;

  if (pds_get_entity_int_data (domain_id, fullname, null_pds_context_id, &d, 0) != 1)
    {
      value = NULL;
      return -1;
    }

  *value = (int)*(d.data);

  return 1;
}

int
pds_bind_int (pds_domain_id_t domain_id,
             const char* fullname,
             int value)
{
  pds_entity_int_data_t d;
  int opt_mask = 0;
  
  d.data_size = sizeof(int);
  d.data = (unsigned char*) &value;

  opt_mask |= CreateObjIfNotFound;

  return pds_set_entity_int_data (domain_id, fullname, null_pds_context_id, &d, opt_mask);
}


int
pds_lookup_blob (pds_domain_id_t domain_id,
                const char* fullname,
                char** value)
{
  pds_entity_char_data_t d;

  d.data = NULL;
  d.data_size = 0;

  if (pds_get_entity_char_data (domain_id, fullname, null_pds_context_id, &d, 0) != 1)
    {
      value = NULL;
      return -1;
    }

  *value = (char*) malloc (d.data_size);
  memcpy (*value, d.data, d.data_size);

  return 1;
}

int
pds_bind_blob (pds_domain_id_t domain_id,
              const char* fullname,
              char *value,
             unsigned int length)
{
  pds_entity_char_data_t d;
  int opt_mask = 0;
  
  d.data_size = length;
  d.data = (unsigned char*) value;

  opt_mask |= CreateObjIfNotFound;

  return pds_set_entity_char_data (domain_id, fullname, null_pds_context_id, &d, opt_mask);
}

/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.3.2.1  2007/01/05 22:40:27  pmw
 *  interim commit for evpath changes, new branch
 *
 *  Revision 1.3  2005/12/08 23:37:04  eisen
 *  Fix includes after removing #include of cm.h from echo.h.
 *
 *  Revision 1.2  2003/02/03 15:30:18  chaos
 *  remove C++-style comments from C files so they compile on elvis
 *
 *  Revision 1.1  2001/10/09 21:37:53  pmw
 *  configuration changes
 *
 *  Revision 1.2  2001/09/23 22:55:53  pmw
 *  Contexts now support attributes of their own.  Practically, this is supported
 *  by having ContextBindable host the attribute list instead of Entity/Context.  Also
 *  added the usual functions on the client side and handlers for getting/setting
 *  this attr list for Contexts.
 *
 *  Revision 1.1  2001/09/10 03:56:50  pmw
 *  C-language versions of client-side code, now compiled exclusively with gcc.
 *
 *  Revision 1.1.1.1  2000/12/13 22:14:23  pmw
 *  imported sources
 *
 *  Revision 1.3  2000/11/30 23:33:47  pmw
 *  fixed schizophrenia regarding management of strings stored as entity data.
 *  strings should! be always stored WITHOUT the terminating NULL character.
 *  wp_lookup_string allocates space for and tacks on this character when it
 *  is called.  If you're calling get_entity_data directly, well, ya better
 *  figure it out yourself.
 *
 *  Revision 1.2  2000/11/19 23:28:28  pmw
 *  wp_server now supports bulk load of data with -f command-line option.  Also added
 *  command message so bulk loads can happen on-line.
 *
 *  Revision 1.1  1999/12/21 19:04:37  pmw
 *  added client_hl.cc
 *
 *
 */
