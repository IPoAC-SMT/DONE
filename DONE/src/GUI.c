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
            settings->deletingNodes = 0;
            hovering = false;
            pulsante->pressed(settings);
        }
        else
        {
            hovering = true;
            DrawText(pulsante->alt_text, GetMouseX() + 20, GetMouseY() + 20, STD_FONT_SIZE, GRAY);
        }
    }
    else
        hovering = false;

    DrawRectangleLines(pulsante->x, pulsante->y, pulsante->width, pulsante->height, hovering ? YELLOW : GRAY); // lo creo con colore diverso se ci sto o meno hoverando

    if (pulsante->edges == NULL)
    {
        // load edges from file
        char *filename = calloc(101, sizeof(char));
        snprintf(filename, 100, "./resources/GUIelements/%s", pulsante->filename);
        FILE *drawing = fopen(filename, "r");
        fgets(filename, 100, drawing);
        sscanf(filename, "%d", &(pulsante->numEdges));
        pulsante->edges = (int **)calloc(pulsante->numEdges, sizeof(int *));
        for (int i = 0; i < pulsante->numEdges; i++)
        {
            // allocate
            pulsante->edges[i] = (int *)calloc(4, sizeof(int));
            // read values
            fgets(filename, 100, drawing);
            sscanf(filename, "%d %d --- %d %d", &(pulsante->edges[i][0]), &(pulsante->edges[i][1]), &(pulsante->edges[i][2]), &(pulsante->edges[i][3]));
        }
    }

    for (int i = 0; i < pulsante->numEdges; i++)
    {
        DrawLineEx((Vector2){pulsante->edges[i][0] + pulsante->x, pulsante->edges[i][1] + pulsante->y}, (Vector2){pulsante->edges[i][2] + pulsante->x, pulsante->edges[i][3] + pulsante->y}, hovering ? 5 : 1, hovering ? YELLOW : GRAY);
    }
}

Vector2 getPos(node_t *nodes, char *name, settings_t *settings)
{
    int i = 0;
    while (i < settings->numnodes)
    {
        if (!strcmp(nodes[i].name, name))
            return (Vector2){nodes[i].x, nodes[i].y};
        else
            i++;
    }
    return (Vector2){-1, -1}; // if the node is not found
}

int min(int a, int b)
{
    return (a < b ? a : b);
}
int max(int a, int b)
{
    return (a > b ? a : b);
}

bool isNearLink(Vector2 *positions)
{
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
    float d = (float)abs(GetMouseY() - (int)(m * GetMouseX() + q)) / (sqrt(1 + m * m));
    return ((d <= 5) &&
            GetMouseX() >= min(positions[0].x, positions[1].x) &&
            GetMouseX() <= max(positions[0].x, positions[1].x) &&
            GetMouseY() >= min(positions[0].y, positions[1].y) &&
            GetMouseY() <= max(positions[0].y, positions[1].y));
}

void DrawLink(link_t link, settings_t *settings, node_t *nodes)
{
    Vector2 positions[2] = {getPos(nodes, link.node1, settings), getPos(nodes, link.node2, settings)};

    if (positions[0].x == -1 || positions[0].y == -1 || positions[1].x == -1 || positions[1].y == -1) // maybe yes, maybe no
        return;

    if (settings->moving_node)
    {
        if (!strcmp(link.node1, settings->node_name))
            positions[0] = (Vector2){GetMouseX(), GetMouseY()};
        else if (!strcmp(link.node2, settings->node_name))
            positions[1] = (Vector2){GetMouseX(), GetMouseY()};
    }

    DrawLineEx(positions[0], positions[1], /*thick*/ isNearLink(positions) ? 5 : 1, /*Color color*/ RED);

    /*if (d && settings->deletingNodes && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) TODO
    {
        settings->moving_node = false;
        settings->placing_node = false;
        settings->drawing_rectangle = false;
    }*/
}

void DrawFakeLink(settings_t *settings, node_t *nodes)
{
    DrawLineEx(getPos(nodes, settings->first_place, settings), (Vector2){GetMouseX(), GetMouseY()}, 5, PURPLE);
}

