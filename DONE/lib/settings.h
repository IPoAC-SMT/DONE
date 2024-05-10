#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "rw-zem.h"

typedef struct
{
    char *deviceName;
    char *bindingInterfaceName;
} binding_t;

typedef struct
{
    rwlock_t *settingsLock;
    int absoluteCount;
    char moving_node;
    char node_type;
    char *node_name;
    char placing_node;
    int numnodes;
    int numlink;
    char placing_link;
    char first_place[50];
    void *first_place_nodetype; // needs to be cast in the future -> into component_type_t
    char dragging_deactivated;
    void *GUIdata;
    char isSimulating;
    char drawing_rectangle;
    /* begin things for rectangles */
    int posX;
    int posY;
    /* end things for rectangles */
    char *openProjectName;
    /*for file opening*/
    char *filename;
    char gettingName;
    char resetName;
    char deletingNodes;
    char ipoac;
    int numrectangles;
    /*for multiple choices*/
    int numOptions;
    int chosenOption;
    char *chosenNode;
    char **options;
    binding_t *interfaceBindings;
    int numBindings;
    char exportDoneScript;
    int numTexts;
    int tmpx, tmpy;
    char *tmpText;
    char placing_text;
    char isClient;
    //char *serverIP; // funny solution: uint_8 serverIP[4];
    char *serverIP;
    char validIP;
    char* tmpIp;
    char gettingIp;
} settings_t;
