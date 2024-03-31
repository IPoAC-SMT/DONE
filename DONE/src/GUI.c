#include "../lib/GUI.h"

void DrawButton(button_t *pulsante, settings_t *settings)
{
    bool hovering;
    if (
        (GetMouseX() >= pulsante->x && GetMouseX() <= (pulsante->x + pulsante->width)) &&
        (GetMouseY() >= pulsante->y && GetMouseY() <= (pulsante->y + pulsante->height)))
    {
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            settings->moving_node = false;
            settings->placing_node = false;
            settings->placing_link = false;
            settings->drawing_rectangle = false;
            hovering = false;
            pulsante->pressed(settings);
        }
        else {
            hovering = true;
            DrawText(pulsante->alt_text,GetMouseX()+20,GetMouseY()+20,STD_FONT_SIZE,GRAY);
        }
    }
    else
        hovering = false;

    DrawRectangleLines(pulsante->x, pulsante->y, pulsante->width, pulsante->height, hovering ? YELLOW : GRAY); // lo creo con colore diverso se ci sto o meno hoverando

    // debug, understand me: printf("HEREEEE\n");

    if (pulsante->edges == NULL) {
        // TODO load edges from file
        char * filename = calloc(101,sizeof(char));
        snprintf(filename,100,"./resources/GUIelements/%s",pulsante->filename);
        FILE * drawing = fopen(filename,"r");
        fgets(filename,100,drawing);
        sscanf(filename,"%d",&(pulsante->numEdges));
        pulsante->edges = (int **) calloc (pulsante->numEdges,sizeof(int*));
        for( int i=0; i<pulsante->numEdges; i++) {
            // allocate
            pulsante->edges[i] = (int *) calloc (4,sizeof(int));
            // read values
            fgets(filename,100,drawing);
            sscanf(filename,"%d %d --- %d %d",&(pulsante->edges[i][0]),&(pulsante->edges[i][1]),&(pulsante->edges[i][2]),&(pulsante->edges[i][3]));
        }
    }

    for (int i=0; i<pulsante->numEdges; i++) {
        DrawLineEx((Vector2){pulsante->edges[i][0]+pulsante->x,pulsante->edges[i][1]+pulsante->y},(Vector2){pulsante->edges[i][2]+pulsante->x,pulsante->edges[i][3]+pulsante->y},hovering?5:1,hovering?YELLOW:GRAY);
    }
}

Vector2 getPos(node_t *nodes, char *name)
{
    int i = 0;
    while (strcmp(nodes[i].name, name))
        i++;
    return (Vector2){nodes[i].x, nodes[i].y};
}

int min(int a, int b)
{
    return (a < b ? a : b);
}
int max(int a, int b)
{
    return (a > b ? a : b);
}

