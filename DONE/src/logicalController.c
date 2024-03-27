#include "../lib/logicalController.h"
#include "../lib/netController.h"
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

    for(int i = 0; i < nodes_num;i i++){
        node_t *current_node = simdata.nodes[i];
        
        switch(current_node->tipo){     // handling every node type differently
            case host_t:
                addNode(current_node->nome,'h');
                break;
            case hub_t:
                break;
            case switch_t:
                break;
            case router_t:
                break;
            case external_interface_t:
                break;
            case external_natted_interface_t:
                break;
        }
    }

}


void openSwitchShell(char* roba){

}

void openHostShell(char * roba){
    
}