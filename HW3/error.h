#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>


typedef enum error_t {
    OK = 0,
    ERROR_ARG = -1,
    ERROR_UNKNOWN = -2,
    ERROR_COMMAND_HELP = -101,
    ERROR_COMMAND_TIME = -102,
    ERROR_COMMAND_TASK = -103,
    ERROR_COMMAND_ECHO = -104,
    ERROR_COMMAND_EVENT = -105,
    ERROR_COMMAND_START_VIEW = -106,
} error_t;

#endif