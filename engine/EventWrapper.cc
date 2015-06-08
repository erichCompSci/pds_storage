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
    entity_type (evt.type),
    descript ("Old entity change event")
{
}

EntityEventWrapper::EntityEventWrapper(pds_entity_data_change_ntf& evt)
  : BaseEventWrapper(&evt),
    entity_type (PDS_ENTITY_CHANGE_DATA),
    descript ("Entity data change event")
{
}


EntityEventWrapper::EntityEventWrapper(pds_entity_exist_change_ntf& evt)
  : BaseEventWrapper(&evt),
    entity_type (evt.type),
    descript ("Entity existence change event")
{
}

EntityEventWrapper::EntityEventWrapper(pds_entity_u_bind_change_ntf& evt)
  : BaseEventWrapper(&evt),
    entity_type (evt.type),
    descript ("Entity bind or unbind change event")
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
  return const_cast<char *> (descript);
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
    context_type (evt.type),
    descript ("Old context change event")
{
}

ContextEventWrapper::ContextEventWrapper(pds_context_exist_change_ntf& evt)
  : BaseEventWrapper(&evt), 
    context_fullname (evt.fullname),
    context_type (evt.type),
    descript ("Context existence change event")
{
}

ContextEventWrapper::ContextEventWrapper(pds_context_u_bind_change_ntf& evt)
  : BaseEventWrapper(&evt),
    context_fullname (evt.fullname),
    context_type (evt.type),
    descript ("Context bind or unbind change event")
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
  return const_cast<char *> (descript);
}
/*********************************************/
int ContextEventWrapper::type()
{
  return context_type;
}
/*********************************************/

