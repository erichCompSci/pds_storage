#include "EngineGuard.h"
#include <assert.h>
#include "../config.h"
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
}

static  pthread_rwlock_t _rw_lock;
static int _init = 0;


/* This API is defined in load.cc
   its main purpose is to invalidate the
   cache XML document */
//extern void  reset_xml_doc();
/***********************************************
  Function:

  Notes:

************************************************/
void ReaderGuard::initialize()
{
  if (_init != 0)
    return;

  pthread_rwlock_init(&_rw_lock, 0);
  _init = 1;
}
/***********************************************
  Function:

  Notes:

************************************************/
ReaderGuard::ReaderGuard()
{
  assert(_init != 0);
  pthread_rwlock_rdlock(&_rw_lock);
}
/***********************************************
  Function:

  Notes:

************************************************/
ReaderGuard::~ReaderGuard()
{
  assert(_init != 0);
  pthread_rwlock_unlock(&_rw_lock);
}

 
/***********************************************
  Function:

  Notes:

************************************************/
WriterGuard::WriterGuard()
{
  assert(_init != 0);
  pthread_rwlock_wrlock(&_rw_lock);
  /* invalidate the cache XML document */
#ifndef NO_XML
  //reset_xml_doc();
#endif
}
/***********************************************
  Function:

  Notes:

************************************************/
WriterGuard::~WriterGuard()
{
  assert(_init != 0);
  pthread_rwlock_unlock(&_rw_lock);
}

 
