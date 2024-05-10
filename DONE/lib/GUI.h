#include <raylib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#include "./interface_t.h"

#include "functions.h"

#define NUMbuttons 18
#define NUMLINK 0
#define NAMELENGTH 50
#define STD_FONT_SIZE 19
#define WIDTH 1000
#define HEIGHT 800

#define BACKGROUND_COLOR \
    CLITERAL(Color) { 19, 31, 42, 0 }
#define FIGURE_COLOR WHITE

void DrawGUI(settings_t *settings, interface_t *interface);
