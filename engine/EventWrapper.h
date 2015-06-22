#ifndef EVENT_WRAPPER_H_
#define EVENT_WRAPPER_H_


extern "C" {
#include "pdsdefs.h"
};
enum EVENT_TYPE {CONTEXT_CHANGE_EVENT, ENTITY_CHANGE_EVENT};

class BaseEventWrapper
{
public:

  BaseEventWrapper(void* evt);  
  virtual ~BaseEventWrapper();  
  virtual const char* full_name() = 0;
  virtual const char* desc() = 0;
  virtual int         type() = 0;

  void* event() const { return event_; }
  virtual EVENT_TYPE my_type() const = 0;
protected:
  void * event_;  
private:
  BaseEventWrapper(const BaseEventWrapper&);
  const BaseEventWrapper& operator= (const BaseEventWrapper&);
  
  
};
/***********************************************/
class EntityEventWrapper: public BaseEventWrapper
{
public:
  EntityEventWrapper(pds_entity_char_data_change_ntf& evt);
  EntityEventWrapper(pds_entity_int_data_change_ntf & evt);
  EntityEventWrapper(pds_entity_float_data_change_ntf & evt);
  EntityEventWrapper(pds_entity_exist_change_ntf& evt);
  EntityEventWrapper(pds_entity_u_bind_change_ntf& evt);
  virtual const char* full_name();
  virtual const char* desc();
  virtual int         type();
  EVENT_TYPE my_type() const { return ENTITY_CHANGE_EVENT; }
private:
  EntityEventWrapper(const EntityEventWrapper&);
  const EntityEventWrapper& operator=(const EntityEventWrapper&);
  char * entity_fullname;
  const char * descript;
  int entity_type;
};
/***********************************************/
class ContextEventWrapper : public BaseEventWrapper
{
public:
  ContextEventWrapper(pds_context_change_event& evt);
  ContextEventWrapper(pds_context_u_bind_change_ntf& evt);
  ContextEventWrapper(pds_context_exist_change_ntf& evt);
  virtual const char* full_name();
  virtual const char* desc();
  virtual int         type();
  EVENT_TYPE my_type() const { return CONTEXT_CHANGE_EVENT; }
private:
  ContextEventWrapper(const ContextEventWrapper&);
  const ContextEventWrapper& operator=(const ContextEventWrapper&);
  char * context_fullname;
  const char * descript;
  int context_type;
};
/***********************************************/
#endif // EVENT_WRAPPER_H_

