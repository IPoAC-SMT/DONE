typedef enum {hub_t,switch_t,router_t,host_t,external_interface_t,external_natted_interface_t} component_type_t;

#define NAMELENGTH 50

typedef struct {
    char* nome;
    component_type_t tipo;
    int x,y;
} node_t;