void DrawNode(node_t *node, settings_t *settings, bool true_node)
{
    int nodex, nodey;
    if (settings->moving_node && !strcmp(node->name, settings->node_name))
    {
        nodex = GetMouseX();
        nodey = GetMouseY();
    }
    else
    {
        nodex = node->x;
        nodey = node->y;
    }
    switch (node->type)
    {
    case switch_t:
        // DrawRectangle(nodex-20, nodey-20, 40, 40, BACKGROUND_COLOR); // this should fix the compenetrations, but it doesn't. :-(
        //  external lines
        DrawLineEx((Vector2){nodex - 18, nodey - 15}, (Vector2){nodex + 18, nodey - 15}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 18, nodey + 15}, (Vector2){nodex + 18, nodey + 15}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 18, nodey - 15}, (Vector2){nodex - 18, nodey + 15}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 18, nodey - 15}, (Vector2){nodex + 18, nodey + 15}, 2, FIGURE_COLOR);
        // frecce
        DrawLineEx((Vector2){nodex + 1, nodey - 9}, (Vector2){nodex + 15, nodey - 9}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 10, nodey - 13}, (Vector2){nodex + 15, nodey - 9}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 10, nodey - 5}, (Vector2){nodex + 15, nodey - 9}, 2, FIGURE_COLOR);

        DrawLineEx((Vector2){nodex - 15, nodey - 3}, (Vector2){nodex - 1, nodey - 3}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 15, nodey - 3}, (Vector2){nodex - 10, nodey - 7}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 15, nodey - 3}, (Vector2){nodex - 10, nodey + 1}, 2, FIGURE_COLOR);

        DrawLineEx((Vector2){nodex + 1, nodey + 3}, (Vector2){nodex + 15, nodey + 3}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 10, nodey + 7}, (Vector2){nodex + 15, nodey + 3}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 10, nodey - 1}, (Vector2){nodex + 15, nodey + 3}, 2, FIGURE_COLOR);

        DrawLineEx((Vector2){nodex - 15, nodey + 9}, (Vector2){nodex - 1, nodey + 9}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 15, nodey + 9}, (Vector2){nodex - 10, nodey + 13}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 15, nodey + 9}, (Vector2){nodex - 10, nodey + 5}, 2, FIGURE_COLOR);

        break;
    case hub_t:
        // external lines
        DrawLineEx((Vector2){nodex - 18, nodey - 12}, (Vector2){nodex + 18, nodey - 12}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 18, nodey + 12}, (Vector2){nodex + 18, nodey + 12}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 18, nodey - 12}, (Vector2){nodex - 18, nodey + 12}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 18, nodey - 12}, (Vector2){nodex + 18, nodey + 12}, 2, FIGURE_COLOR);
        // frecce
        DrawLineEx((Vector2){nodex - 15, nodey}, (Vector2){nodex + 15, nodey}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 15, nodey}, (Vector2){nodex - 10, nodey + 4}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 15, nodey}, (Vector2){nodex - 10, nodey - 4}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 15, nodey}, (Vector2){nodex + 10, nodey + 4}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 15, nodey}, (Vector2){nodex + 10, nodey - 4}, 2, FIGURE_COLOR);

        break;
    case external_interface_t:
        DrawLineEx((Vector2){nodex - 3, nodey + 18}, (Vector2){nodex - 3, nodey + 13}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 3, nodey + 18}, (Vector2){nodex + 3, nodey + 13}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 3, nodey + 18}, (Vector2){nodex + 3, nodey + 18}, 2, FIGURE_COLOR);

        DrawLineEx((Vector2){nodex - 3, nodey + 13}, (Vector2){nodex - 7, nodey + 10}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 3, nodey + 13}, (Vector2){nodex + 7, nodey + 10}, 2, FIGURE_COLOR);

        DrawLineEx((Vector2){nodex - 7, nodey + 10}, (Vector2){nodex - 10, nodey + 9}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 7, nodey + 10}, (Vector2){nodex + 10, nodey + 9}, 2, FIGURE_COLOR);

        DrawLineEx((Vector2){nodex - 10, nodey + 9}, (Vector2){nodex - 10, nodey - 10}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 10, nodey + 9}, (Vector2){nodex + 10, nodey - 10}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 10, nodey - 10}, (Vector2){nodex + 10, nodey - 10}, 1, FIGURE_COLOR);

        DrawLineEx((Vector2){nodex - 7, nodey - 10}, (Vector2){nodex - 7, nodey - 15}, 1, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 7, nodey - 10}, (Vector2){nodex + 7, nodey - 15}, 1, FIGURE_COLOR);

        DrawLineEx((Vector2){nodex - 7, nodey - 15}, (Vector2){nodex - 5, nodey - 18}, 1, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 7, nodey - 15}, (Vector2){nodex + 5, nodey - 18}, 1, FIGURE_COLOR);

        DrawLineEx((Vector2){nodex + 5, nodey - 18}, (Vector2){nodex - 5, nodey - 18}, 1, FIGURE_COLOR);

        DrawLineEx((Vector2){nodex - 3, nodey - 10}, (Vector2){nodex - 3, nodey - 15}, 1, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 1, nodey - 10}, (Vector2){nodex - 1, nodey - 15}, 1, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 1, nodey - 10}, (Vector2){nodex + 1, nodey - 15}, 1, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 3, nodey - 10}, (Vector2){nodex + 3, nodey - 15}, 1, FIGURE_COLOR);
        // draw a boh, something
        break;
    case external_natted_interface_t:
        DrawLineEx((Vector2){nodex - 3, nodey + 18}, (Vector2){nodex - 3, nodey + 13}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 3, nodey + 18}, (Vector2){nodex + 3, nodey + 13}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 3, nodey + 18}, (Vector2){nodex + 3, nodey + 18}, 2, FIGURE_COLOR);

        DrawLineEx((Vector2){nodex - 3, nodey + 13}, (Vector2){nodex - 7, nodey + 10}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 3, nodey + 13}, (Vector2){nodex + 7, nodey + 10}, 2, FIGURE_COLOR);

        DrawLineEx((Vector2){nodex - 7, nodey + 10}, (Vector2){nodex - 10, nodey + 9}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 7, nodey + 10}, (Vector2){nodex + 10, nodey + 9}, 2, FIGURE_COLOR);

        DrawLineEx((Vector2){nodex - 10, nodey + 9}, (Vector2){nodex - 10, nodey - 10}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 10, nodey + 9}, (Vector2){nodex + 10, nodey - 10}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 10, nodey - 10}, (Vector2){nodex + 10, nodey - 10}, 1, FIGURE_COLOR);

        DrawLineEx((Vector2){nodex - 7, nodey - 10}, (Vector2){nodex - 7, nodey - 15}, 1, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 7, nodey - 10}, (Vector2){nodex + 7, nodey - 15}, 1, FIGURE_COLOR);

        DrawLineEx((Vector2){nodex - 7, nodey - 15}, (Vector2){nodex - 5, nodey - 18}, 1, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 7, nodey - 15}, (Vector2){nodex + 5, nodey - 18}, 1, FIGURE_COLOR);

        DrawLineEx((Vector2){nodex + 5, nodey - 18}, (Vector2){nodex - 5, nodey - 18}, 1, FIGURE_COLOR);

        DrawLineEx((Vector2){nodex - 3, nodey - 10}, (Vector2){nodex - 3, nodey - 15}, 1, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 1, nodey - 10}, (Vector2){nodex - 1, nodey - 15}, 1, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 1, nodey - 10}, (Vector2){nodex + 1, nodey - 15}, 1, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 3, nodey - 10}, (Vector2){nodex + 3, nodey - 15}, 1, FIGURE_COLOR);

        // DrawText("NAT",nodex-9,nodey-3,0,FIGURE_COLOR);

        DrawLineEx((Vector2){nodex - 7, nodey - 3}, (Vector2){nodex - 7, nodey + 2}, 1, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 7, nodey - 3}, (Vector2){nodex - 3, nodey + 2}, 1, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 3, nodey - 3}, (Vector2){nodex - 3, nodey + 2}, 1, FIGURE_COLOR);

        DrawLineEx((Vector2){nodex - 1, nodey - 3}, (Vector2){nodex - 1, nodey + 2}, 1, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 3, nodey - 3}, (Vector2){nodex + 3, nodey + 2}, 1, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 1, nodey - 1}, (Vector2){nodex + 3, nodey - 1}, 1, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 1, nodey - 3}, (Vector2){nodex + 3, nodey - 3}, 1, FIGURE_COLOR);

        DrawLineEx((Vector2){nodex + 4, nodey - 3}, (Vector2){nodex + 8, nodey - 3}, 1, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 6, nodey - 3}, (Vector2){nodex + 6, nodey + 2}, 1, FIGURE_COLOR);
        // draw a boh, something
        break;
    case router_t:
        if (!settings->ipoac)
        {
            // draw a circle
            // DrawRectangle(nodex-20, nodey-20, 40, 40, BACKGROUND_COLOR); // this should fix the compenetrations, but it doesn't. :-(
            DrawRing((Vector2){nodex, nodey}, 16, 18, 0, 360, 0, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex - 3, nodey}, (Vector2){nodex - 14, nodey}, 2, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex - 3, nodey}, (Vector2){nodex - 8, nodey + 4}, 2, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex - 3, nodey}, (Vector2){nodex - 8, nodey - 4}, 2, FIGURE_COLOR);

            DrawLineEx((Vector2){nodex + 3, nodey}, (Vector2){nodex + 14, nodey}, 2, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex + 3, nodey}, (Vector2){nodex + 8, nodey + 4}, 2, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex + 3, nodey}, (Vector2){nodex + 8, nodey - 4}, 2, FIGURE_COLOR);

            DrawLineEx((Vector2){nodex, nodey - 3}, (Vector2){nodex, nodey - 14}, 2, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex + 4, nodey - 10}, (Vector2){nodex, nodey - 14}, 2, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex - 4, nodey - 10}, (Vector2){nodex, nodey - 14}, 2, FIGURE_COLOR);

            DrawLineEx((Vector2){nodex, nodey + 3}, (Vector2){nodex, nodey + 14}, 2, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex + 4, nodey + 10}, (Vector2){nodex, nodey + 14}, 2, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex - 4, nodey + 10}, (Vector2){nodex, nodey + 14}, 2, FIGURE_COLOR);
        }
        else
        {
            DrawLineEx((Vector2){nodex - 16, nodey + 16}, (Vector2){nodex + 4, nodey + 12}, 2, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex + 4, nodey + 12}, (Vector2){nodex + 10, nodey + 2}, 2, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex + 10, nodey + 2}, (Vector2){nodex + 16, nodey + 4}, 2, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex + 16, nodey + 4}, (Vector2){nodex + 12, nodey + 2}, 2, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex + 12, nodey + 2}, (Vector2){nodex + 4, nodey + 2}, 2, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex + 4, nodey + 2}, (Vector2){nodex + 4, nodey - 10}, 2, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex + 4, nodey - 10}, (Vector2){nodex - 8, nodey - 18}, 2, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex - 8, nodey - 18}, (Vector2){nodex - 8, nodey - 10}, 2, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex - 8, nodey - 10}, (Vector2){nodex - 18, nodey - 12}, 2, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex - 18, nodey - 12}, (Vector2){nodex - 8, nodey + 6}, 2, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex - 8, nodey + 6}, (Vector2){nodex - 16, nodey + 16}, 2, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex - 8, nodey + 6}, (Vector2){nodex + 4, nodey + 2}, 2, FIGURE_COLOR);
            DrawLineEx((Vector2){nodex - 8, nodey + 6}, (Vector2){nodex - 8, nodey - 10}, 2, FIGURE_COLOR);
        }
        break;
    case host_t:
        // DrawRectangle(nodex-20, nodey-20, 40, 40, BACKGROUND_COLOR); // this should fix the compenetrations, but it doesn't. :-(
        DrawLineEx((Vector2){nodex - 18, nodey - 18}, (Vector2){nodex + 18, nodey - 18}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 18, nodey - 18}, (Vector2){nodex - 18, nodey + 5}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 18, nodey - 18}, (Vector2){nodex + 18, nodey + 5}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 18, nodey + 5}, (Vector2){nodex + 18, nodey + 5}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 3, nodey + 5}, (Vector2){nodex - 3, nodey + 15}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 3, nodey + 5}, (Vector2){nodex + 3, nodey + 15}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 15, nodey + 15}, (Vector2){nodex + 15, nodey + 15}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 15, nodey + 18}, (Vector2){nodex + 15, nodey + 18}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex - 15, nodey + 15}, (Vector2){nodex - 15, nodey + 18}, 2, FIGURE_COLOR);
        DrawLineEx((Vector2){nodex + 15, nodey + 15}, (Vector2){nodex + 15, nodey + 18}, 2, FIGURE_COLOR);

        break;
    }
    DrawText(node->name, nodex - 20, nodey + 40, STD_FONT_SIZE, GRAY);
    // se non sto al momento spostando niente e ci clicco con il tasto sinistro allora inizio a draggarlo
    if (
        true_node &&
        GetMouseX() <= node->x + 20 &&
        GetMouseX() >= node->x - 20 &&
        GetMouseY() <= node->y + 20 &&
        GetMouseY() >= node->y - 20) // sono nell'area del vero nodo
    {
        if (
            IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && // tasto sinistro
            !settings->dragging_deactivated)
        {
            if (!settings->moving_node && !settings->placing_node)
            {
                if (!strcmp(settings->node_name, node->name))
                {
                    settings->node_name = (char *)calloc(NAMELENGTH, sizeof(char));
                }
                else
                {
                    if (settings->placing_link == 3)
                    {
                        settings->placing_link = 0;
                    }
                    else
                    {
                        settings->placing_node = false;
                        settings->deletingNodes = 0;
                        settings->drawing_rectangle = false;
                        settings->moving_node = 1;
                        settings->node_type = node->type;
                        settings->node_name = (char *)calloc(NAMELENGTH, sizeof(char));
                        snprintf(settings->node_name, NAMELENGTH, "%s", node->name);
                    }
                }
            }
        }
        else if (
            IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
        {
            if (settings->isSimulating)
            {
                settings->placing_node = 0;
                settings->placing_link = 0;
                settings->drawing_rectangle = 0;
                settings->moving_node = 0;
                settings->node_type = node->type;
                settings->dragging_deactivated = true;
                settings->deletingNodes = 0;
                settings->node_name = (char *)calloc(NAMELENGTH, sizeof(char));
                snprintf(settings->node_name, NAMELENGTH, "%s", node->name);
                openShell(settings);
            }
            else
            {
                if (settings->openProjectName == NULL)
                {
                    printf("Save the project first!\n");
                }
                else
                {
                    char temp[200];
                    snprintf(temp, 200, "%s.conf", settings->openProjectName);
                    int a = 0;
                    if ((a = fork()) == 0)
                    {
                        char *args[] = {"x-terminal-emulator", "-e", "vim", temp, NULL};
                        execvp(args[0], args);
                    }
                    else if (a > 0)
                    {
                        // waitpid
                        waitpid(a, NULL, 0);
                    }
                    else
                        printf("Wasn't able to open project configs, sorry\n");
                }
            }
        }
    }
}

