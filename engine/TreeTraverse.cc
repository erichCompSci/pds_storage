/*
 *
 *  $Id: TreeTraverse.cc,v 1.6 2007-12-04 21:06:20 pmw Exp $
 *
 */

#include "TreeVisitor.h"
#include "TreeTraverse.h"

#include "Entity.h"
#include "Domain.h"

#include <stack>
enum STACK_STATE {INITIAL, PROCESSED};
struct StackEntry
{
  StackEntry(STACK_STATE state, Context* c, const std::string& path)
    : c_ (c), state_(state), path_(path)
  {}

  STACK_STATE state_;
  Context*    c_;
  string      path_;
};
/******************************************************
*******************************************************/
PDSTreeTraverse::PDSTreeTraverse(AbstractPDSTreeVisitor& visitor)
: visitor_(visitor)
{
}
/******************************************************
*******************************************************/
PDSTreeTraverse::~PDSTreeTraverse()
{
}
/******************************************************
*******************************************************/
void 
PDSTreeTraverse::traverseTree()
{
  vector<Domain*>::iterator i;  
  for (i = Domain::global_domain_list_.begin();
       i != Domain::global_domain_list_.end();
       i++)
    {      
	  Domain*d = *i;
      visitor_.start_domain(*d);
      traverseDomain(*i);
      visitor_.end_domain(*d);
    }
}
/******************************************************
*******************************************************/
void 
PDSTreeTraverse::traverseDomain(Domain* d)
{ 
  std::stack<StackEntry*> context_stack; 
  Context* root = d->root_;
    
  context_stack.push(new StackEntry(INITIAL, root, "/"));

  while (context_stack.size() > 0) {
    StackEntry* top = context_stack.top();
    Context* c = top->c_;

    string path = top->path_;

    if (context_stack.top()->state_ == PROCESSED) {      
      visitor_.end_context(*d, *c, path);
      StackEntry * delete_me = context_stack.top();
      context_stack.pop();
      delete delete_me;
      continue;
    }
    
    visitor_.start_context(*d, *c, path);
    context_stack.top()->state_ = PROCESSED;        


    visitEntities(d, c, path);
    // loop over sub contexts    
    bindingMap& bmap = (bindingMap&)c->get_name_map();
    bindingMap::iterator bi;  
    for (bi = bmap.begin(); bi != bmap.end(); bi++)
      {
        if (((*bi).second.second)->is_placeholder()) continue;
        if ((*bi).second.first == context_binding)
          {                        
            string my_full_name = path;
            if (path != "/") 
               my_full_name += "/";

            my_full_name += (*bi).first;
            Context* c1 = (Context*) (*bi).second.second;            
            context_stack.push(new StackEntry(INITIAL, c1, my_full_name));
          } // if, a context binding
      } // for, iterate over all child contexts        
  }    
  
}
/******************************************************
*******************************************************/
void PDSTreeTraverse::visitEntities(Domain* d, Context* c, const std::string&path)
{
    bindingMap& bmap = (bindingMap&)c->get_name_map();
    bindingMap::iterator bi;  
    for (bi = bmap.begin(); bi != bmap.end(); bi++)
      {
        if (((*bi).second.second)->is_placeholder()) continue;
        if ((*bi).second.first == entity_binding)
          {                        
            Entity * e = (Entity*) (*bi).second.second;
            visitor_.start_entity(*d, *c, *e, path);
            // nothing in between for now
            visitor_.end_entity(*d, *c, *e, path);
          } // if, an entity binding
      } // for, iterate over all entities  
}

/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.5.2.1  2007/01/05 22:40:34  pmw
 *  interim commit for evpath changes, new branch
 *
 *
 */
