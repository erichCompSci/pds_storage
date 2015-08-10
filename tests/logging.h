#ifndef _LOGGING_H
#define _LOGGING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PDS_CONNECT_FILE "/net/hu21/elohrman/pds_connect"
#define LOG_DUMP_DIR "/net/hu21/elohrman/build_pds_storage/dump_dir"

// All of these return 0 on failure, 1 on success

// Initialize log stuff.
// The unique identifier will probably be node_name:group_id:proc_id
int initialize_log(char * log_name, char * unique_ident);

// Log the floating average
int log_average(float average);

// Log some character string
int log_something(char * something);


#endif
