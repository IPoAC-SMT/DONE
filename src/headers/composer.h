#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define MAX_COMMAND_SIZE 100
#define MAX_NAME_SIZE 30
#define MAX_PID_SIZE 10

int addNode(char *name, char type);
int delNode(char *name);

int addSwitch(char *name);
int delSwitch(char *name);

int addCableBetweenNodes(char *firstNode, char *secondNode);
int delCableBetweenNodes(char *firstNode, char *secondNode);

int addCableBetweenNodeAndSwitch(char *nodeName, char *switchName);
int delCableBetweenNodeAndSwitch(char *nodeName, char *switchName);