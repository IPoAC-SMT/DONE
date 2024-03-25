#include "../lib/settings.h"
#include "../lib/logicalController.h"

void helloworld(settings_t*settings){
    printf("hello world\n");
}

void tempStart(settings_t*settings){
    startSimulation();
}

void tempStop(settings_t*settings){
    stopSimulation();
}

void openShell(char *name){
    openShellWrapper(name);
}

/*
 *     
    startSimulation();
    stopSimulation();

 * */
