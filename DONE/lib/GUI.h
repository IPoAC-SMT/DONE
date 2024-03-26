#include <raylib.h>
#include <string.h>
#include <math.h>

#include "./interface_t.h"

#include "functions.h"

#define NUMPULSANTI 16
#define NUMNODI 6
#define NUMLINK 5
#define NAMELENGTH 20
#define STD_FONT_SIZE 19

#define BACKGROUND_COLOR CLITERAL(Color){19, 31, 42,0} 
#define FIGURE_COLOR GRAY

void DrawGUI(settings_t *settings, interface_t *interface);
