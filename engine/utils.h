#ifndef _UTILS_H_
#define _UTILS_H_

/*
 *  
 *  $Id: utils.h,v 1.2 2003-10-05 12:10:32 pmw Exp $
 *
 */

#include "agent/pdsdefs.h"

/*
 *
 *  operations on service object ids
 *
 */
#define DOMAIN_REF 0
#define ENTITY_REF 1
#define CONTEXT_REF 1

#ifdef __cplusplus
extern "C"
{
#endif 

unsigned long
get_ref_from_id (unsigned char id[PDS_ID_SIZE], int which);

void
make_domain_id_null (pds_domain_id_t* id);

void
make_entity_char_data_null (pds_entity_char_data_t* d);
void
make_entity_int_data_null (pds_entity_int_data_t* d);
void
make_entity_float_data_null (pds_entity_float_data_t* d);

void
make_entity_id_null (pds_entity_id_t* id);

void
make_context_id_null (pds_context_id_t* id);

unsigned long
get_timestamp();

int 
pds_is_context_id_null (pds_context_id_t*);

int 
pds_is_entity_id_null (pds_entity_id_t*);

int 
pds_is_domain_id_null (pds_domain_id_t*);

#ifdef __cplusplus
}
#endif

/*
 * 
 *  $Log: not supported by cvs2svn $
 *  Revision 1.1  2001/10/09 21:37:55  pmw
 *  configuration changes
 *
 *  Revision 1.2  2001/09/10 03:55:43  pmw
 *  client lib is now libpdsagent, and is compiled entirely with gcc.  libpdsutil no
 *  longer exists.
 *
 *  Revision 1.1.1.1  2000/12/13 22:14:22  pmw
 *  imported sources
 *
 *  Revision 1.4  2000/11/28 22:49:31  pmw
 *  1) moved CManager stuff to Proactive class, since other classes
 *  don't need to deal with it any longer
 *  2) moved object ID management out of Domain and into objectId.
 *
 *  Revision 1.3  2000/11/21 00:23:55  pmw
 *  Since I didn't have enough to do, a little code reorganization to keep
 *  things from getting dramatically out of hand.  Fixed a couple of
 *  bugs-in-waiting.
 *
 *  Revision 1.2  2000/10/25 21:44:48  pmw
 *  Compiles after major revisions to support multiple WP services on
 *  client-side.  This should eventually allow a client to use services by
 *  linking in the library directly or by communicating using CMRPC.
 *
 *  Revision 1.1  1999/12/21 18:47:56  pmw
 *  Too many changes - really shouldn't have done this many
 *
 *
 */

#endif /* closes inclusion guard */
