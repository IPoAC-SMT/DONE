typedef enum {hub_t,switch_t,router_t,host_t,boh_altro_non_so} tipo_componente_t;

typedef struct {
    tipo_componente_t tipo;
} node_t;
