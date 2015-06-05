/*
 *
 *  $Id: pdsd_http.cc,v 1.9 2008-06-26 19:24:20 pmw Exp $
 *
 */

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#ifndef __GNUC__
#include <assert.h>
#endif
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include "atl.h"
#include "evpath.h"
#include "cm_transport.h"
#include "cmrpc.h"

};

extern "C" 
{
#include "common/formats.h"
}
#include "engine/objectId.h"
#include "engine/Proactive.h"
#include "engine/Context.h"
#include "engine/Entity.h"
#include "engine/Domain.h"
#include "engine/EngineGuard.h"
#include "engine/utils.h"

#define MAX_HTTP_REQ_LEN 2048

extern "C" void
handle_http_request (CMConnection cmc, CMTransport trans, char *cmbuffer, long length)
{
  char buffer[MAX_HTTP_REQ_LEN];
  char c;
  long data_length = 0;
  long actual = 0;
  int done = 0;
  int pos = 0;
  int state0 = 0;

  pdsTrace_out (pdsdVerbose, "pdsd: handle_http_request enter\n");
  

  
  while (!done) {
    actual = trans->read_to_buffer_func (CMget_static_trans_services(),
                                         trans->trans_data,
                                         &c, 1, 0);
    if (actual != 1) {
      /*
       *  well, we didn't even really get started, did we
       */
      pdsTrace_out (pdsdVerbose, "pdsd: handle_http_request couldn't perform transport read\n");
      return;
    }

    /*  state machine to check for end of request  */
    if (c == '\n') {
      if (state0) 
        done = 1;
      else 
        state0 = 1;
    } else {
      state0 = 0;
    }

    buffer[pos++] = c;
  }
  buffer[pos] = 0;

  pdsTrace_out (pdsdVerbose, "pdsd: handle_http_request request text is [%s]\n", buffer);

  pdsTrace_out (pdsdVerbose, "pdsd: handle_http_request exit\n");

}

/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.8  2007/12/24 14:40:29  eisen
 *  Partial conversion to FFS-based CMRPC.
 *
 *  Revision 1.7  2007/12/04 21:06:24  pmw
 *  Merge changes from evpath branch.  Also probably includes
 *  Darwin fixes.  Compiles on isleroyale and marquesas, I think
 *  I'm misconfigured somehow for building on gondor.
 *
 *  Revision 1.6.2.1  2007/01/05 22:40:40  pmw
 *  interim commit for evpath changes, new branch
 *
 *  Revision 1.6  2004/05/05 12:20:00  eisen
 *  Fix this so that it compiles with changes to CMNonCMHandler.
 *
 *  Revision 1.5  2003/10/21 19:54:15  eisen
 *  Various tweaks and hacks designed to get this to compile cleanly with
 *  Forte 7.
 *
 *  Revision 1.4  2003/10/20 23:34:10  chaos
 *  K&R doesn't allow variables as array sizes
 *
 *  Revision 1.3  2003/10/07 17:20:27  pmw
 *  fixed references to dump_server_content_wrapper
 *  re-insert errno.h inclusion
 *  CMTransport function spec related changes (again)
 *
 *  Revision 1.2  2003/10/07 15:13:46  chaos
 *  Tweak non-cm handler to answer changes in CM.  Make load.cc compile where we don't have XML and need a header to reference errno.
 *
 *  Revision 1.1  2003/07/29 03:48:52  pmw
 *  initial changes to support HTTP requests
 *
 *
 */
