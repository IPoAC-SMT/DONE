#include <signal.h>
#include <stdlib.h>

#include "./interface_t.h"

typedef struct{
    node_t *nodes;
    link_t *links;
    int nodes_num;
    int links_num;
} simdata_t;

//void setSignalHandling();
//void forcedCLIExit();
void startSimulation(interface_t *simulation, int nodes_num, int links_num);
void openSwitchShell(char* roba);
void openHostShell(char * roba);
void stopSimulation(interface_t *simulation, int nodes_num, int links_num);