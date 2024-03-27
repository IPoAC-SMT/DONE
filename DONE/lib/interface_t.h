#include "components/node_t.h"
#include "components/pulsante_t.h"
#include "components/link_t.h"

typedef struct {    //GUI data
    pulsante_t * pulsanti;
    node_t * nodi;
    link_t * links;
} interface_t;

interface_t* init_interface(pulsante_t* pulsanti, node_t* nodi,link_t * links);
