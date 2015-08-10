#include "logging.h"

static FILE * log_fd;
char * proc_ident;

int initialize_log(char * log_name)
{
  char temp_ptr[250];
  strcpy(temp_ptr, LOG_DUMP_DIR);
  strcat(temp_ptr, log_name);
  strcat(temp_ptr, ".expr");

  proc_ident = strdup(log_name);

  printf("Log file path: %s\n", temp_ptr);

  log_fd = fopen(temp_ptr, "w");
  if (!log_fd || !proc_ident)
    return 0;    
  else
    return 1;
}

int log_average(float average)
{
  if(fprintf(log_fd, "%s\tAverage: %f\n", proc_ident, average) < 0)
  {
    fprintf(stderr, "Error: log_average did not log!\n");
    return 0;
  }
  return 1;
}

int log_something(char * something)
{
  if(fprintf(log_fd, "%s\n", something) < 0)
  {
    fprintf(stderr, "Error: log_something did not log!\n");
    return 0;
  }
  return 1;
}
