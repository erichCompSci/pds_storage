#ifndef _CLEANING_THREAD_H_
#define _CLEANING_THREAD_H_

class CleaningThread
{    
public:
  // Thread entry point
  static int start (void* arg);

  static int stop( void* arg );
  
  // sleep between sweeps in seconds  
  static void sleep_period(long period);
  // Set the timeout period
  static void timeout(long  to);
  // Set the hearbeat entity name
  static void heartbeat_entity_name(const char* entity_name);

private:
  static void cleanup();  
    
  static bool go;

private:
  CleaningThread();
  ~CleaningThread();
};


#endif
