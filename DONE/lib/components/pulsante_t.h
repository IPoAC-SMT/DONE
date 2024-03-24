#include "../settings.h"
typedef struct {
    int x;                          // x to start drawing
    int y;                          // y to start drawing
    int width;                      // width of the rectangle
    int height;                     // height of the rectangle
    char *iconName;                 // name of the icon
    void (*pressed)(settings_t*);   // function to be called when pressed
} pulsante_t;
