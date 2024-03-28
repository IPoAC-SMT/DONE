#include <stdio.h>
#include <stdlib.h>
typedef struct
{
    char interfaceHidden;
    char moving_node;
    char node_type;
    char * node_id;
    char placing_node;
    int numnodes;
    int numlink;
    char placing_link;
    char first_place[50];
    void * first_place_nodetype;    // needs to be cast in the future -> into component_type_t
    char dragging_deactivated;
    void * GUIdata;
} settings_t;

void toggleInterface(settings_t *settings);