void DrawLink(link_t link, settings_t* settings,node_t* nodes){
    Vector2 positions[2] = {getPos(nodes,link.node1),getPos(nodes,link.node2)};
    if (settings->moving_node){
        if (!strcmp(link.node1,settings->node_name)) positions[0]=(Vector2){GetMouseX(),GetMouseY()};
        else if (!strcmp(link.node2,settings->node_name)) positions[1]=(Vector2){GetMouseX(),GetMouseY()};
    }
    // breve digressione matematica: per verificare se sono su una linea verifico che la distanza del punto dalla linea sia minore di un qualcosa, diciamo 5 pixel.
    // ora, questo significa che posso rappresentare la mia linea con la seguente equazione:
    // y = mx + q, ossia y - mx - q = 0
    // oppure come y - y_0 = m (x - x_0)
    // ricavo m come (y - y_0) / (x - x_0)
    float m = (float)(positions[0].y - positions[1].y) / (float)(positions[0].x - positions[1].x);
    // a questo punto so che la mia retta è rappresentata da y = m (x - x_0) + y_0
    // quindi q = y_0 - m x_0
    float q = positions[1].y - m * positions[1].x;
    // quindi la distanza punto-retta si calcola come \mod(y_mouse - (m x_mouse + y_0))/\sqrt(1+m^2)
    float d = (float)abs(GetMouseY() - (m * GetMouseX() + q)) / (sqrt(1 + m * m));
    bool cond = ((d <= 5) &&
                 GetMouseX() >= min(positions[0].x, positions[1].x) &&
                 GetMouseX() <= max(positions[0].x, positions[1].x) &&
                 GetMouseY() >= min(positions[0].y, positions[1].y) &&
                 GetMouseY() <= max(positions[0].y, positions[1].y));
    DrawLineEx(positions[0], positions[1], /*thick*/ cond ? 5 : 1, /*Color color*/ RED);

    if (d && IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
    {
        settings->moving_node = false;
        settings->placing_node = false;
        settings->drawing_rectangle = false;
        printf("HERE I should open the configs\n");
    }
    /* debug
    char toPrint[15];
    snprintf(toPrint,10,"%f",d);
    DrawText(toPrint, 420, 420, 42, BLUE);
    */
}

void DrawFakeLink(settings_t*settings,node_t*nodes){
    DrawLineEx(getPos(nodes,settings->first_place),(Vector2){GetMouseX(),GetMouseY()},5,PURPLE);
}

void DrawNode(node_t* node, settings_t* settings, bool true_node){
    int nodex, nodey;
    if (!strcmp(node->name,settings->node_name)) {
        nodex = GetMouseX();
        nodey = GetMouseY();
    }
    else {
        nodex = node->x;
        nodey = node->y;
    }
    switch(node->type){
        case switch_t:
            DrawRectangle(nodex-20, nodey-20, 40, 40, BACKGROUND_COLOR); // this should fix the compenetrations, but it doesn't. :-(
            // external lines
            DrawLineEx((Vector2){nodex-18,nodey-15},(Vector2){nodex+18,nodey-15},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-18,nodey+15},(Vector2){nodex+18,nodey+15},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-18,nodey-15},(Vector2){nodex-18,nodey+15},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+18,nodey-15},(Vector2){nodex+18,nodey+15},2,FIGURE_COLOR);
            // frecce
            DrawLineEx((Vector2){nodex+1,nodey-9},(Vector2){nodex+15,nodey-9},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+10,nodey-13},(Vector2){nodex+15,nodey-9},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+10,nodey-5},(Vector2){nodex+15,nodey-9},2,FIGURE_COLOR);
            
            DrawLineEx((Vector2){nodex-15,nodey-3},(Vector2){nodex-1,nodey-3},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-15,nodey-3},(Vector2){nodex-10,nodey-7},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-15,nodey-3},(Vector2){nodex-10,nodey+1},2,FIGURE_COLOR);
            
            DrawLineEx((Vector2){nodex+1,nodey+3},(Vector2){nodex+15,nodey+3},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+10,nodey+7},(Vector2){nodex+15,nodey+3},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+10,nodey-1},(Vector2){nodex+15,nodey+3},2,FIGURE_COLOR);
            
            DrawLineEx((Vector2){nodex-15,nodey+9},(Vector2){nodex-1,nodey+9},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-15,nodey+9},(Vector2){nodex-10,nodey+13},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-15,nodey+9},(Vector2){nodex-10,nodey+5},2,FIGURE_COLOR);
            
            break;
        case hub_t:
            // external lines
            DrawLineEx((Vector2){nodex-18,nodey-12},(Vector2){nodex+18,nodey-12},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-18,nodey+12},(Vector2){nodex+18,nodey+12},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-18,nodey-12},(Vector2){nodex-18,nodey+12},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+18,nodey-12},(Vector2){nodex+18,nodey+12},2,FIGURE_COLOR);
            // frecce
            DrawLineEx((Vector2){nodex-15,nodey},(Vector2){nodex+15,nodey},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-15,nodey},(Vector2){nodex-10,nodey+4},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-15,nodey},(Vector2){nodex-10,nodey-4},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+15,nodey},(Vector2){nodex+10,nodey+4},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+15,nodey},(Vector2){nodex+10,nodey-4},2,FIGURE_COLOR);

            break;
        case external_interface_t:
            DrawLineEx((Vector2){nodex-3,nodey+18},(Vector2){nodex-3,nodey+13},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+3,nodey+18},(Vector2){nodex+3,nodey+13},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-3,nodey+18},(Vector2){nodex+3,nodey+18},2,FIGURE_COLOR);
            
            DrawLineEx((Vector2){nodex-3,nodey+13},(Vector2){nodex-7,nodey+10},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+3,nodey+13},(Vector2){nodex+7,nodey+10},2,FIGURE_COLOR);

            DrawLineEx((Vector2){nodex-7,nodey+10},(Vector2){nodex-10,nodey+9},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+7,nodey+10},(Vector2){nodex+10,nodey+9},2,FIGURE_COLOR);

            DrawLineEx((Vector2){nodex-10,nodey+9},(Vector2){nodex-10,nodey-10},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+10,nodey+9},(Vector2){nodex+10,nodey-10},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-10,nodey-10},(Vector2){nodex+10,nodey-10},1,FIGURE_COLOR);

            DrawLineEx((Vector2){nodex-7,nodey-10},(Vector2){nodex-7,nodey-15},1,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+7,nodey-10},(Vector2){nodex+7,nodey-15},1,FIGURE_COLOR);

            DrawLineEx((Vector2){nodex-7,nodey-15},(Vector2){nodex-5,nodey-18},1,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+7,nodey-15},(Vector2){nodex+5,nodey-18},1,FIGURE_COLOR);
            
            DrawLineEx((Vector2){nodex+5,nodey-18},(Vector2){nodex-5,nodey-18},1,FIGURE_COLOR);
            
            DrawLineEx((Vector2){nodex-3,nodey-10},(Vector2){nodex-3,nodey-15},1,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-1,nodey-10},(Vector2){nodex-1,nodey-15},1,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+1,nodey-10},(Vector2){nodex+1,nodey-15},1,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+3,nodey-10},(Vector2){nodex+3,nodey-15},1,FIGURE_COLOR);
            // draw a boh, something
            break;
        case external_natted_interface_t:
            DrawLineEx((Vector2){nodex-3,nodey+18},(Vector2){nodex-3,nodey+13},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+3,nodey+18},(Vector2){nodex+3,nodey+13},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-3,nodey+18},(Vector2){nodex+3,nodey+18},2,FIGURE_COLOR);
            
            DrawLineEx((Vector2){nodex-3,nodey+13},(Vector2){nodex-7,nodey+10},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+3,nodey+13},(Vector2){nodex+7,nodey+10},2,FIGURE_COLOR);

            DrawLineEx((Vector2){nodex-7,nodey+10},(Vector2){nodex-10,nodey+9},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+7,nodey+10},(Vector2){nodex+10,nodey+9},2,FIGURE_COLOR);

            DrawLineEx((Vector2){nodex-10,nodey+9},(Vector2){nodex-10,nodey-10},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+10,nodey+9},(Vector2){nodex+10,nodey-10},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-10,nodey-10},(Vector2){nodex+10,nodey-10},1,FIGURE_COLOR);

            DrawLineEx((Vector2){nodex-7,nodey-10},(Vector2){nodex-7,nodey-15},1,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+7,nodey-10},(Vector2){nodex+7,nodey-15},1,FIGURE_COLOR);

            DrawLineEx((Vector2){nodex-7,nodey-15},(Vector2){nodex-5,nodey-18},1,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+7,nodey-15},(Vector2){nodex+5,nodey-18},1,FIGURE_COLOR);
            
            DrawLineEx((Vector2){nodex+5,nodey-18},(Vector2){nodex-5,nodey-18},1,FIGURE_COLOR);
            
            DrawLineEx((Vector2){nodex-3,nodey-10},(Vector2){nodex-3,nodey-15},1,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-1,nodey-10},(Vector2){nodex-1,nodey-15},1,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+1,nodey-10},(Vector2){nodex+1,nodey-15},1,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+3,nodey-10},(Vector2){nodex+3,nodey-15},1,FIGURE_COLOR);

            //DrawText("NAT",nodex-9,nodey-3,0,FIGURE_COLOR);

            DrawLineEx((Vector2){nodex-7,nodey-3},(Vector2){nodex-7,nodey+2},1,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-7,nodey-3},(Vector2){nodex-3,nodey+2},1,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-3,nodey-3},(Vector2){nodex-3,nodey+2},1,FIGURE_COLOR);

            DrawLineEx((Vector2){nodex-1,nodey-3},(Vector2){nodex-1,nodey+2},1,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+3,nodey-3},(Vector2){nodex+3,nodey+2},1,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-1,nodey-1},(Vector2){nodex+3,nodey-1},1,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-1,nodey-3},(Vector2){nodex+3,nodey-3},1,FIGURE_COLOR);

            DrawLineEx((Vector2){nodex+4,nodey-3},(Vector2){nodex+8,nodey-3},1,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+6,nodey-3},(Vector2){nodex+6,nodey+2},1,FIGURE_COLOR);
            // draw a boh, something
            break;
        case router_t:
            // draw a circle
            DrawRectangle(nodex-20, nodey-20, 40, 40, BACKGROUND_COLOR); // this should fix the compenetrations, but it doesn't. :-(
            DrawRing((Vector2){nodex,nodey}, 16, 18, 0, 360, 0, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-3,nodey},(Vector2){nodex-14,nodey},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-3,nodey},(Vector2){nodex-8,nodey+4},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-3,nodey},(Vector2){nodex-8,nodey-4},2,FIGURE_COLOR);
            
            DrawLineEx((Vector2){nodex+3,nodey},(Vector2){nodex+14,nodey},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+3,nodey},(Vector2){nodex+8,nodey+4},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+3,nodey},(Vector2){nodex+8,nodey-4},2,FIGURE_COLOR);
            
            DrawLineEx((Vector2){nodex,nodey-3},(Vector2){nodex,nodey-14},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+4,nodey-10},(Vector2){nodex,nodey-14},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-4,nodey-10},(Vector2){nodex,nodey-14},2,FIGURE_COLOR);
            
            DrawLineEx((Vector2){nodex,nodey+3},(Vector2){nodex,nodey+14},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+4,nodey+10},(Vector2){nodex,nodey+14},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-4,nodey+10},(Vector2){nodex,nodey+14},2,FIGURE_COLOR);
            break;
        case host_t:
            DrawRectangle(nodex-20, nodey-20, 40, 40, BACKGROUND_COLOR); // this should fix the compenetrations, but it doesn't. :-(
            DrawLineEx((Vector2){nodex-18,nodey-18},(Vector2){nodex+18,nodey-18},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-18,nodey-18},(Vector2){nodex-18,nodey+5},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+18,nodey-18},(Vector2){nodex+18,nodey+5},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-18,nodey+5},(Vector2){nodex+18,nodey+5},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-3,nodey+5},(Vector2){nodex-3,nodey+15},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+3,nodey+5},(Vector2){nodex+3,nodey+15},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-15,nodey+15},(Vector2){nodex+15,nodey+15},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-15,nodey+18},(Vector2){nodex+15,nodey+18},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex-15,nodey+15},(Vector2){nodex-15,nodey+18},2,FIGURE_COLOR);
            DrawLineEx((Vector2){nodex+15,nodey+15},(Vector2){nodex+15,nodey+18},2,FIGURE_COLOR);
            // draw a something else
            // temporaneamente commentato in attesa di capire come si usi
            /*Texture2D router = LoadTexture("./resources/icons/elfuny.png");
            Rectangle source_rec = { 0.0f, 0.0f, router.width, router.height};  // takes all the image 
            Rectangle dest_rec = {nodex-20, nodey-20, 40,40};   // where the image should be placed and how big it will be
            Vector2 origin = {0.0f,0.0f};   // the origin is the top right corner
            DrawTexturePro(router, source_rec, dest_rec, origin, 0.0f, WHITE);  // draw a router and do not rotate it (0.0f)
            */
            break;
    }
    DrawText(node->name,nodex-20,nodey+40,STD_FONT_SIZE,GRAY);
    // se non sto al momento spostando niente e ci clicco con il tasto sinistro allora inizio a draggarlo
    if (
        true_node &&
        GetMouseX() <= node->x+20 &&
        GetMouseX() >= node->x-20 &&
        GetMouseY() <= node->y+20 &&
        GetMouseY() >= node->y-20)
    {
        if (
            IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
            ! settings->dragging_deactivated
            )
        {
            if(!settings->moving_node && !settings->placing_node){
                if (!strcmp(settings->node_name,node->name)){
                    settings->node_name = (char*) calloc(NAMELENGTH,sizeof(char));
                }
                else {
                    if (settings->placing_link == 3) {
                        settings->placing_link = 0;
                    }
                    else {
                        settings->placing_node = false;
                        settings->drawing_rectangle = false;
                        settings->moving_node = 1;
                        settings->node_type = node->type;
                        settings->node_name = (char*) calloc(NAMELENGTH,sizeof(char));
                        snprintf(settings->node_name,NAMELENGTH,"%s",node->name);
                    }
                }
            }
        }
        else if (
            IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)
            )
        {
            if(settings->isSimulating) {
                // TODO open shell on node NOT TESTED
                settings->placing_node = false;
                settings->placing_link = false;
                settings->drawing_rectangle = false;
                settings->moving_node = false;
                settings->node_type = node->type;
                settings->node_name = (char*) calloc(NAMELENGTH,sizeof(char));
                snprintf(settings->node_name,NAMELENGTH,"%s",node->name);
            }
            else {
                // TODO open node configs
            }
        }
    }
}

