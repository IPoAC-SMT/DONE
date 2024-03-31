#include "../lib/logicalController.h"

#define MAX_FILENAME 50

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
    if (settings->isSimulating)
        return;
    settings->isSimulating = 42;
    startSimulation((interface_t *)settings->GUIdata, settings->numnodes, settings->numlink);
}

void stop(settings_t *settings)
{ // sending data, again, to logical controller, that stops the simulation
    if (!settings->isSimulating)
        return;
    settings->isSimulating = 0;
    stopSimulation((interface_t *)settings->GUIdata, settings->numnodes, settings->numlink);
}

void openShell(settings_t *settings)
{
    if (!settings->isSimulating)
        return;
    settings->node_type == switch_t ? openSwitchShellWrapper() : openNodeShellWrapper(settings->node_name);
}

void clearCanvas(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    if (settings->numlink > 0 || settings->numnodes > 0)
    {
        settings->numnodes = 0;
        settings->numlink = 0;
        interface_t *interface = (interface_t *)settings->GUIdata;
        free(interface->links);
        free(interface->nodes);
        free(interface->rectangles);
    }
}

char *getFilename(){
    char *res = (char *) calloc(MAX_FILENAME, sizeof(char));
    char buf[30];
    strcpy(res, "./saves/");
    printf("Specify the file pathname: ./saves/");
    scanf("%30s", buf);
    strcat(res, buf);
    return res;
}

void openProject(settings_t *settings) // TODO: fix crash
{
    if (settings->isSimulating)
    { // if an experiment was running we need to kill the containers
        stop(settings);
    }

    //clearCanvas(settings); // first, clearing the canvas

    char *filename = getFilename();

    FILE *file = fopen(filename, "r");
    int numnodes, numlinks;
    node_t *nodes;
    link_t *links;

    if (file != NULL)
    {
        fscanf(file, "%d\n%d\n", &numnodes, &numlinks);
        printf("%d %d\n", numnodes, numlinks);
        printf("%d %d\n", settings->numnodes, settings->numlink);

        printf("numnodes: %d\n", numnodes);

        settings->numnodes = numnodes;
        printf("settings->numnodes: %d\n", settings->numnodes);

        // nodes = (node_t *)malloc(numnodes * sizeof(node_t));
        // links = (link_t *)malloc(numlinks * sizeof(link_t));

        char name[50]; // buffer

        /*
        for(int i=0; i < numnodes; i++){    // reading all nodes
            fscanf(file,"%s\n%d\n%d %d\n", name, &nodes[i].type, &nodes[i].x, &nodes[i].y);
            printf("%s\n", name);
            nodes[i].name = (char *)malloc(strlen(name) * sizeof(char));    // allocating memory for node name
            //strcpy(name, nodes[i].name);
            //printf("node name: %s\n", nodes[i].name);
        }
        */
        // for(int i=0; i < numlinks; i++){    // reading all links
        // fscanf(file,"%s\n%d\n%d %d\n",&nodes[i].name, &nodes[i].type, &nodes[i].x, &nodes[i].y);
        //}
    }
    else
    {
        printf("ALERT: There was an error while opening the file. Perhaps the path is wrong?\n");
    }
    free(filename);
}

void saveProject(settings_t *settings)
{
    if (settings->isSimulating)
    { // if an experiment was running we need to kill the containers
        stop(settings);
    }
                    
    char *filename = getFilename();
    FILE *file = fopen(filename, "w");

    if (file != NULL)
    {
        printf("%d\n", settings->numnodes);
        fprintf(file, "%d\n%d\n", settings->numnodes, settings->numlink); // saving node number and link number at the top of the file

        interface_t *gui = (interface_t *)(settings->GUIdata);

        for (int i = 0; i < settings->numnodes; i++)
        { // saving every node
            node_t current_node = gui->nodes[i];
            fprintf(file, "%s\n%d\n%d %d\n", current_node.name, current_node.type, current_node.x, current_node.y); // data for every node
        }

        for (int i = 0; i < settings->numlink; i++)
        { // saving every link
            link_t current_link = gui->links[i];
            fprintf(file, "%s\n%s\n%d\n%d\n", current_link.node1, current_link.node2, current_link.node1_type, current_link.node2_type); // data for every link
        }

        fclose(file);
    }
    else
    {
        printf("ALERT: There was an error while opening the file. Perhaps the path is wrong?\n");
    }
    free(filename);
}