void setNode(char *name, node_t *nodes, settings_t *settings)
{
    for (int i = 0; i < settings->numnodes; i++)
    {
        if (!strcmp(name, nodes[i].name))
        {
            nodes[i].x = GetMouseX();
            nodes[i].y = GetMouseY();
        }
    }
}

void getName(settings_t *settings)
{
    // TODO bugged, don't care
    static void (*functions[2])(settings_t *) = {openProject, saveProject};
    char *toPrint = (char *)calloc(300, sizeof(char));
    char character = GetCharPressed();
    if (IsKeyReleased(KEY_ENTER))
    {
        functions[settings->gettingName - 1](settings); // TODO submit
        settings->resetName = true;
        settings->gettingName = 0;
        return;
    }
    if (character >= 32 && character <= 126)
    {
        // valid letter
        char temp[200];
        snprintf(temp, 200, "%s%c", settings->filename, character);
        strncpy(settings->filename, temp, 200);
    }
    else if (IsKeyReleased(KEY_BACKSPACE))
    {
        settings->filename[strlen(settings->filename) - 1] = '\0';
    }
    snprintf(toPrint, 300, "Insert the name of the file: ./saves/%s", settings->filename);
    DrawText(toPrint, 1100, 20, STD_FONT_SIZE, GRAY);
    free(toPrint);
}

