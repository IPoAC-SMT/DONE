#include<stdio.h>
#include<stdarg.h>
#include<stdlib.h>
#include<string.h>

#define INFO "\033[1;34m"
#define WARNING "\033[1;33m"
#define SUCCESS "\033[1;32m"
#define ERROR "\033[1;31m"

void logInfo(char * message, char * format, ...){
    va_list args;
    char * composite = (char*) calloc(strlen(message)+strlen(format)+50,sizeof(char));
    snprintf(composite,strlen(message)+strlen(format)+50,"%sINFO\t| %s\033[0m %s \n",INFO, message,format);
    vprintf(composite,args);
    free(composite);
}

void logSuccess(char * message, char * format, ...){
    va_list args;
    char * composite = (char*) calloc(strlen(message)+strlen(format)+50,sizeof(char));
    snprintf(composite,strlen(message)+strlen(format)+50,"%sSUCCESS\t| %s\033[0m %s \n",SUCCESS, message,format);
    vprintf(composite,args);
    free(composite);
}

void logWarning(char * message, char * format, ...){
    va_list args;
    char * composite = (char*) calloc(strlen(message)+strlen(format)+50,sizeof(char));
    snprintf(composite,strlen(message)+strlen(format)+50,"%sWARNING\t| %s\033[0m %s \n",WARNING, message,format);
    vprintf(composite,args);
    free(composite);
}

void logError(char * message, char * format, ...){
    va_list args;
    char * composite = (char*) calloc(strlen(message)+strlen(format)+50,sizeof(char));
    snprintf(composite,strlen(message)+strlen(format)+50,"%sERROR\t| %s\033[0m %s \n",ERROR, message,format);
    vprintf(composite,args);
    free(composite);
}
