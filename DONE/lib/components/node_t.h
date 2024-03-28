typedef enum {hub_t,switch_t,router_t,host_t,external_interface_t,external_natted_interface_t} component_type_t;

#define NAMELENGTH 50

typedef struct {
    char* name;
    component_type_t type;
    int x,y;
} node_t;