char *identify(int num)
{
    return (char[10][10]){"hu", "s", "r", "h", "e", "en"}[num]; // hub, switch, router, host, external interface, external natted interface
}

void appendNode(interface_t *interface, node_t newnode, settings_t *settings)
{
    if (settings->numnodes == 0)
    {
        interface->nodes = (node_t *)calloc(1, sizeof(node_t));
    }
    else
        interface->nodes = (node_t *)realloc(interface->nodes, (settings->numnodes + 1) * sizeof(node_t));
    interface->nodes[settings->numnodes].name = (char *)calloc(NAMELENGTH, sizeof(char));
    strncpy(interface->nodes[settings->numnodes].name, newnode.name, 49);
    interface->nodes[settings->numnodes].type = newnode.type;
    interface->nodes[settings->numnodes].x = newnode.x;
    interface->nodes[settings->numnodes].y = newnode.y;
}

void appendLink(interface_t *interface, settings_t *settings, link_t link)
{
    if (settings->numlink == 0)
    {
        interface->links = (link_t *)calloc(1, sizeof(link_t));
    }
    else
        interface->links = (link_t *)realloc(interface->links, (settings->numlink + 1) * sizeof(link_t)); // reallocating if not first link
    interface->links[settings->numlink].node1 = (char *)calloc(NAMELENGTH, sizeof(char));                 // allocating memory for interface names
    interface->links[settings->numlink].node2 = (char *)calloc(NAMELENGTH, sizeof(char));
    strncpy(interface->links[settings->numlink].node1, link.node1, 49); // setting interface names
    strncpy(interface->links[settings->numlink].node2, link.node2, 49);
    interface->links[settings->numlink].node1_type = link.node1_type;
    interface->links[settings->numlink].node2_type = link.node2_type;
    // printf("allocated %s that is a %d and %s that is a %d\n", interface->links[settings->numlink].node1, interface->links[settings->numlink].node1_type, interface->links[settings->numlink].node2, interface->links[settings->numlink].node2_type);
}

