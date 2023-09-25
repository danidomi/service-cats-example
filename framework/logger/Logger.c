#include "Logger.h"

// Function to get the current timestamp
void getCurrentTime(char *timestamp, size_t size) {
    time_t rawTime;
    struct tm *timeInfo;

    time(&rawTime);
    timeInfo = localtime(&rawTime);

    strftime(timestamp, size, "%Y-%m-%d %H:%M:%S", timeInfo);
}

// Function to log a message
void logMessage(LogLevel level, const char *format, ...) {
    va_list args;
    va_start(args, format);

    char timestamp[20];
    getCurrentTime(timestamp, sizeof(timestamp));
    FILE *logFile;
    if (strcmp(LOG_FILE, "stderr") == 0){
        logFile = stderr;
    } else{
        logFile = fopen(LOG_FILE, "a"); // Open or create the log file in append mode
    }

    if (logFile == NULL) {
        perror("Error opening log file");
        return;
    }

    fprintf(logFile, "[%s] ", timestamp);

    switch (level) {
        case DEBUG:
            fprintf(logFile, "[DEBUG] ");
            break;
        case INFO:
            fprintf(logFile, "[INFO] ");
            break;
        case WARNING:
            fprintf(logFile, "[WARNING] ");
            break;
        case ERROR:
            fprintf(logFile, "[ERROR] ");
            break;
    }

    vfprintf(logFile, format, args);
    fprintf(logFile, "\n");

    fclose(logFile);
    va_end(args);
}