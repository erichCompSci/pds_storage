/*
 *
 *  $Id: trace.c,v 1.6 2007-12-04 21:06:16 pmw Exp $
 *
 */


#include "config.h"

#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdio.h>
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#else
#include <varargs.h>
#endif
#include <errno.h>
extern int vfprintf();

#include "formats.h"


static unsigned long trace_status = 0;
const unsigned long pdsengineVerbose = 0;
const unsigned long pdsagentVerbose = 1;
const unsigned long pdsdVerbose = 2;
const unsigned long pdsDataVerbose = 3;
const unsigned long pdsLastTraceType = 4;

extern int
pdsTrace_on (const unsigned long type)
{
  if (trace_status == 0)
    {
      trace_status |= ((getenv("pdsengineVerbose") != NULL) << pdsengineVerbose);
      trace_status |= ((getenv("pdsagentVerbose") != NULL) << pdsagentVerbose);
      trace_status |= ((getenv("pdsdVerbose") != NULL) << pdsdVerbose);
      trace_status |= ((getenv("pdsDataVerbose") != NULL) << pdsDataVerbose);
      if (getenv("pdsVerbose") != NULL)
        {
          trace_status |= ((1 << pdsLastTraceType) - 1);
        }
    }
  return (trace_status & (1 << type));
}

extern void
pdsTrace_out(const unsigned long trace_type,  char const *format, ...)
{
    va_list ap;
 
    if (pdsTrace_on(trace_type))
      {
#ifdef HAVE_STDARG_H
        va_start(ap, format);
#else
        va_start(ap);
#endif
        vfprintf(stdout, format, ap);
        va_end(ap);
        printf("\n");
      }
}


/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.5.2.1  2007/01/05 22:40:30  pmw
 *  interim commit for evpath changes, new branch
 *
 *  Revision 1.5  2006/10/11 10:56:47  eisen
 *  Fix cmrpc.h and evpath.h things
 *
 *  Revision 1.4  2004/10/14 21:17:30  pmw
 *  Changes to support the CMrpc renaming.
 *
 *  Revision 1.3  2003/10/21 16:03:07  eisen
 *  Changes to avoid warnings and errors and generally make this compile with
 *  SUNWspro7 CC with the -library=stlport4 argument.  Using this correctly
 *  requires setting CXX=CC, CXXFLAGS=-library=stlport4,
 *  CXXCPP="CC -E -library=stlport4".  Chaos is setup to do this now.
 *
 *  Revision 1.2  2002/01/22 03:46:55  pmw
 *  Added CVS headers.
 *
 *
 */
