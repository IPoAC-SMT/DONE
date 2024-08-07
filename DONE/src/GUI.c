#include "../lib/GUI.h"
#include "../lib/log.h"

void DrawButton(button_t *pulsante, settings_t *settings)
{
    if ((!settings->isSimulating && !settings->isClient) || pulsante->always_show)
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
                DrawText(pulsante->alt_text, GetMouseX() + 20, GetMouseY() + 20, STD_FONT_SIZE, FIGURE_COLOR);
            }
        }
        else
            hovering = false;

        DrawRectangleLines(pulsante->x, pulsante->y, pulsante->width, pulsante->height, hovering ? YELLOW : FIGURE_COLOR); // lo creo con colore diverso se ci sto o meno hoverando

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
            DrawLineEx((Vector2){pulsante->edges[i][0] + pulsante->x, pulsante->edges[i][1] + pulsante->y}, (Vector2){pulsante->edges[i][2] + pulsante->x, pulsante->edges[i][3] + pulsante->y}, hovering ? 3 : 1, hovering ? YELLOW : FIGURE_COLOR);
        }
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
    if (isNearLink(positions))
    {
        char tmp[100];
        // snprintf(tmp,99,"%cveth-%s-%s/%cveth-%s-%s",);
        if (link.node1_type == switch_t || link.node2_type == switch_t)
        {
            snprintf(tmp, 99, "sveth-%s-%s", (link.node1_type == switch_t ? link.node2 : link.node1), (link.node1_type == switch_t ? link.node1 : link.node2));
        }
        else
        {
            snprintf(tmp, 99, "veth-%s-%s/veth-%s-%s", link.node1, link.node2, link.node2, link.node1);
        }
        DrawText(tmp, GetMouseX() + 10, GetMouseY() - 10, STD_FONT_SIZE, FIGURE_COLOR);
    }
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
        /*case the_Internet_t:
                //DrawRectangle(nodex-20, nodey-20, 40, 40, BLACK);
                DrawLineEx((Vector2){nodex - 15, nodey + 10}, (Vector2){nodex + 14, nodey + 10}, 2, FIGURE_COLOR);
                DrawRing((Vector2){nodex-15,nodey+7}, 2, 4, 180, 360, 0, FIGURE_COLOR);
                DrawRing((Vector2){nodex+14,nodey+6}, 3, 5, 210, 0, 0, FIGURE_COLOR);
                DrawRing((Vector2){nodex-9,nodey+1}, 4, 6, 160, 300, 0, FIGURE_COLOR);
                DrawRing((Vector2){nodex+2,nodey-1}, 9,11 , 76, 267, 0, FIGURE_COLOR);
            break;*/
    case the_Internet_t:
        // DrawRectangle(nodex-20, nodey-20, 40, 40, BLACK);
        DrawLineEx((Vector2){nodex - 15, nodey + 10}, (Vector2){nodex + 14, nodey + 10}, 2, FIGURE_COLOR);
        DrawRing((Vector2){nodex - 15, nodey + 7}, 2, 4, 180, 360, 0, FIGURE_COLOR);
        DrawRing((Vector2){nodex + 14, nodey + 6}, 3, 5, 210, 0, 0, FIGURE_COLOR);
        DrawRing((Vector2){nodex - 9, nodey + 1}, 4, 6, 160, 300, 0, FIGURE_COLOR);
        DrawRing((Vector2){nodex + 2, nodey - 1}, 9, 11, 76, 267, 0, FIGURE_COLOR);
        break;
    }
    DrawText(node->name, nodex - 20, nodey + 40, STD_FONT_SIZE, FIGURE_COLOR);
    // se non sto al momento spostando niente e ci clicco con il tasto sinistro allora inizio a draggarlo
    if (
        true_node &&
        GetMouseX() <= node->x + 20 &&
        GetMouseX() >= node->x - 20 &&
        GetMouseY() <= node->y + 20 &&
        GetMouseY() >= node->y - 20) // sono nell'area del vero node
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
                if (!settings->isClient)
                {
                    if (node->type == external_interface_t || node->type == external_natted_interface_t)
                    {
                        populateInterfaceOptionsWrapper(settings);
                    }
                    else
                    {
                        if (settings->openProjectName == NULL)
                        {
                            logWarning("Save the project first!", "");
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
                                logWarning("Wasn't able to open project configs", "sorry");
                        }
                    }
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
    DrawText(toPrint, 1100, 20, STD_FONT_SIZE, FIGURE_COLOR);
    free(toPrint);
    toPrint = NULL;
}

