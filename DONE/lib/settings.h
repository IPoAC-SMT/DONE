#include <stdio.h>
#include <stdlib.h>
typedef struct
{
    char interfaceHidden;
    char moving_node;
    char node_type;
    char * node_id;
    char placing_node;
    int numnodi;
    int numlink;
    char placing_link;
    char first_place[50];
    char dragging_deactivated;
} settings_t;

void toggleInterface(settings_t *settings);
