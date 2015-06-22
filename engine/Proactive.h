#ifndef _PROACTIVE_H_
#define _PROACTIVE_H_
/*
 *
 *  $Id: Proactive.h,v 1.6 2008-01-01 00:38:18 pmw Exp $
 *
 */

extern "C"
{
#include "evpath.h"
};

#include "engine/storage_stone.h"
#include "agent/pdsdefs.h"
#include <vector>
#include <utility>
#include <map>

extern std::map<int, FMStructDescRec *> global_format_map;


class Proactive
{
 public:

  virtual void set_up_stone(int which_event);
  virtual EVstone add_target( attr_list attrs, EVstone remote_stone, unsigned short which_event );
  virtual void send_event_ (void *ev, unsigned short which_event);

 private:
  int determine_stone_set_up(unsigned short which_event);
  std::vector<EVstone> stones_;
  std::vector<EVaction> splits_;
  std::vector<EVsource> source_handles_;

 public:

  static void use_this_CM (CManager cm);
  int set_aggregate_stone (char * cod_func,unsigned int which_type);


 protected:

  Proactive ();
  virtual ~Proactive();

  std::vector<std::pair<unsigned short, FMStructDescRec *> > formats_list_;
  static CManager server_cm_;

};  

/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.4  2007/12/04 21:06:20  pmw
 *  Merge changes from evpath branch.  Also probably includes
 *  Darwin fixes.  Compiles on isleroyale and marquesas, I think
 *  I'm misconfigured somehow for building on gondor.
 *
 *  Revision 1.3.2.2  2007/01/09 01:37:29  pmw
 *  various changes including a fix for the delete context server hang
 *
 *  Revision 1.3.2.1  2007/01/06 08:55:50  pmw
 *  *** empty log message ***
 *
 *  Revision 1.3  2006/10/11 02:01:26  chaos
 *  Change so we don't have include of evpath.h in cmrpc.h
 *
 *  Revision 1.2  2003/01/30 19:16:26  mansour
 *  Add support for domain events
 *
 *  Revision 1.1  2001/10/09 21:37:55  pmw
 *  configuration changes
 *
 *  Revision 1.3  2001/10/07 22:19:15  pmw
 *  Added domain_change event structures so that Proactive class is always correctly
 *  initialized.
 *
 *  Revision 1.2  2001/07/02 04:02:03  pmw
 *  interim checkin for changes to make Contexts also support events.  compiles
 *  but tests probably do not run.
 *
 *  Revision 1.1.1.1  2000/12/13 22:14:21  pmw
 *  imported sources
 *
 *  Revision 1.5  2000/11/29 04:37:36  pmw
 *  changed channel-handling again, Entity now performs most of its own
 *  change event sends
 *
 *  Revision 1.4  2000/11/28 22:49:27  pmw
 *  1) moved CManager stuff to Proactive class, since other classes
 *  don't need to deal with it any longer
 *  2) moved object ID management out of Domain and into objectId.
 *
 *  Revision 1.3  2000/11/22 21:42:35  pmw
 *  proactivity based on entity events now appears to work correctly.  In particular,
 *  creation events based on the prior registration of placeholder entities and their
 *  later "creation" as real entities works.
 *
 *  Revision 1.2  2000/11/21 21:35:25  pmw
 *  Domain change events now work correctly.  moved event transmission
 *  responsibilities to Proactive class.
 *
 *  Revision 1.1  2000/11/21 00:23:47  pmw
 *  Since I didn't have enough to do, a little code reorganization to keep
 *  things from getting dramatically out of hand.  Fixed a couple of
 *  bugs-in-waiting.
 *
 *
 */
#endif