void getServerIp(settings_t *settings) // the bug is not here
{
    char *toPrint = (char *)calloc(300, sizeof(char));
    char character = GetCharPressed();
    if (!settings->tmpIp)
    {
        settings->tmpIp = (char *)calloc(NAMELENGTH, sizeof(char));
    }
    if (IsKeyPressed(KEY_ENTER))
    {
        if (validateIP(settings, settings->tmpIp))
        {
            // getData(settings,interface); // submit, not really needed
            settings->tmpIp = NULL;
            settings->gettingIp = 0;
        }
        else
        {
            settings->tmpIp = NULL;
        }
    }
    else if (character >= 32 && character <= 126)
    {
        // valid letter
        char *temp = (char *)calloc(200, sizeof(char));
        snprintf(temp, 199, "%s%c", settings->tmpIp, character);
        strncpy(settings->tmpIp, temp, 200);
    }
    else if (IsKeyReleased(KEY_BACKSPACE))
    {
        settings->tmpIp[strlen(settings->tmpIp) - 1] = '\0';
    }
    snprintf(toPrint, 299, "Insert the server IP: %s", settings->tmpIp);
    DrawText(toPrint, 1100, 20, STD_FONT_SIZE, FIGURE_COLOR);
    // free(toPrint);
}

char *identify(int num)
{
    return (char[10][10]){"s", "r", "h", "e", "en", "I"}[num]; // hub, switch, router, host, external interface, external natted interface, Internet
}

char *identifyType(int num)
{
    return (char[30][30]){"switch", "router", "host", "external interface", "external natted interface", "internet"}[num]; // hub, switch, router, host, external interface, external natted interface, Internet
}

void appendText(settings_t *settings, interface_t *interface)
{
    if (settings->numTexts == 0)
    {
        interface->texts = (text_t *)calloc(1, sizeof(text_t));
    }
    else
        interface->texts = (text_t *)realloc(interface->texts, (settings->numTexts + 1) * sizeof(text_t));
    interface->texts[settings->numTexts].text = (char *)calloc(NAMELENGTH, sizeof(char));
    strncpy(interface->texts[settings->numTexts].text, settings->tmpText, 49);
    interface->texts[settings->numTexts].x = settings->tmpx;
    interface->texts[settings->numTexts].y = settings->tmpy;
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

        if (isNearLink(positions))
            return &interface->links[i];
    }
    return NULL;
}

text_t *getInverseText(interface_t *interface, settings_t *settings)
{
    for (int i = settings->numTexts - 1; i >= 0; i--)
    {
        if (
            interface->texts[i].x - 3 <= GetMouseX() &&
            interface->texts[i].y - 3 <= GetMouseY() &&
            (interface->texts[i].x + (int)strlen(interface->texts[i].text) * STD_FONT_SIZE / 2 + 3) >= GetMouseX() &&
            interface->texts[i].y + STD_FONT_SIZE + 3 >= GetMouseY())
            return &interface->texts[i];
    }
    return NULL;
}

