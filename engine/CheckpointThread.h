#ifndef _CHECKPOINT_THREAD_H_
#define _CHECKPOINT_THREAD_H_

class CheckpointThread
{    
public:
  // Thread entry point
  static int start (void* arg);
  static int stop(void* arg);
  // sleep between sweeps in seconds  
  static void sleep_period(long period);
  
  

private:
  static void checkpoint();  
  static bool go;
    
private:
  CheckpointThread();
  ~CheckpointThread();
};


#endif
