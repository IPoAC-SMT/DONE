#include "../lib/logicalController.h"

void helloworld(settings_t*settings){
    printf("%d\n", settings->numlink);
}

void placehub(settings_t*settings){
    if(settings->isSimulating) return;
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 0;
}

void placeswitch(settings_t*settings){
    if(settings->isSimulating) return;
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 1;
}

void placerouter(settings_t*settings){
    if(settings->isSimulating) return;
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 2;
}

void placehost(settings_t*settings){
    if(settings->isSimulating) return;
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 3;
}

void placeexternalinterface(settings_t*settings){
    if(settings->isSimulating) return;
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 4;
}

void placeexternalnattedinterface(settings_t*settings){
    if(settings->isSimulating) return;
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 5;
}

void placelink(settings_t*settings){
    if(settings->isSimulating) return;
    settings->moving_node = 0;
    settings->placing_node = 0;
    settings->placing_link = 1;
}

void start(settings_t*settings) {    // sending data to logical controller, that starts the simulation
    settings->isSimulating = 42;
    startSimulation((interface_t *)settings->GUIdata,settings->numnodes,settings->numlink);
}

void stop(settings_t*settings){    // sending data, again, to logical controller, that stops the simulation
    settings->isSimulating = 0;
    stopSimulation((interface_t *)settings->GUIdata,settings->numnodes,settings->numlink);
}

void openShell(settings_t*settings){
    settings->node_type == switch_t ? openSwitchShellWrapper() : openNodeShellWrapper(settings->node_name);
}
