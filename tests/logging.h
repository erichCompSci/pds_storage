#ifndef _LOGGING_H
#define _LOGGING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>

#define PDS_CONNECT_FILE "/net/hu21/elohrman/pds_connect"
//Point the below to the build_directory/dump_dir
#define LOG_DUMP_DIR "/net/hu21/elohrman/Systems/build_pds_storage/dump_dir/"

// All of these return 0 on failure, 1 on success

// Initialize log stuff.
// The unique identifier will probably be node_name:group_id:proc_id
int initialize_log(char * log_name);

// Log the floating average
int log_average(float average);

int log_rusage(struct rusage * usage);

// Log some character string
int log_something(char * something);


#endif
