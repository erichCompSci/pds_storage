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

Proactive::Proactive ()
{}

Proactive::~Proactive()
{
  for(int i = 0; i < source_handles_.size(); ++i)
  {
    EVfree_source (source_handles_[i]);
  }

  for(int i = 0; i < stones_.size(); ++i)
  {
    EVfree_stone( server_cm_, stones_[i]);
  }

}

void
Proactive::use_this_CM (CManager cm)
{
  Proactive::server_cm_ = cm;
}


void
Proactive::set_up_stone(int which_event)
{
  
    stones_.push_back(EValloc_stone(server_cm_));
    splits_.push_back(EVassoc_split_action(server_cm_, stones_.back(), NULL));
    source_handles_.push_back(EVcreate_submit_handle(server_cm_, stones_.back(), global_format_map[which_event]));
    
}

int
Proactive::determine_stone_set_up(unsigned short which_event)
{

  //We know the format list
  if(formats_list_.empty())
  {
    return -1; 
  }

  int i;
  for(i = 0; i < formats_list_.size(); ++i)
  {
    if(formats_list_[i].first == which_event)
        break;
  }
  
  if(i == formats_list_.size())
  {
    return -1;
  }
  
  return i;
}

EVstone
Proactive::add_target( attr_list contact_list, EVstone remote_stone, unsigned short which_event ) 
{
  int which_target = determine_stone_set_up(which_event);
  if(which_target < 0)
  {
    set_up_stone(which_event);
  }
  // You have to call again, because this might be different depending on order of calls
  which_target = determine_stone_set_up(which_event);

  EVstone s = EValloc_stone( server_cm_ );
  EVassoc_bridge_action( server_cm_, s, contact_list, remote_stone );


  EVaction_add_split_target( server_cm_, stones_[which_target], splits_[which_target], s );

  return s;
}
 
int
Proactive::set_aggregate_stone(char * cod_func, unsigned int which_type)
{
  EVstone s = EValloc_stone(server_cm_);
  int which_target = determine_stone_set_up(which_type);
  if(which_target < 0)
  {
    set_up_stone(which_type);
    which_target = determine_stone_set_up(which_type);
  }
  
  //Set up storage stone
  FMStructDescList storage_list[] = { formats_list_[which_target].fm_format, NULL };
  char * action_spec = create_e_rolling_bucket_action_spec(storage_list, 5, cod_func);
  if(!action_spec)
  {
    fprintf(stderr, "Error: failure in the action spec creation for aggregate stone\n");
    return 0;
  }
  EVassoc_multi_action(server_cm_, s, action_spec, NULL);
  
  //Connect storage stone to split stone
  EVstone_set_output(server_cm_, s, 0, stones_[which_target]);

  //Free source handle
  EVfree_source (source_handles_[which_target]);

  //Generate new source handle connecting to storage stone
  source_handles_[which_target] = EVcreate_submit_handle( server_cm_, s, formats_list_[which_target].fm_format);

  if(!source_handles_[which_target])
  {
    fprintf(stderr, "Error: failure in creating the submit_handle for aggregate stone\n");
    return 0;
  }

  return 1;
  
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
