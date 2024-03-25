#include "../lib/logicalController.h"
#include "../lib/netController.h"


void setSignalHandling()
{
    signal(SIGINT, forcedCLIExit);
}

void forcedCLIExit()
{
    stopSimulation();
    exit(0);
}

void openShellWrapper(char *name){
    openNodeShell(name);
}