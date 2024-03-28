#include "components/node_t.h"
#include "components/button_t.h"
#include "components/link_t.h"

typedef struct {    //GUI data
    button_t * buttons;
    node_t * nodes;
    link_t * links;
} interface_t;

interface_t* init_interface(button_t* buttons, node_t* nodes,link_t * links);
