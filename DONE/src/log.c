#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INFO "\033[1;34m"
#define WARNING "\033[1;33m"
#define SUCCESS "\033[1;32m"
#define ERROR "\033[1;31m"

long unsigned int getTime(void)
{
    return (unsigned long)time(NULL);
}

void logInfo(char *message, char *format, ...)
{
    va_list args;
    char *composite = (char *)calloc(strlen(message) + strlen(format) + 50, sizeof(char));
    snprintf(composite, strlen(message) + strlen(format) + 50, "%s %lu\t|    INFO\t| %s\033[0m %s \n", INFO, getTime(), message, format);
    va_start(args, format);
    vprintf(composite, args);
    va_end(args);
    free(composite);
}

void logSuccess(char *message, char *format, ...)
{
    va_list args;
    char *composite = (char *)calloc(strlen(message) + strlen(format) + 50, sizeof(char));
    snprintf(composite, strlen(message) + strlen(format) + 50, "%s %lu\t|    SUCCESS\t| %s\033[0m %s \n", SUCCESS, getTime(), message, format);
    va_start(args, format);
    vprintf(composite, args);
    va_end(args);
    free(composite);
}

void logWarning(char *message, char *format, ...)
{
    va_list args;
    char *composite = (char *)calloc(strlen(message) + strlen(format) + 50, sizeof(char));
    snprintf(composite, strlen(message) + strlen(format) + 50, "%s %lu\t|    WARNING\t| %s\033[0m %s \n", WARNING, getTime(), message, format);
    va_start(args, format);
    vprintf(composite, args);
    va_end(args);
    free(composite);
}

void logError(char *message, char *format, ...)
{
    va_list args;
    char *composite = (char *)calloc(strlen(message) + strlen(format) + 50, sizeof(char));
    snprintf(composite, strlen(message) + strlen(format) + 50, "%s %lu\t|    ERROR\t| %s\033[0m %s \n", ERROR, getTime(), message, format);
    va_start(args, format);
    vprintf(composite, args);
    va_end(args);
    free(composite);
}
