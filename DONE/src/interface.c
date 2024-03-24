#include "../lib/interface_t.h"

interface_t* init_interface(pulsante_t* pulsanti, node_t* nodi,link_t * links) {
    interface_t* interface = (interface_t*) calloc(1,sizeof(interface_t));
    interface->pulsanti = pulsanti;
    interface->nodi = nodi;
    interface->links = links;
    return interface;
}
