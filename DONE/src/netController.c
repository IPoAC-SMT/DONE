#include "../lib/netController.h"
#include "../lib/netlib.h"

int startSimulation()
{
    printf("Starting simulation\n");
    addNode("nodo1", 'h');
    addNode("nodo2", 'h');
    addSwitch("s1");
    addSwitch("s2");
    addCableBetweenNodeAndSwitch("nodo1", "s1");
    addCableBetweenNodeAndSwitch("nodo2", "s2");
    addCableBetweenSwitches("s1", "s2");
    return 0;
}

int stopSimulation()
{
    printf("Stopping simulation\n");
    delNode("nodo1");
    delNode("nodo2");
    delSwitch("s1");
    delSwitch("s2");
    return 0;
}