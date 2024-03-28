#include "../lib/logicalController.h"

void helloworld(settings_t*settings){
    printf("hello world\n");
}

void placehub(settings_t*settings){
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 0;
}

void placeswitch(settings_t*settings){
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 1;
}

void placerouter(settings_t*settings){
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 2;
}

void placehost(settings_t*settings){
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 3;
}

void placeexternalinterface(settings_t*settings){
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 4;
}

void placeexternalnattedinterface(settings_t*settings){
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 5;
}

void placelink(settings_t*settings){
    settings->moving_node = 0;
    settings->placing_node = 0;
    settings->placing_link = 1;
}

void sendDataToLogicalController(settings_t*settings) {    // sending data to logical controller
    sendDataToDocker((interface_t *)settings->GUIdata,settings->numnodes,settings->numlink);
}

void tempStart(settings_t*settings){
    startSimulation();
}

void tempStop(settings_t*settings){
    stopSimulation();
}

void openShell(settings_t*settings){
    settings->node_type == switch_t ? openSwitchShell(settings->node_id) : openHostShell(settings->node_id);
}
