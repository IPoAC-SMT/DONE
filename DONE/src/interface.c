#include "../lib/interface_t.h"
#include "../lib/log.h"

interface_t *init_interface(button_t *buttons, node_t *nodes, link_t *links, rectangle_t *rectangles, text_t *texts)
{
    interface_t *interface = (interface_t *)calloc(1, sizeof(interface_t));
    interface->buttons = buttons;
    interface->nodes = nodes;
    interface->links = links;
    interface->rectangles = rectangles;
    interface->texts = texts;
    return interface;
}