void setNode(char * name, node_t* nodes, settings_t * settings){
    for (int i=0; i<settings->numnodes; i++) {
        if (!strcmp(name,nodes[i].name)){
            nodes[i].x = GetMouseX();
            nodes[i].y = GetMouseY();
        }
    }
}

char * identify(int num){
    return (char[50][50]){"hu","s","r","h","e","en"}[num]; // hub, switch, router, host, external interface, external natted interface
}

void appendNode(interface_t*interface,node_t newnode,settings_t*settings) {
    if (settings->numnodes == 0){
        interface->nodes = (node_t*)calloc(1,sizeof(node_t));
    }
    else interface->nodes = (node_t*)realloc(interface->nodes,(settings->numnodes+1)*sizeof(node_t));
    interface->nodes[settings->numnodes].name = (char*) calloc(NAMELENGTH,sizeof(char));
    strncpy(interface->nodes[settings->numnodes].name,newnode.name,49);
    interface->nodes[settings->numnodes].type = newnode.type;
    interface->nodes[settings->numnodes].x = newnode.x;
    interface->nodes[settings->numnodes].y = newnode.y;
}

void appendLink(interface_t*interface,settings_t*settings,link_t link) {
    if (settings->numlink == 0){
        interface->links = (link_t*)calloc(1,sizeof(link_t));
    }
    else interface->links = (link_t*)realloc(interface->links,(settings->numlink+1)*sizeof(link_t)); // reallocating if not first link
    interface->links[settings->numlink].node1 = (char*) calloc(NAMELENGTH,sizeof(char));    // allocating memory for interface names
    interface->links[settings->numlink].node2 = (char*) calloc(NAMELENGTH,sizeof(char));
    strncpy(interface->links[settings->numlink].node1,link.node1,49);   // setting interface names
    strncpy(interface->links[settings->numlink].node2,link.node2,49);
    interface->links[settings->numlink].node1_type = link.node1_type;
    interface->links[settings->numlink].node2_type = link.node2_type;
    //printf("allocated %s that is a %d and %s that is a %d\n", interface->links[settings->numlink].node1, interface->links[settings->numlink].node1_type, interface->links[settings->numlink].node2, interface->links[settings->numlink].node2_type);
}

