#include "../lib/GUI.h"

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

void DrawGUI(settings_t* settings, interface_t * interface){
    for (int i=0; i<NUMPULSANTI; i++) DrawButton(interface->pulsanti[i],settings);

}
