/*
 *
 *  $Id: Proactive.cc,v 1.8 2008-06-26 19:24:17 pmw Exp $
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Proactive.h"

#include "common/formats.h"

CManager Proactive::server_cm_ = static_cast<CManager> (0);

Proactive::Proactive (handler_tag_FMStructDescRec * formats_)
  : stones_ (NULL),
    source_handles_ (NULL),
    formats_list_ (formats_)
{}


Proactive::~Proactive()
{
  for(int i = 0; source_handles_[i] != NULL; ++i)
  {
    EVfree_source (source_handles_[i]);
  }

  for(int i = 0; stones_[i] != -1; ++i)
  {
    EVfree_stone( server_cm_, stones_[i]);
  }


  /*if (source_handle_ != NULL) {
    EVfree_source( source_handle_ );
    // also need to remove stone action?
  }
  if (stone_ != -1) {
    EVfree_stone( server_cm_, stone_ );
    
      -- pending resolution from eisen about semantics

      EVdrain_stone( server_cm_, stone_ );
      EVdestroy_stone( server_cm_, stone_ );
    
  }*/
}

void
Proactive::use_this_CM (CManager cm)
{
  Proactive::server_cm_ = cm;
}


void
Proactive::set_up_stones()
{
  if (stones_ == NULL) {

    int i = 0;

    for(i = 0; formats_list_[i].fm_format != NULL; ++i);
    assert((i != 0) && "There is nothing in the formats_list but a Null value");

    stones_ = (EVstone *)malloc(sizeof(EVstone) * (i + 1));
    splits_ = (EVaction *)malloc(sizeof(EVaction) * (i + 1));
    source_handles_ = (EVsource *)malloc(sizeof(EVsource) * (i + 1));
    if(!stones_)
    {
      fprintf(stderr, "Error: malloc failed and stones_ is NULL\n");
      //exit(1);
    }

    if(!splits_)
    {
      fprintf(stderr, "Error: malloc failed and splits_ is NULL\n");
      //exit(1);
    }

    if(!source_handles_)
    {
        fprintf(stderr, "Error: malloc failed and source_handles_ is NULL\n");
        //exit(1);
    }
    if((!stones_) || (!splits_) || (!source_handles_))
    {
      fprintf(stderr, "Error: malloc failed to return pointer in Proactive set_up_stones\n");
      exit(1);
    }

    for(int j = 0; j < i; ++j)
    {
      stones_[j] = EValloc_stone( server_cm_);
      splits_[j] = EVassoc_split_action( server_cm_, stones_[j], NULL);
      source_handles_[j] = EVcreate_submit_handle( server_cm_, stones_[j], formats_list_[j].fm_format);
    }
    
    *(stones_ + i) = -1;
    *(splits_ + i) = -1;
    *(source_handles_ + i) = NULL;
    
  } 

}

int
Proactive::determine_correct_stone(unsigned short which_event)
{
  int i;
  for(i = 0; formats_list_[i].fm_format != NULL; ++i)
  {
    if(formats_list_[i].handler_type == which_event)
        break;
  }
  
  if(formats_list_[i].fm_format == NULL)
  {
    fprintf(stderr, "Error: Someone asked to channel a non-existent or incorrect type\n");
    //exit(1);
  }
  
  return i;
}

EVstone
Proactive::add_target( attr_list contact_list, EVstone remote_stone, unsigned short which_event ) 
{
  set_up_stones();
  EVstone s = EValloc_stone( server_cm_ );
  EVassoc_bridge_action( server_cm_, s, contact_list, remote_stone );

  int which_target = determine_correct_stone(which_event);

  EVaction_add_split_target( server_cm_, stones_[which_target], splits_[which_target], s );
  return s;
}
 

void 
Proactive::send_event_ (void *ev, unsigned short which_event)
{
  int which_source = determine_correct_stone(which_event);
  EVsubmit( source_handles_[which_source], ev, NULL );
}

/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.7  2008/01/01 00:38:18  pmw
 *  Changes to make PDS work with the new FFS libraries
 *
 *  Revision 1.5  2007/12/04 21:06:20  pmw
 *  Merge changes from evpath branch.  Also probably includes
 *  Darwin fixes.  Compiles on isleroyale and marquesas, I think
 *  I'm misconfigured somehow for building on gondor.
 *
 *  Revision 1.4.2.2  2007/01/09 01:37:29  pmw
 *  various changes including a fix for the delete context server hang
 *
 *  Revision 1.4.2.1  2007/01/05 22:40:34  pmw
 *  interim commit for evpath changes, new branch
 *
 *  Revision 1.4  2006/10/11 12:29:00  chaos
 *  cm.h issues
 *
 *  Revision 1.3  2003/01/30 19:16:26  mansour
 *  Add support for domain events
 *
 *  Revision 1.2  2003/01/29 03:36:39  mansour
 *  A bunch of changes to make get_full_name work. Add a my_name_ at the ContextBindable level. This really helps for debugging only and can be removed. Fixed the algorithm for get_full_name.
 *
 *  Also, added calls to generate events on context_creation and deletion.
 *
 *  Revision 1.1  2001/10/09 21:37:55  pmw
 *  configuration changes
 *
 *  Revision 1.3  2001/09/10 03:55:42  pmw
 *  client lib is now libpdsagent, and is compiled entirely with gcc.  libpdsutil no
 *  longer exists.
 *
 *  Revision 1.2  2001/07/02 04:02:03  pmw
 *  interim checkin for changes to make Contexts also support events.  compiles
 *  but tests probably do not run.
 *
 *  Revision 1.1.1.1  2000/12/13 22:14:23  pmw
 *  imported sources
 *
 *  Revision 1.8  2000/11/30 15:13:17  pmw
 *  fixed problem where server hung because i called ECho_CM_init twice on the
 *  same CM value.
 *
 *  Revision 1.7  2000/11/29 16:16:03  pmw
 *  *** empty log message ***
 *
 *  Revision 1.6  2000/11/29 04:47:03  pmw
 *  duh.  ifdef-ed out old send-event code.  may come back later.
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
 *  Revision 1.1  2000/11/21 00:23:46  pmw
 *  Since I didn't have enough to do, a little code reorganization to keep
 *  things from getting dramatically out of hand.  Fixed a couple of
 *  bugs-in-waiting.
 *
 *
 */