rectangle_t *getInverseRectangle(interface_t *interface, settings_t *settings)
{
    for (int i = settings->numrectangles - 1; i >= 0; i--)
    {
        if (
            min(interface->rectangles[i].x, interface->rectangles[i].x1) <= GetMouseX() &&
            min(interface->rectangles[i].y, interface->rectangles[i].y1) <= GetMouseY() &&
            max(interface->rectangles[i].x, interface->rectangles[i].x1) >= GetMouseX() &&
            max(interface->rectangles[i].y, interface->rectangles[i].y1) >= GetMouseY())
            return &interface->rectangles[i];
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
    DrawText(message, 1900 - STD_FONT_SIZE / 2 * strlen(message), 970, STD_FONT_SIZE, FIGURE_COLOR);
}

void DrawRectangleWrapper(rectangle_t *rectangle)
{
    // printf("drawing the rectangle %p\n",rectangle);
    DrawRectangle(min(rectangle->x, rectangle->x1), min(rectangle->y, rectangle->y1), abs(rectangle->x1 - rectangle->x), abs(rectangle->y1 - rectangle->y), CLITERAL(Color){rectangle->r, rectangle->g, rectangle->b, 63});
    DrawRectangleLines(min(rectangle->x, rectangle->x1), min(rectangle->y, rectangle->y1), abs(rectangle->x1 - rectangle->x), abs(rectangle->y1 - rectangle->y), CLITERAL(Color){rectangle->r, rectangle->g, rectangle->b, 255});
}

void addRectangle(interface_t *interface, settings_t *settings)
{
    if (settings->numrectangles == 0){
        printf("hereee!\n");
        interface->rectangles = (rectangle_t *)calloc(1, sizeof(rectangle_t));
    }else{
        rectangle_t *backup_rectangles = interface->rectangles;
        interface->rectangles = (rectangle_t *)calloc(settings->numrectangles + 1, sizeof(rectangle_t));
        for(int i = 0; i < settings->numrectangles; i++){
            printf("here\n");
            interface->rectangles[i] = backup_rectangles[i];
        }   
    }
    interface->rectangles[settings->numrectangles].x = settings->posX;
    interface->rectangles[settings->numrectangles].y = settings->posY;
    interface->rectangles[settings->numrectangles].x1 = GetMouseX();
    interface->rectangles[settings->numrectangles].y1 = GetMouseY();
    interface->rectangles[settings->numrectangles].r = rand() % 256;
    interface->rectangles[settings->numrectangles].g = rand() % 256;
    interface->rectangles[settings->numrectangles].b = rand() % 256;
    printf("numrects: %d\n", settings->numrectangles);
}

void export(settings_t *settings, interface_t *interface)
{
    logInfo("Ready to export", "as DoneScript.ds");
    FILE *ptr = fopen("DoneScript.ds", "w");
    for (int i = 0; i < settings->numnodes; i++)
    {
        fprintf(ptr, "create %s at %d %d as %s\n", identifyType(interface->nodes[i].type), interface->nodes[i].x, interface->nodes[i].y, interface->nodes[i].name);
    }
    for (int i = 0; i < settings->numlink; i++)
    {
        fprintf(ptr, "link %s and %s\n", interface->links[i].node1, interface->links[i].node2);
    }
    for (int i = 0; i < settings->numrectangles; i++)
    {
        fprintf(ptr, "draw rectangle between %d %d and %d %d with color %d %d %d\n", interface->rectangles[i].x, interface->rectangles[i].y, interface->rectangles[i].x1, interface->rectangles[i].y1, interface->rectangles[i].r, interface->rectangles[i].g, interface->rectangles[i].b);
    }
    for (int i = 0; i < settings->numTexts; i++)
    {
        fprintf(ptr, "add text \"%s\" at %d %d\n", interface->texts[i].text, interface->texts[i].x, interface->texts[i].y);
    }
    if (settings->openProjectName)
    { // if a project is open, we need to eventually load configs
        char config_filename[50];
        strcpy(config_filename, settings->openProjectName);
        strcat(config_filename, ".conf");
        FILE *file = fopen(config_filename, "r");
        if (file != NULL)
        {
            char command[1024] = {0};
            char buf[200]; // buffer for reading the file
            char *nodeName;

            while (fgets(buf, 200, file)) // reading the node name line
            {
                nodeName = strtok(strdup(buf), ":"); // retrieving the node name from the config
                if (nodeName != NULL)
                {
                    do
                    {
                        while (fgets(buf, 200, file))
                        {
                            if (buf[0] == '\n' && !strlen(command))
                                break;
                            buf[strlen(buf)] = 0;
                            if (!strlen(command))
                                strcpy(command, buf);
                            else
                                strcat(command, buf);
                            if (strchr(buf, ';') != NULL)
                                break;
                        }

                        if (strlen(command))
                        {
                            command[strlen(command) - 2] = '\0';
                            fprintf(ptr, "send command to %s begin script\n%s\nend script\n", nodeName, command);
                            memset(command, 0, 1024);
                        }
                    } while (buf[0] != '\n');
                }
                else
                {
                    logError("error", "");
                    fclose(file);
                    fclose(ptr);
                    return;
                }
            }
            logSuccess("Simulation running", "setup finished");
        }
    }
    /*
        if (settings->openProjectName)
        {
            char config_filename[50];
            snprintf(config_filename, 50, "%s.conf", settings->openProjectName);
            FILE *file = fopen(config_filename, "r");
            if (file)
            {
                char buf[200];
                char *nodeName;

                while (fgets(buf, 200, file))
                {
                    nodeName = strtok(strdup(buf), ":");
                    if (nodeName)
                    {
                        do
                        {
                            if (!fgets(buf, 200, file) || buf[0] == '\n')
                                break;
                            fprintf(ptr, "send command to %s %s", nodeName, buf);
                        } while (42);
                    }
                }
            }
        }
        */
    fclose(ptr);
    logSuccess("Exporting as DoneScript", "you can find it as DoneScript.ds");
}

void DrawTextWrapped(text_t text)
{
    DrawText(text.text, text.x, text.y, STD_FONT_SIZE, FIGURE_COLOR);
    DrawRectangleLines(text.x - 3, text.y - 3, strlen(text.text) * STD_FONT_SIZE / 2 + 6, STD_FONT_SIZE + 6, BLUE);
}

void DrawGUI(settings_t *settings, interface_t *interface)
{
    if (settings->isSimulating)
        DrawMessageAtAngle("Simulation in progress...");

    // 1. piazzo i buttons
    for (int i = 0; i < NUMbuttons; i++)
        DrawButton(&(interface->buttons[i]), settings);

    // 2. se premo esc disattivo dragging e placing TODO bugged
    if (IsKeyReleased(KEY_ESCAPE))
    {
        logInfo("Escape key pressed", "");
        settings->moving_node = false;
        settings->placing_node = false;
        settings->drawing_rectangle = false;
        settings->placing_link = false;
        settings->node_type = -1;
        settings->deletingNodes = 0;
        settings->gettingName = 0;
        settings->resetName = 1;
        settings->placing_text = 0;
        logSuccess("All effects successfully deactivated", "esc worked correctly");
    }

    // 3. se sto spostando cose
    if (settings->moving_node)
    {
        // giusto per sicurezza, disattivo il placing se sto piazzando cose
        // settings->placing_node = false;
        // disegno un node "fantasma" dove sto muovendo il mouse
        DrawNode(&(node_t){settings->node_name, settings->node_type, GetMouseX(), GetMouseY()}, settings, false);
        // se sto premendo (rilasciando, tbh) il tasto sinistro del mouse
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            // smetto di spostare il node
            settings->moving_node = false;
            // ne imposto le posizioni
            getWriteLock(settings);
            setNode(settings->node_name, interface->nodes, settings);
            releaseWriteLock(settings);
            logSuccess("Correctly moved node", "");
        }
        else
            DrawMessageAtAngle("Select a new position for the node");
    }

    // 4. altrimenti, se sto posizionando qualcosa di nuovo
    else if (settings->placing_node)
    {
        // creo il name del nuovo node
        char name[50];
        snprintf(name, 50, "%s-%d", identify(settings->node_type), settings->absoluteCount);
        // disegno un node "fantasma" dove sto muovendo il mouse
        DrawNode(&(node_t){name, settings->node_type, GetMouseX(), GetMouseY()}, settings, false);
        // se premo il mouse
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            // allora non lo sto più piazzando
            settings->placing_node = false;
            // aggiungo il node alla lista
            getWriteLock(settings);
            appendNode(interface, (node_t){name, settings->node_type, GetMouseX(), GetMouseY()}, settings);
            // e aumento il numero di nodes
            settings->numnodes++;
            settings->absoluteCount++;
            releaseWriteLock(settings);
            logSuccess("Correctly created node", "");
        }
        else
            DrawMessageAtAngle("Choose the position for this new node");
    }

    // 5. altrimenti, se sto posizionando un link
    else if (settings->placing_link)
    {
        // se ho già selezionato il primo node
        if (settings->placing_link == 1 && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && isSomethingUnder(GetMouseX(), GetMouseY(), interface->nodes, settings))
        {
            // printf("%s\n",getInversePos(GetMouseX(),GetMouseY(),interface->nodes,settings));
            strncpy(settings->first_place, getInversePos(GetMouseX(), GetMouseY(), interface->nodes, settings)->name, 50);      // adding name of first selected node to settings
            settings->first_place_nodetype = (void *)getInversePos(GetMouseX(), GetMouseY(), interface->nodes, settings)->type; // adding type of first selected node to settings
            settings->dragging_deactivated = true;
            settings->placing_link = 2;
            logInfo("First node selected", "now select the second");
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
            getWriteLock(settings);
            appendLink(interface, settings, (link_t){settings->first_place, getInversePos(GetMouseX(), GetMouseY(), interface->nodes, settings)->name, (component_type_t)settings->first_place_nodetype, getInversePos(GetMouseX(), GetMouseY(), interface->nodes, settings)->type});
            // aumento il numero di link
            settings->numlink++;
            releaseWriteLock(settings);
            settings->dragging_deactivated = false;
            logSuccess("Correctly linked nodes", "");
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
                logInfo("Saved first point", "now select the opposite");
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
                getWriteLock(settings);
                addRectangle(interface, settings);
                settings->numrectangles += 1;
                releaseWriteLock(settings);
                logSuccess("Rectangle successfully placed", "");
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
    else if (settings->deletingNodes) // deleting nodes or links or maybe rectangles or maybe texts?
    {
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            node_t *node = getInversePos(GetMouseX(), GetMouseY(), interface->nodes, settings);

            char *stringa = (char *)calloc(200, sizeof(char));
            if (node)
            {
                getWriteLock(settings);

                for (int i = 0; i < settings->numBindings; i++)
                { // checking if bindings preferences were specified for the node. If so, delete them
                    if (!strcmp(settings->interfaceBindings[i].deviceName, node->name))
                    {
                        settings->interfaceBindings[i].bindingInterfaceName = strdup(settings->interfaceBindings[settings->numBindings - 1].bindingInterfaceName);
                        settings->interfaceBindings[i].deviceName = strdup(settings->interfaceBindings[settings->numBindings - 1].deviceName);
                        settings->numBindings--;
                        settings->interfaceBindings = (binding_t *)realloc(settings->interfaceBindings, settings->numBindings * sizeof(binding_t));
                        logSuccess("Deleted binding", "");
                        break;
                    }
                }

                strncpy(stringa, node->name, 200);

                /* char a = false; <= we will never forget you */

                settings->numnodes--;

                node->name = strdup(interface->nodes[settings->numnodes].name);
                node->x = interface->nodes[settings->numnodes].x;
                node->y = interface->nodes[settings->numnodes].y;
                node->type = interface->nodes[settings->numnodes].type;
                logSuccess("Deleted node", "");

                for (int i = 0; i < settings->numlink; i++)
                {
                    if (!strcmp(interface->links[i].node1, stringa) || !strcmp(interface->links[i].node2, stringa))
                    {
                        settings->numlink--;

                        interface->links[i].node1 = strdup(interface->links[settings->numlink].node1);
                        interface->links[i].node2 = strdup(interface->links[settings->numlink].node2);
                        interface->links[i].node1_type = interface->links[settings->numlink].node1_type;
                        interface->links[i].node2_type = interface->links[settings->numlink].node2_type;
                        logInfo("Deleted related link", "");
                    }
                }
                releaseWriteLock(settings);

                free(stringa);
                stringa = NULL;
            }
            else
            { // deleting (maybe) a link
                link_t *link = getInverseLink(interface, settings);
                if (link)
                {
                    getWriteLock(settings);
                    settings->numlink--;

                    link->node1 = strdup(interface->links[settings->numlink].node1);
                    link->node2 = strdup(interface->links[settings->numlink].node2);
                    link->node1_type = interface->links[settings->numlink].node1_type;
                    link->node2_type = interface->links[settings->numlink].node2_type;
                    releaseWriteLock(settings);
                    logSuccess("Deleted link", "");
                }
                else
                {
                    // maybe deleting a rectangle
                    rectangle_t *rectangle = getInverseRectangle(interface, settings);
                    if (rectangle)
                    {
                        getWriteLock(settings);
                        settings->numrectangles--;
                        rectangle->x = interface->rectangles[settings->numrectangles].x;
                        rectangle->x1 = interface->rectangles[settings->numrectangles].x1;
                        rectangle->y = interface->rectangles[settings->numrectangles].y;
                        rectangle->y1 = interface->rectangles[settings->numrectangles].y1;
                        rectangle->r = interface->rectangles[settings->numrectangles].r;
                        rectangle->g = interface->rectangles[settings->numrectangles].g;
                        rectangle->b = interface->rectangles[settings->numrectangles].b;
                        releaseWriteLock(settings);
                        logSuccess("Deleted rectangle", "");
                    }
                    else
                    {
                        // maybe deleting a text?
                        text_t *text = getInverseText(interface, settings);
                        if (text)
                        {
                            getWriteLock(settings);
                            settings->numTexts--;
                            text->text = strdup(interface->texts[settings->numTexts].text);
                            text->x = interface->texts[settings->numTexts].x;
                            text->y = interface->texts[settings->numTexts].y;
                            releaseWriteLock(settings);
                            logSuccess("Deleted text", "");
                        }
                    }
                }
            }
        }
        else
            DrawMessageAtAngle("Select the node, the link or the rectangle to delete");
    }
    else if (settings->placing_text)
    {
        if (settings->placing_text == -1)
        {
            settings->placing_text = 1;
        }
        else if (settings->placing_text == 1)
        {
            settings->dragging_deactivated = true;
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                settings->tmpx = GetMouseX();
                settings->tmpy = GetMouseY();
                settings->tmpText = (char *)calloc(200, sizeof(char));
                settings->placing_text = 2;
            }
        }
        else if (settings->placing_text == 2)
        {
            DrawText(settings->tmpText, settings->tmpx, settings->tmpy, STD_FONT_SIZE, FIGURE_COLOR);
            char character = GetCharPressed();
            if (IsKeyReleased(KEY_ENTER))
            {
                getWriteLock(settings);
                appendText(settings, interface);
                settings->placing_text = 0;
                settings->numTexts++;
                releaseWriteLock(settings);
                settings->dragging_deactivated = false;
            }
            if (character >= 32 && character <= 126)
            {
                // valid letter
                char temp[200];
                snprintf(temp, 200, "%s%c", settings->tmpText, character);
                strncpy(settings->tmpText, temp, 200);
            }
            else if (IsKeyReleased(KEY_BACKSPACE))
            {
                settings->tmpText[strlen(settings->tmpText) - 1] = '\0';
            }
            // sto accumulando lettere in attesa di un invio, se c'è l'invio aggiungo le cose
        }
    }
    if (settings->gettingName)
    {
        if (settings->resetName)
        {
            if (settings->filename != NULL){
                free(settings->filename);
                settings->filename = NULL;
            }
            settings->filename = (char *)calloc(200, sizeof(char));
            settings->resetName = 0;
        }
        getName(settings);
    }
    else if (settings->gettingIp)
    {
        getServerIp(settings);
    }

    // posiziono i rettangoli
    for (int i = 0; i < settings->numrectangles; i++)
        DrawRectangleWrapper(&(interface->rectangles[i]));

    // ora posiziono tutto: nodes e link
    for (int i = 0; i < settings->numlink; i++)
        DrawLink(interface->links[i], settings, interface->nodes);
    for (int i = 0; i < settings->numnodes; i++)
        DrawNode(&(interface->nodes[i]), settings, true);
    for (int i = 0; i < settings->numTexts; i++)
        DrawTextWrapped(interface->texts[i]);

    if (settings->numOptions)
    {
        DrawRectangle(0, 0, WIDTH, HEIGHT, CLITERAL(Color){252, 245, 229, 150});
        for (int i = 0; i < settings->numOptions; i++)
        {

            if (settings->numOptions)
            {

                if (getInversePos(GetMouseX(), GetMouseY(), interface->nodes, settings) && !settings->chosenNode)
                {
                    settings->chosenNode = strdup(getInversePos(GetMouseX(), GetMouseY(), interface->nodes, settings)->name);
                }

                DrawRectangle(0, 0, WIDTH, HEIGHT, CLITERAL(Color){252, 245, 229, 150});
                int optionHeight = min((HEIGHT - 500) / settings->numOptions, 100);
                for (int i = 0; i < settings->numOptions; i++)
                {

                    bool cond = (i == settings->chosenOption ||
                                 (GetMouseX() > WIDTH / 2 - 300 &&
                                  GetMouseX() < WIDTH / 2 + 300 &&
                                  GetMouseY() > 250 + i * optionHeight &&
                                  GetMouseY() < 250 + (i + 1) * optionHeight));

                    DrawRectangleLines(WIDTH / 2 - 300, 250 + i * optionHeight, 600, optionHeight, cond ? RED : BLUE);
                    DrawText(settings->options[i], WIDTH / 2 - 250, 250 + i * optionHeight + optionHeight / 2, STD_FONT_SIZE, BLACK);
                    if (cond)
                        settings->chosenOption = i;
                    // printf("%d\n",subcond);
                }
                if (
                    IsKeyReleased(KEY_ENTER) ||
                    (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
                     GetMouseX() > WIDTH / 2 - 300 &&
                     GetMouseX() < WIDTH / 2 + 300 &&
                     GetMouseY() > 250 &&
                     GetMouseY() < 250 + (settings->numOptions) * optionHeight))
                {
                    if (settings->chosenNode)
                    {
                        trackChosenInterfBinding(settings);
                        settings->chosenNode = NULL;
                    }
                }
                if (IsKeyReleased(KEY_UP))
                    settings->chosenOption = max(0, settings->chosenOption - 1);
                if (IsKeyReleased(KEY_DOWN))
                    settings->chosenOption = min(settings->numOptions - 1, settings->chosenOption + 1);
            }
        }
    }
    if (settings->exportDoneScript)
    {
        export(settings, interface);
        settings->exportDoneScript = false;
    }
}
