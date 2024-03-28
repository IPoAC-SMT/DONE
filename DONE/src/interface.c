#include "../lib/interface_t.h"

interface_t* init_interface(button_t* buttons, node_t* nodes,link_t * links) {
    interface_t* interface = (interface_t*) calloc(1,sizeof(interface_t));
    interface->buttons = buttons;
    interface->nodes = nodes;
    interface->links = links;
    return interface;
}
