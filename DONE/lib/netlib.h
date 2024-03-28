#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_COMMAND_SIZE 300
#define MAX_NAME_SIZE 50
#define MAX_PID_SIZE 10

int initEnv();

int addNode(char *name, char type);
int delNode(char *name);

int addExternalInterface(char *name, char *interface);

int addSwitch(char *name);
int delSwitch(char *name);

int addCableBetweenNodes(char *firstNode, char *secondNode);
int delCableBetweenNodes(char *firstNode, char *secondNode);

int addCableBetweenNodeAndSwitch(char *nodeName, char *switchName);
int delCableBetweenNodeAndSwitch(char *nodeName, char *switchName);

int addCableBetweenSwitches(char *firstSwitch, char *secondSwitch);
int delCableBetweenSwitches(char *firstSwitch, char *secondSwitch);

int sendNetworkSetupCommand(char *name, char *command);
int openNodeShell(char *name);