node_t *getInversePos(int x, int y, node_t *nodes, settings_t *settings)
{ // now returns entire struct node (node type is needed to handle different types of links in logical controller)
    for (int i = 0; i < settings->numnodes; i++)
    {
        if (
            x <= nodes[i].x + 20 &&
            x >= nodes[i].x - 20 &&
            y <= nodes[i].y + 20 &&
            y >= nodes[i].y - 20)
        {
            return &nodes[i];
        }
    }
    return NULL;
}

link_t *getInverseLink(interface_t *interface, settings_t *settings)
{ // now returns entire struct link

    for (int i = 0; i < settings->numlink; i++)
    {
        Vector2 positions[2] = {getPos(interface->nodes, interface->links[i].node1, settings), getPos(interface->nodes, interface->links[i].node2, settings)};

        if (positions[0].x == -1 || positions[0].y == -1 || positions[1].x == -1 || positions[1].y == -1) // maybe yes, maybe no
            return NULL;

        if(isNearLink(positions))
            return &interface->links[i];

    }
    return NULL;
}

bool isSomethingUnder(int x, int y, node_t *nodes, settings_t *settings)
{
    for (int i = 0; i < settings->numnodes; i++)
    {
        if (
            x <= nodes[i].x + 20 &&
            x >= nodes[i].x - 20 &&
            y <= nodes[i].y + 20 &&
            y >= nodes[i].y - 20)
        {
            return true;
        }
    }
    return false;
}

