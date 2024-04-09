#include "../settings.h"
typedef struct
{
    int x;                         // x to start drawing
    int y;                         // y to start drawing
    int width;                     // width of the rectangle
    int height;                    // height of the rectangle
    void (*pressed)(settings_t *); // function to be called when pressed
    char * alt_text;               // text to display when hovering
    int numEdges;
    int ** edges;
    char * filename;
    char always_show;
} button_t;
