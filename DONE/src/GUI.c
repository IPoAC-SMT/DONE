#include "../lib/GUI.h"

//const diocane = ;

void DrawButton(pulsante_t pulsante,settings_t*settings){
    bool hovering;
    if(
        (GetMouseX()>=pulsante.x && GetMouseX()<=(pulsante.x+pulsante.width))
        &&
        (GetMouseY()>=pulsante.y && GetMouseY()<=(pulsante.y+pulsante.height))
    )
    {
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            hovering = false;
            pulsante.pressed(settings);
        }
        else hovering = true;
    }
    else hovering = false;

    DrawRectangleLines(pulsante.x,pulsante.y,pulsante.width,pulsante.height,hovering?YELLOW:BLACK); // lo creo con colore diverso se ci sto o meno hoverando
}

Vector2 getPos(node_t* nodi, char* nome){
    int i=0;
    while (strcmp(nodi[i].nome,nome)) i++;
    return (Vector2){nodi[i].x,nodi[i].y};
}

int min(int a,int b){
    return (a<b?a:b);
}
int max(int a,int b){
    return (a>b?a:b);
}

void DrawLink(link_t link, settings_t* settings,node_t* nodi){
    Vector2 positions[2] = {getPos(nodi,link.nodo1),getPos(nodi,link.nodo2)};
    // breve digressione matematica: per verificare se sono su una linea verifico che la distanza del punto dalla linea sia minore di un qualcosa, diciamo 5 pixel.
    // ora, questo significa che posso rappresentare la mia linea con la seguente equazione:
    // y = mx + q, ossia y - mx - q = 0
    // oppure come y - y_0 = m (x - x_0)
    // ricavo m come (y - y_0) / (x - x_0)
    float m = (float)(positions[0].y - positions[1].y) / (float)(positions[0].x - positions[1].x);
    // a questo punto so che la mia retta è rappresentata da y = m (x - x_0) + y_0
    // quindi q = y_0 - m x_0
    float q = positions[1].y-m*positions[1].x;
    // quindi la distanza punto-retta si calcola come \mod(y_mouse - (m x_mouse + y_0))/\sqrt(1+m^2)
    float d = (float)abs(GetMouseY() - (m*GetMouseX()+q))/(sqrt(1+m*m));
    bool cond = (
            (d<=5) &&
            GetMouseX() >= min(positions[0].x,positions[1].x) &&
            GetMouseX() <= max(positions[0].x,positions[1].x) &&
            GetMouseY() >= min(positions[0].y,positions[1].y) &&
            GetMouseY() <= max(positions[0].y,positions[1].y)
            );
    DrawLineEx(positions[0],positions[1], /*thick*/ cond?5:1 , /*Color color*/ RED);
    
    if (d && IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)){
        printf("HERE I should open the configs\n");
    }
    /* debug
    char toPrint[15];
    snprintf(toPrint,10,"%f",d);
    DrawText(toPrint, 420, 420, 42, BLUE);
    */
}

void DrawNode(node_t* node, settings_t* settings){
    switch(node->tipo){
        case switch_t:
            // draw a rectangle
            break;
        case hub_t:
            break;
        case external_interface_t:
            // draw a boh, something
            break;
        case router_t:
            // draw a circle
            break;
        case host_t:
            //DrawRectangle(node->x-20, node->y-20, 40, 40, PURPLE);   
            // draw a something else
            Texture2D router = LoadTexture("../resources/icons/router.png");
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTexture(router, 40, 40, WHITE);
            EndDrawing();
            break;
    }
    // TODO draw text with name
    DrawText(node->nome,node->x-20,node->y+40,STD_FONT_SIZE,BLUE);
    // se non sto al momento spostando niente e ci clicco con il tasto sinistro allora inizio a draggarlo
    

}

void DrawGUI(settings_t* settings, interface_t * interface){
    for (int i=0; i<NUMPULSANTI; i++) DrawButton(interface->pulsanti[i],settings);
    for (int i=0; i<NUMLINK;i++) DrawLink(interface->links[i],settings,interface->nodi);
    for (int i=0; i<NUMNODI;i++) DrawNode(&(interface->nodi[i]),settings);

    // se dragging allora ci piazzo cose

}
