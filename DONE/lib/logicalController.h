#include <signal.h>
#include <stdlib.h>

#include "./interface_t.h"

typedef struct{
    node_t *nodes;
    link_t *links;
    int nodes_num;
    int links_num;
} simdata_t;

void initEnv();
void forcedCLIExit();
void startSimulation(interface_t *simulation, int nodes_num, int links_num);
void openSwitchShellWrapper();
void openNodeShellWrapper(char * node_name);
void stopSimulation(interface_t *simulation, int nodes_num, int links_num);