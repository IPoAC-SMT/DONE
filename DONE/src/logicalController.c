#include "../lib/logicalController.h"
#include "../lib/netlib.h"
#include "../lib/log.h"

interface_t *lastSimulation = NULL;
int lastNodesNum, lastLinksNum;

void initEnv()
{

    signal(SIGINT, forcedCLIExit);
    signal(SIGTERM, forcedCLIExit);
    signal(SIGQUIT, forcedCLIExit);
    signal(SIGKILL, forcedCLIExit);
    signal(SIGSTOP, forcedCLIExit);
    signal(SIGTSTP, forcedCLIExit);
    signal(SIGABRT, forcedCLIExit);
    signal(SIGSEGV, forcedCLIExit);

    createNetnsDir();
}

void forcedCLIExit()
{
    if (lastSimulation != NULL)
    {
        stopSimulation(lastSimulation, lastNodesNum, lastLinksNum);
    }
    exit(0);
}

// functions that interact with data collected by the GUI and pass it to the lower level (docker)

void startSimulation(interface_t *simulation, int nodes_num, int links_num)
{
    interfaces *availableInterfaces = getNetInterfaces();

    simdata_t simdata; // filling simulation struct
    simdata.nodes = simulation->nodes;
    simdata.links = simulation->links;
    simdata.nodes_num = nodes_num;
    simdata.links_num = links_num;

    lastSimulation = simulation;
    lastNodesNum = nodes_num;
    lastLinksNum = links_num;

    logInfo("Starting simulation","");

    // actually sending data to docker

    for (int i = 0; i < nodes_num; i++)
    { // handling nodes
        node_t *current_node = &simdata.nodes[i];

        switch (current_node->type)
        { // handling every node type differently depending on node type
        case host_t:
            addNode(current_node->name, 'h');
            break;
        case switch_t:
            addSwitch(current_node->name);
            break;
        case router_t:
            addNode(current_node->name, 'r');
            break;
        case external_interface_t:
            addExternalInterface(current_node->name, availableInterfaces->interfaces_name[1]);
            break;
        case external_natted_interface_t:
            addExternalInterface(current_node->name, availableInterfaces->interfaces_name[1]);
            break;
        }
    }

    for (int i = 0; i < links_num; i++)
    { // handling links
        link_t *current_link = &simdata.links[i];

        // differentiating between different kinds of connections
        if (current_link->node1_type == switch_t && current_link->node2_type == switch_t)
        {
            // 1. switch-switch
            addCableBetweenSwitches(current_link->node1, current_link->node2);
        }
        else if (current_link->node1_type == switch_t)
        {
            // 2. switch-node
            addCableBetweenNodeAndSwitch(current_link->node2, current_link->node1);
        }
        else if (current_link->node2_type == switch_t)
        {
            // 3. node-switch
            addCableBetweenNodeAndSwitch(current_link->node1, current_link->node2);
        }
        else
        {
            // 4. every other case is node-node
            addCableBetweenNodes(current_link->node1, current_link->node2);
        }
    }
}

void openNodeShellWrapper(char *node_name)
{
    openNodeShell(node_name);
}

void openSwitchShellWrapper()
{
    openSwitchShell();
}

void sendNodeCommand(char *name, char *command)
{
    sendNodeSetupCommand(name, command);
}

void sendSwitchCommand(char *command)
{
    sendSwitchSetupCommand(command);
}

void stopSimulation(interface_t *simulation, int nodes_num, int links_num)
{
    simdata_t simdata; // filling simulation struct
    simdata.nodes = simulation->nodes;
    simdata.links = simulation->links;
    simdata.nodes_num = nodes_num;
    simdata.links_num = links_num;

    logInfo("Stopping simulation","");

    for (int i = 0; i < nodes_num; i++)
    { // handling nodes is enough, all links between them will be deleted automatically
        node_t *current_node = &simdata.nodes[i];

        if (current_node->type == switch_t)
        {
            delSwitch(current_node->name);
        }
        else
        {
            delNode(current_node->name);
        }
    }

    lastSimulation = NULL; // to avoid errors while closing from cli
}
