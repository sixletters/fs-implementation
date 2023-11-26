// Logging macros
#ifndef LOG_H
#define LOG_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Logging macros
#ifndef NDEBUG
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define debug(M, ...)
#endif

#define info(M, ...) fprintf(stderr, "INFO %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define error(M, ...) fprintf(stderr, "ERROR %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#endif