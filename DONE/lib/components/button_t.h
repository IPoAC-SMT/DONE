#include "../settings.h"
typedef struct
{
    int x;                         // x to start drawing
    int y;                         // y to start drawing
    int width;                     // width of the rectangle
    int height;                    // height of the rectangle
    int iconType;                  // type of the icon
    void (*pressed)(settings_t *); // function to be called when pressed
    char * alt_text;               // text to display when hovering
    int numEdges;
    int ** edges;
    char * filename;
} button_t;
