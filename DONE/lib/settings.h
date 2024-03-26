#include <stdio.h>
#include <stdlib.h>
typedef struct
{
    char interfaceHidden;
    char moving_node;
    char node_type;
    char * node_id;
    char placing_node;
} settings_t;

void toggleInterface(settings_t *settings);
