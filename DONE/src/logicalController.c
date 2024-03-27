#include "../lib/logicalController.h"
#include "../lib/netlib.h"


void setSignalHandling()
{
    signal(SIGINT, forcedCLIExit);
}

void forcedCLIExit()
{
    stopSimulation();
    exit(0);
}

// functions that interact with data collected by the GUI and pass it to the lower level (docker)

void sendDataToDocker(interface_t *simulation, int nodes_num, int links_num)
{
    simdata_t simdata;      // filling simulation struct
    simdata.nodes = simulation->nodi;
    simdata.links = simulation->links;
    simdata.nodes_num = nodes_num;
    simdata.links_num = links_num;

    initEnv();

    // actually sending data to docker

    for(int i = 0; i < nodes_num;i i++){    // handling nodes
        node_t *current_node = simdata.nodes[i];
        
        switch(current_node->tipo){     // handling every node type differently depending on node type
            case host_t:
                addNode(current_node->nome,'h');
                break;
            case hub_t:     // TODO: wait for hub code 
                break;
            case switch_t:
                addNode(current_node->nome);
                break;
            case router_t:
                addNode(current_node->nome,'r');
                break;
            case external_interface_t:
                addExternalInterface(current_node->nome, "");   // TODO: @tiz314 - handling strings
                break;
            case external_natted_interface_t:
                addExternalInterface(current_node->nome, "");   // TODO: @tiz314 - handling strings
                break;
        }
    }

    for(int i = 0; i < links_num; i++){
        
    }

}


void openSwitchShell(char* roba){

}

void openHostShell(char * roba){

}

void startSimulation(){
    // starting simulation
}

void stopSimulation(){
    // stopping simulation
}