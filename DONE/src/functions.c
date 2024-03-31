#include "../lib/logicalController.h"
#include <string.h>

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
    if (settings->isSimulating || settings->numnodes == 0)
        return;
    settings->isSimulating = 42;
    startSimulation((interface_t *)settings->GUIdata, settings->numnodes, settings->numlink);

    if (settings->openProjectName)
    { // if a project is open, we need to eventually load configs

        char config_filename[50];
        strcpy(config_filename, settings->openProjectName);
        strcat(config_filename, ".conf");
        printf("config_filename: %s\n", config_filename);
        FILE *file = fopen(config_filename, "r");
        if (file != NULL)
        {
            char buf[100]; // buffer for reading the file
            char *nodeName;

            while (fgets(buf, 100, file)) // reading the node name line
            {
                nodeName = strtok(strdup(buf), ":"); // retrieving the node name from the config
                if (nodeName != NULL)
                {
                    do
                    {
                        fgets(buf, 100, file); // reading the command line
                        if (buf == NULL || buf[0] == '\n')
                            break; // if i reached the end of the commands for this node, break and go to the next one
                        printf("buf: %s\n", buf);
                        if (nodeName[0] == 's')
                        {
                            sendSwitchCommand(buf);
                        }
                        else
                        {
                            printf("sending command to node %s: %s\n", nodeName, buf);
                            sendNodeCommand(nodeName, buf); // sending the command to the logical controller
                        }
                    } while (1);
                }
                else
                {
                    printf("error\n");
                    fclose(file);
                    return;
                }
            }
        }
        else
        {
            printf("ALERT: There was an error while opening the config file. Have you touched it?!\n");
        }
    }
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

char *getFilename()
{
    char *res = (char *)calloc(MAX_FILENAME, sizeof(char));
    char buf[20];
    strcpy(res, "./saves/");
    printf("Specify the file pathname: ./saves/");
    scanf("%20s", buf);
    strcat(res, buf);
    return res;
}

void openProject(settings_t *settings) // TODO: fix crash
{
    if (settings->isSimulating)
    { // if an experiment was running we need to kill the containers
        stop(settings);
    }

    clearCanvas(settings); // first, clearing the canvas

    free(settings->openProjectName); // freeing the old project name

    char *filename = getFilename();
    strcat(filename, ".done");

    settings->openProjectName = filename; // updating the project name
    FILE *file = fopen(filename, "r");
    int numnodes, numlinks;
    node_t *nodes;
    link_t *links;

    if (file != NULL)
    {
        fscanf(file, "%d\n%d\n", &numnodes, &numlinks);

        nodes = (node_t *)malloc(numnodes * sizeof(node_t));
        links = (link_t *)malloc(numlinks * sizeof(link_t));

        char name[50]; // buffer
        char othername[50];
        int type, type2;

        // sem_wait(settings->dio_melanzana);

        for (int i = 0; i < numnodes; i++)
        { // reading all nodes

            fscanf(file, "%s\n%d\n%d %d\n", name, &type, &nodes[i].x, &nodes[i].y);
            nodes[i].type = type;
            // printf("%s\n", name);
            nodes[i].name = (char *)malloc(strlen(name) * sizeof(char)); // allocating memory for node name
            strcpy(nodes[i].name, name);
        }

        for (int i = 0; i < numlinks; i++)
        { // reading all links
            fscanf(file, "%s\n%s\n%d\n%d\n", name, othername, &type, &type2);
            links[i].node1_type = type;
            links[i].node2_type = type2;
            links[i].node1 = (char *)malloc(strlen(name) * sizeof(char));
            links[i].node2 = (char *)malloc(strlen(othername) * sizeof(char));
            strcpy(links[i].node1, name);
            strcpy(links[i].node2, othername);
        }

        interface_t *gui = settings->GUIdata;
        gui->nodes = nodes;
        gui->links = links;
        settings->numnodes = numnodes;
        settings->numlink = numlinks;
    }
    else
    {
        printf("ALERT: There was an error while opening the file. Perhaps the path is wrong?\n");
    }
}

void saveProject(settings_t *settings)
{
    if (settings->isSimulating)
    { // if an experiment was running we need to kill the containers
        stop(settings);
    }

    if(settings->openProjectName)
    {
        //free(settings->openProjectName); // freeing the old project name
    }

    char *filename = getFilename();
    strcat(filename, ".done");
    FILE *file = fopen(filename, "w");

    if (file != NULL)
    {
        settings->openProjectName = filename;

        // creating the project file
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

        // creating the config file template
        char *config_filename = (char *)calloc(strlen(filename) + 5, sizeof(char));
        strcpy(config_filename, filename);
        strcat(config_filename, ".conf");

        file = fopen(config_filename, "a"); // creating the config file, and if already exists, dont overwrite it
        if (file != NULL)
        {

            fclose(file);
            file = fopen(config_filename, "r");

            fseek(file, 0, SEEK_END);
            if (!ftell(file)) // if the file is empty
            {
                fclose(file);
                file = fopen(config_filename, "w");
                for (int i = 0; i < settings->numnodes; i++)
                { // saving every node
                    node_t current_node = gui->nodes[i];
                    fprintf(file, "%s:\n\n", current_node.name); // name for every node
                }
            }

            fclose(file);
        }
        else
        {
            printf("ALERT: There was an error while creating the config file. Perhaps the path is wrong?\n");
        }
        //free(config_filename);
    }
    else
    {
        printf("ALERT: There was an error while opening the file. Perhaps the path is wrong?\n");
    }
}