#ifndef LOGGER
#define LOGGER

#include "libs.h"

#define LOG_TEXT(f) DEBUG ? fprintf(stderr, f) : 1

#define LOG_INFO(f, ...) DEBUG ? fprintf(stderr,f, __VA_ARGS__) : 1

#endif