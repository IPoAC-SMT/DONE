#include "../lib/logicalController.h"

void helloworld(settings_t *settings)
{
    printf("%d\n", settings->numlink);
}

void placehub(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    settings->moving_node = 0;
    settings->placing_link = 0;
    settings->drawing_rectangle = 0;
    settings->placing_node = 1;
    settings->node_type = 0;
}

void placeswitch(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    settings->placing_link = 0;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 1;
}

void placerouter(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    settings->placing_link = 0;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 2;
}

void placehost(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    settings->placing_link = 0;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 3;
}

void placeexternalinterface(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    settings->placing_link = 0;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 4;
}

void placeexternalnattedinterface(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    settings->placing_link = 0;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 5;
}

void placelink(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 0;
    settings->placing_link = 1;
}

void placeRectangle(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    settings->drawing_rectangle = -1;
    settings->moving_node = 0;
    settings->placing_node = 0;
    settings->placing_link = 0;
}
void initEnvironment()
{
    initEnv();
}

void start(settings_t *settings)
{ // sending data to logical controller, that starts the simulation
    settings->isSimulating = 42;
    startSimulation((interface_t *)settings->GUIdata, settings->numnodes, settings->numlink);
}

void stop(settings_t *settings)
{ // sending data, again, to logical controller, that stops the simulation
    settings->isSimulating = 0;
    stopSimulation((interface_t *)settings->GUIdata, settings->numnodes, settings->numlink);
}

void openShell(settings_t *settings)
{
    settings->node_type == switch_t ? openSwitchShellWrapper() : openNodeShellWrapper(settings->node_name);
}

void openProject(settings_t *settings)
{
}

void saveProject(settings_t *settings)
{
}

void clearCanvas(settings_t *settings)
{
    settings->numnodes = 0;
    settings->numlink = 0;
    interface_t *interface = (interface_t *)settings->GUIdata;
    interface->links = NULL;
    interface->nodes = NULL;
    interface->rectangles = NULL;
}