#include "../lib/netController.h"
#include "../lib/netlib.h"

int startSimulation()
{
    printf("Starting simulation\n");
    addNode("nodo1", 'h');
    addNode("nodo2", 'h');
    addSwitch("s1");
    addCableBetweenNodeAndSwitch("nodo1", "s1");
    addCableBetweenNodeAndSwitch("nodo2", "s1");
    openNodeShell("nodo1");
    return 0;
}

int stopSimulation()
{
    printf("Stopping simulation\n");
    delNode("nodo1");
    delNode("nodo2");
    delSwitch("s1");
    return 0;
}