void DrawMessageAtAngle(char *message)
{
    DrawText(message, 1900 - STD_FONT_SIZE / 2 * strlen(message), 970, STD_FONT_SIZE, GRAY);
}

void DrawRectangleWrapper(rectangle_t *rectangle)
{
    // printf("drawing the rectangle %p\n",rectangle);
    DrawRectangle(min(rectangle->x, rectangle->x1), min(rectangle->y, rectangle->y1), abs(rectangle->x1 - rectangle->x), abs(rectangle->y1 - rectangle->y), CLITERAL(Color){rectangle->r, rectangle->g, rectangle->b, 63});
    DrawRectangleLines(min(rectangle->x, rectangle->x1), min(rectangle->y, rectangle->y1), abs(rectangle->x1 - rectangle->x), abs(rectangle->y1 - rectangle->y), CLITERAL(Color){rectangle->r, rectangle->g, rectangle->b, 255});
}

void addRectangle(interface_t *interface, settings_t *settings)
{
    if (settings->numrectangles == 0)
        interface->rectangles = (rectangle_t *)calloc(1, sizeof(rectangle_t));
    else
        interface->rectangles = (rectangle_t *)realloc(interface->rectangles, (settings->numrectangles + 1) * sizeof(rectangle_t));
    interface->rectangles[settings->numrectangles].x = settings->posX;
    interface->rectangles[settings->numrectangles].y = settings->posY;
    interface->rectangles[settings->numrectangles].x1 = GetMouseX();
    interface->rectangles[settings->numrectangles].y1 = GetMouseY();
    interface->rectangles[settings->numrectangles].r = rand() % 256;
    interface->rectangles[settings->numrectangles].g = rand() % 256;
    interface->rectangles[settings->numrectangles].b = rand() % 256;
}

