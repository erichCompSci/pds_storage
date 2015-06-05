#include "EventWrapper.h"
/*********************************************/
BaseEventWrapper::BaseEventWrapper(void* evt)
  : event_ (evt)
{
}
/*********************************************/
BaseEventWrapper::~BaseEventWrapper()
{
  event_ = 0;
}
/*********************************************/
EntityEventWrapper::EntityEventWrapper(pds_entity_change_event& evt)
  : BaseEventWrapper(&evt),
    entity_type (evt.type)
{
}

EntityEventWrapper::EntityEventWrapper(entity_data_change_ntf& evt)
  : BaseEventWrapper(&evt),
    entity_type (PDS_ENTITY_CHANGE_DATA)
{
}


EntityEventWrapper::EntityEventWrapper(entity_exist_change_ntf& evt)
  : BaseEventWrapper(&evt),
    entity_type (evt.type)
{
}

EntityEventWrapper::EntityEventWrapper(entity_u_bind_change_ntf& evt)
  : BaseEventWrapper(&evt),
    entity_type (evt.type)
{
}
/*********************************************/
const char* EntityEventWrapper::full_name()
{
  return const_cast<char *>("");
}
/*********************************************/
const char* EntityEventWrapper::desc()
{
  return const_cast<char *> ("No more desc");
}
/*********************************************/
int EntityEventWrapper::type()
{
  return entity_type;
}
/*********************************************/
ContextEventWrapper::ContextEventWrapper(pds_context_change_event& evt)
  : BaseEventWrapper(&evt),
    context_fullname (evt.fullname),
    context_type (evt.type)
{
}

ContextEventWrapper::ContextEventWrapper(context_exist_change_ntf& evt)
  : BaseEventWrapper(&evt), 
    context_fullname (evt.fullname),
    context_type (evt.type)
{
}

ContextEventWrapper::ContextEventWrapper(context_u_bind_change_ntf& evt)
  : BaseEventWrapper(&evt),
    context_fullname (evt.fullname),
    context_type (evt.type)
{
}


/*********************************************/
const char* ContextEventWrapper::full_name()
{
  return context_fullname;
}
/*********************************************/
const char* ContextEventWrapper::desc()
{
  return const_cast<char *>("No more desc");
}
/*********************************************/
int ContextEventWrapper::type()
{
  return context_type;
}
/*********************************************/

