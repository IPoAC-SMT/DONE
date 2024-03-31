#include <stdio.h>
#include <stdlib.h>
typedef struct
{
    char interfaceHidden;
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
    // TODO there was something else
    /* end things for rectangles */
    char *openProjectName;
} settings_t;

void toggleInterface(settings_t *settings);