void DrawGUI(settings_t *settings, interface_t *interface)
{

    // 1. piazzo i buttons
    for (int i = 0; i < NUMbuttons; i++)
        DrawButton(&(interface->buttons[i]), settings);

    // 2. se premo esc disattivo dragging e placing TODO bugged
    if (IsKeyReleased(KEY_ESCAPE))
    {
        settings->moving_node = false;
        settings->placing_node = false;
        settings->drawing_rectangle = false;
        settings->placing_link = false;
        settings->node_type = -1;
        settings->deletingNodes = 0;
        settings->gettingName = 0;
        settings->resetName = 1;
        free(settings->node_name);
        settings->node_name = (char*)calloc(NAMELENGTH,sizeof(char));
    }

    // 3. se sto spostando cose
    if (settings->moving_node)
    {
        // giusto per sicurezza, disattivo il placing se sto piazzando cose
        //settings->placing_node = false;
        // disegno un nodo "fantasma" dove sto muovendo il mouse
        DrawNode(&(node_t){settings->node_name, settings->node_type, GetMouseX(), GetMouseY()}, settings, false);
        // se sto premendo (rilasciando, tbh) il tasto sinistro del mouse
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            // smetto di spostare il nodo
            settings->moving_node = false;
            // ne imposto le posizioni
            setNode(settings->node_name, interface->nodes, settings);
        }
        else DrawMessageAtAngle("Select a new position for the node");
    }

    // 4. altrimenti, se sto posizionando qualcosa di nuovo
    else if (settings->placing_node)
    {
        // creo il name del nuovo nodo
        char name[50];
        snprintf(name, 50, "%s-%d", identify(settings->node_type), settings->absoluteCount);
        // disegno un nodo "fantasma" dove sto muovendo il mouse
        DrawNode(&(node_t){name, settings->node_type, GetMouseX(), GetMouseY()}, settings, false);
        // se premo il mouse
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            // allora non lo sto più piazzando
            settings->placing_node = false;
            // aggiungo il nodo alla lista
            appendNode(interface, (node_t){name, settings->node_type, GetMouseX(), GetMouseY()}, settings);
            // e aumento il numero di nodes
            settings->numnodes++;
            settings->absoluteCount++;
        }
        else
            DrawMessageAtAngle("Choose the position for this new node");
    }

    // 5. altrimenti, se sto posizionando un link
    else if (settings->placing_link)
    {
        // se ho già selezionato il primo nodo
        if (settings->placing_link == 1 && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && isSomethingUnder(GetMouseX(), GetMouseY(), interface->nodes, settings))
        {
            // printf("%s\n",getInversePos(GetMouseX(),GetMouseY(),interface->nodes,settings));
            strncpy(settings->first_place, getInversePos(GetMouseX(), GetMouseY(), interface->nodes, settings)->name, 50);      // adding name of first selected node to settings
            settings->first_place_nodetype = (void *)getInversePos(GetMouseX(), GetMouseY(), interface->nodes, settings)->type; // adding type of first selected node to settings
            settings->dragging_deactivated = true;
            settings->placing_link = 2;
        }
        else if (settings->placing_link == 1)
        {
            DrawCircle(GetMouseX(), GetMouseY(), 5, PURPLE);
            DrawMessageAtAngle("Select the first node to link");
        }
        // altrimenti
        else if (settings->placing_link == 2 && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && isSomethingUnder(GetMouseX(), GetMouseY(), interface->nodes, settings))
        {
            settings->placing_link = 3;
            // funny modified function that now returns link_t with the extra needed info too
            appendLink(interface, settings, (link_t){settings->first_place, getInversePos(GetMouseX(), GetMouseY(), interface->nodes, settings)->name, (component_type_t)settings->first_place_nodetype, getInversePos(GetMouseX(), GetMouseY(), interface->nodes, settings)->type});
            // aumento il numero di link
            settings->numlink++;
            settings->dragging_deactivated = false;
        }
        else if (settings->placing_link == 2)
        {
            DrawFakeLink(settings, interface->nodes);
            DrawMessageAtAngle("Select the second node to link");
        }
    }

    else if (settings->drawing_rectangle)
    {
        // TODO draw rectangle with all the cases and prompting for the color
        if (settings->drawing_rectangle == 1)
        {
            settings->dragging_deactivated = true;
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                settings->posX = GetMouseX();
                settings->posY = GetMouseY();
                settings->drawing_rectangle = 2;
            }
            else
            {
                DrawRectangle(GetMouseX(), GetMouseY(), 5, 5, PURPLE);
                DrawMessageAtAngle("Select the first angle");
            }
        }
        else if (settings->drawing_rectangle == 2)
        {
            settings->dragging_deactivated = true;
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                settings->dragging_deactivated = false;
                settings->drawing_rectangle = 0;
                addRectangle(interface, settings);
                settings->numrectangles += 1;
            }
            else
            {
                DrawMessageAtAngle("Select the second angle");
                DrawRectangleLines(min(settings->posX, GetMouseX()), min(settings->posY, GetMouseY()), abs(GetMouseX() - settings->posX), abs(GetMouseY() - settings->posY), PURPLE);
            }
        }
        else
        {
            settings->drawing_rectangle = 1;
        }
    }
    else if (settings->deletingNodes) // deleting nodes or links
    {
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            node_t *nodo = getInversePos(GetMouseX(), GetMouseY(), interface->nodes, settings);
            char *stringa = (char *)calloc(200, sizeof(char));
            if (nodo)
            {
                strncpy(stringa, nodo->name, 200);

                /* char a = false; <= we will never forget you */

                settings->numnodes--;

                nodo->name = strdup(interface->nodes[settings->numnodes].name);
                nodo->x = interface->nodes[settings->numnodes].x;
                nodo->y = interface->nodes[settings->numnodes].y;
                nodo->type = interface->nodes[settings->numnodes].type;

                for (int i = 0; i < settings->numlink; i++)
                {
                    if (!strcmp(interface->links[i].node1, stringa) || !strcmp(interface->links[i].node2, stringa))
                    {
                        settings->numlink--;

                        interface->links[i].node1 = strdup(interface->links[settings->numlink].node1);
                        interface->links[i].node2 = strdup(interface->links[settings->numlink].node2);
                        interface->links[i].node1_type = interface->links[settings->numlink].node1_type;
                        interface->links[i].node2_type = interface->links[settings->numlink].node2_type;
                    }
                }

                free(stringa);
            }
            else
            { // deleting (maybe) a link
                link_t *link = getInverseLink(interface, settings);
                if (link)
                {
                    settings->numlink--;

                    link->node1 = strdup(interface->links[settings->numlink].node1);
                    link->node2 = strdup(interface->links[settings->numlink].node2);
                    link->node1_type = interface->links[settings->numlink].node1_type;
                    link->node2_type = interface->links[settings->numlink].node2_type;
                }
            }
        }
        else
            DrawMessageAtAngle("Select the node or the link to delete");
    }

    if (settings->gettingName)
    {
        if (settings->resetName)
        {
            if (settings->filename != NULL)
                free(settings->filename);
            settings->filename = (char *)calloc(200, sizeof(char));
            settings->resetName = 0;
        }
        getName(settings);
    }

    // posiziono i rettangoli
    for (int i = 0; i < settings->numrectangles; i++)
        DrawRectangleWrapper(&(interface->rectangles[i]));

    // ora posiziono tutto: nodes e link
    for (int i = 0; i < settings->numlink; i++)
        DrawLink(interface->links[i], settings, interface->nodes);
    for (int i = 0; i < settings->numnodes; i++)
        DrawNode(&(interface->nodes[i]), settings, true);
}
