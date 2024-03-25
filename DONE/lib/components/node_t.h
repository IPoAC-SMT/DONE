typedef enum {hub_t,switch_t,router_t,host_t,external_interface_t,boh_altro_non_so} tipo_componente_t;

typedef struct {
    char * nome;
    tipo_componente_t tipo;
    int x,y;
} node_t;