node_t * getInversePos(int x, int y, node_t * nodes, settings_t * settings) { // now returns entire struct node (node type is needed to handle different types of links in logical controller)
    for (int i=0; i<settings->numnodes; i++) {
        if (
            x <= nodes[i].x + 20 &&
            x >= nodes[i].x - 20 &&
            y <= nodes[i].y + 20 &&
            y >= nodes[i].y -20
        ) {
            return &nodes[i];
        }
    }
    return NULL;
}

bool isSomethingUnder(int x,int y,node_t * nodes,settings_t*settings){
    for (int i=0; i<settings->numnodes; i++) {
        if (
            x <= nodes[i].x + 20 &&
            x >= nodes[i].x - 20 &&
            y <= nodes[i].y + 20 &&
            y >= nodes[i].y -20
        ) {
            return true;
        }
    }
    return false;

}

void DrawMessageAtAngle(char*message) {
    DrawText(message,1900-STD_FONT_SIZE/2*strlen(message),970,STD_FONT_SIZE,GRAY);
}

void DrawGUI(settings_t* settings, interface_t * interface) {

    // 1. piazzo i buttons
    for (int i=0;i<NUMbuttons;i++) DrawButton(&(interface->buttons[i]),settings);
    
    // 2. se premo esc disattivo dragging e placing TODO bugged
    if(IsKeyReleased(KEY_ESCAPE)) {
        settings->moving_node = false;
        settings->placing_node = false;
        settings->drawing_rectangle = false;
        settings->placing_link = false;
        settings->node_type = -1;
    }

    // 3. se sto spostando cose
    if (settings->moving_node) {
        // giusto per sicurezza, disattivo il placing se sto piazzando cose
        settings->placing_node = false;
        // disegno un nodo "fantasma" dove sto muovendo il mouse
        DrawNode(&(node_t){settings->node_name,settings->node_type,GetMouseX(),GetMouseY()},settings,false);
        // se sto premendo (rilasciando, tbh) il tasto sinistro del mouse
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            // smetto di spostare il nodo
            settings->moving_node = false;
            // ne imposto le posizioni
            setNode(settings->node_name,interface->nodes,settings);
        }
        else DrawMessageAtAngle("Select a new position for the node");
    }

    // 4. altrimenti, se sto posizionando qualcosa di nuovo
    else if (settings->placing_node) {
        // creo il name del nuovo nodo
        char name[50];
        snprintf(name,50,"%s-%d",identify(settings->node_type), settings->numnodes);
        // disegno un nodo "fantasma" dove sto muovendo il mouse
        DrawNode(&(node_t){name,settings->node_type,GetMouseX(),GetMouseY()},settings,false);
        // se premo il mouse
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            // allora non lo sto più piazzando
            settings->placing_node = false;
            // aggiungo il nodo alla lista
            appendNode(interface,(node_t){name,settings->node_type,GetMouseX(),GetMouseY()},settings);
            // e aumento il numero di nodes
            settings->numnodes++;
        }
        else DrawMessageAtAngle("Choose the position for this new node");
    }
 
    // 5. altrimenti, se sto posizionando un link
    else if (settings->placing_link) {
        // se ho già selezionato il primo nodo
        if (settings->placing_link==1 && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && isSomethingUnder(GetMouseX(),GetMouseY(),interface->nodes,settings)) {
            //printf("%s\n",getInversePos(GetMouseX(),GetMouseY(),interface->nodes,settings));
            strncpy(settings->first_place,getInversePos(GetMouseX(),GetMouseY(),interface->nodes,settings)->name,50);    // adding name of first selected node to settings 
            settings->first_place_nodetype = (void *)getInversePos(GetMouseX(),GetMouseY(),interface->nodes,settings)->type;     // adding type of first selected node to settings
            settings->dragging_deactivated = true;
            settings->placing_link = 2;
        }
        else if (settings->placing_link == 1) {
            DrawCircle(GetMouseX(), GetMouseY(), 5, PURPLE);
            DrawMessageAtAngle("Select the first node to link");
        }
        // altrimenti
        else if(settings->placing_link == 2 && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)&&isSomethingUnder(GetMouseX(),GetMouseY(),interface->nodes,settings)){
            settings->placing_link = 3;
            // funny modified function that now returns link_t with the extra needed info too
            appendLink(interface,settings,(link_t){settings->first_place,getInversePos(GetMouseX(),GetMouseY(),interface->nodes,settings)->name,(component_type_t)settings->first_place_nodetype,getInversePos(GetMouseX(),GetMouseY(),interface->nodes,settings)->type});
            //aumento il numero di link
            settings->numlink++;
            settings->dragging_deactivated = false;
        }
        else if(settings->placing_link == 2) {
            DrawFakeLink(settings,interface->nodes);
            DrawMessageAtAngle("Select the second node to link");
        }
    }

    else if (settings->drawing_rectangle) {
        // TODO draw rectangle with all the cases and prompting for the color
        if (settings->drawing_rectangle == 1) {
            settings->dragging_deactivated = true;
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                settings->posX = GetMouseX();
                settings->posY = GetMouseY();
                settings->drawing_rectangle = 2;
            }
            else {
                DrawRectangle(GetMouseX(),GetMouseY(),5,5,PURPLE);
                DrawMessageAtAngle("Select the first angle");
            }
        }
        else if (settings->drawing_rectangle == 2) {
            settings->dragging_deactivated = true;
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                settings->dragging_deactivated = false;
                // TODO add rectangle to interface
                settings->drawing_rectangle = 0;
            }
            else {
                DrawMessageAtAngle("Select the second angle");
                DrawRectangleLines(min(settings->posX,GetMouseX()),min(settings->posY,GetMouseY()),abs(GetMouseX()-settings->posX),abs(GetMouseY()-settings->posY),PURPLE);
                // TODO draw partial rectangle
            }
        }
        else {
            settings->drawing_rectangle = 1;
        }
    }

    // ora posiziono tutto: nodes e link
    for (int i=0; i<settings->numlink;i++) DrawLink(interface->links[i],settings,interface->nodes);
    for (int i=0; i<settings->numnodes;i++) DrawNode(&(interface->nodes[i]),settings,true);

}
