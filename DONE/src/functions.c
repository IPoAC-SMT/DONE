#include "../lib/settings.h"
#include "../lib/logicalController.h"

void helloworld(settings_t*settings){
    printf("hello world\n");
}

void placeRouter(settings_t*settings){
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 2;
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
