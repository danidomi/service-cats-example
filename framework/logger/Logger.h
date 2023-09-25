#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#ifndef LOGGER_H
#define LOGGER_H
#define LOG_FILE "stderr"
#endif

// Define log levels
typedef enum {
    DEBUG,
    INFO,
    WARNING,
    ERROR
} LogLevel;

void logMessage(LogLevel level, const char *format, ...);