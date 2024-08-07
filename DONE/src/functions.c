#include "../lib/logicalController.h"
#include "../lib/log.h"
#include "../lib/doneserver.h"
#include "../lib/functions.h"
#include <string.h>
#include <arpa/inet.h>

#define MAX_FILENAME 50

void *actAsServer(void *vargp)
{
    return serverFunction(vargp);
}

void exportDoneScript(settings_t *settings)
{
    settings->exportDoneScript = 1;
}

// --- receiving from the GUI the instruction to place or remove a node/link ---

void placeswitch(settings_t *settings)
{
    if (settings->isSimulating || settings->isClient)
        return;
    settings->placing_link = 0;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->gettingIp = 0;
    settings->placing_node = 1;
    settings->node_type = 0;
    settings->isClient = 0;
    settings->deletingNodes = 0;
    settings->placing_text = 0;
    logInfo("Ready to place a node", "type switch");
}

void placerouter(settings_t *settings)
{
    if (settings->isSimulating || settings->isClient)
        return;
    settings->placing_link = 0;
    settings->drawing_rectangle = 0;
    settings->gettingIp = 0;
    settings->isClient = 0;
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 1;
    settings->deletingNodes = 0;
    settings->placing_text = 0;
    logInfo("Ready to place a node", "type router");
}

void placehost(settings_t *settings)
{
    if (settings->isSimulating || settings->isClient)
        return;
    settings->placing_link = 0;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 2;
    settings->gettingIp = 0;
    settings->isClient = 0;
    settings->deletingNodes = 0;
    settings->placing_text = 0;
    logInfo("Ready to place a node", "type host");
}

void placeexternalinterface(settings_t *settings)
{
    if (settings->isSimulating || settings->isClient)
        return;
    settings->placing_link = 0;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->isClient = 0;
    settings->gettingIp = 0;
    settings->placing_node = 1;
    settings->node_type = 3;
    settings->deletingNodes = 0;
    settings->placing_text = 0;
    logInfo("Ready to place a node", "type external interface");
}

void deleteNode(settings_t *settings)
{
    if (settings->isSimulating || settings->isClient)
        return;
    settings->placing_link = 0;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 0;
    settings->gettingIp = 0;
    settings->node_type = 0;
    settings->isClient = 0;
    settings->deletingNodes = 1;
    settings->placing_text = 0;
    logInfo("Ready to delete a node", "");
}

void placeexternalnattedinterface(settings_t *settings)
{
    if (settings->isSimulating || settings->isClient)
        return;
    settings->placing_link = 0;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 4;
    settings->gettingIp = 0;
    settings->isClient = 0;
    settings->deletingNodes = 0;
    settings->placing_text = 0;
    logInfo("Ready to place a node", "type external natted interface");
}

void placelink(settings_t *settings)
{
    if (settings->isSimulating || settings->isClient)
        return;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 0;
    settings->placing_link = 1;
    settings->gettingIp = 0;
    settings->deletingNodes = 0;
    settings->isClient = 0;
    settings->placing_text = 0;
    logInfo("Ready to place a link", "");
}

void placeText(settings_t *settings)
{
    if (settings->isSimulating || settings->isClient)
        return;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->gettingIp = 0;
    settings->placing_node = 0;
    settings->placing_link = 0;
    settings->deletingNodes = 0;
    settings->placing_text = -1;
    settings->isClient = 0;
    logInfo("Ready to place a text", "");
}
void placeRectangle(settings_t *settings)
{
    if (settings->isSimulating || settings->isClient)
        return;
    settings->drawing_rectangle = -1;
    settings->moving_node = 0;
    settings->placing_node = 0;
    settings->gettingIp = 0;
    settings->placing_link = 0;
    settings->deletingNodes = 0;
    settings->placing_text = 0;
    settings->isClient = 0;
    logInfo("Ready to place a rectangle", "");
}
void becomeClient(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 0;
    settings->gettingIp = 0;
    settings->placing_link = 0;
    settings->deletingNodes = 0;
    settings->placing_text = 0;
    settings->isClient = !settings->isClient;
    settings->isClient ? logInfo("Ready to become a client", "") : logInfo("Client stopped", "");
    if (!settings->isClient)
        synchFile(settings);
}

void becomeServer(settings_t *settings)
{
    if (settings->isSimulating || settings->isClient)
        return;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 0;
    settings->gettingIp = 0;
    settings->placing_link = 0;
    settings->deletingNodes = 0;
    settings->placing_text = 0;
    settings->isClient = 0;
    if (!settings->isServer)
    {
        logInfo("Start server command received", "");
        settings->hasToBeServer = 1;
    }
    else
    {
        logInfo("Stop server command received", "");
        settings->deactivateServer = 1;
    }
}

void initEnvironment()
{
    initEnv();
    logSuccess("Environment initialized successfully", "");
}

// get the device type the user clicked on
component_type_t getType(char *nodeName, settings_t *settings)
{
    for (int i = 0; i < settings->numnodes; i++)
    {
        if (!strcmp(((interface_t *)settings->GUIdata)->nodes[i].name, nodeName))
        {
            return ((interface_t *)settings->GUIdata)->nodes[i].type;
        }
    }
    return (component_type_t)NULL;
}

// start the simulation
void start(settings_t *settings)
{ // sending data to logical controller, that starts the simulation
    if (settings->isSimulating || settings->numnodes == 0)
        return;
    settings->isSimulating = 42;
    startSimulation((interface_t *)settings->GUIdata, settings->numnodes, settings->numlink, settings->numBindings, settings->interfaceBindings);

    if (settings->openProjectName)
    { // if a project is open, we need to eventually load configs

        char config_filename[50];
        strcpy(config_filename, settings->openProjectName);
        logInfo("Open Project Name:", "%s", settings->openProjectName);
        strcat(config_filename, ".conf");
        logInfo("Config Filename:", "%s", config_filename);
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
                            command[strlen(command) - 1] = 0; // just removing the '\n' at the end of the command
                            component_type_t type = getType(nodeName, settings);
                            if (type == switch_t)
                            {
                                logInfo("sending to switch:", "%s", command);
                                if (sendSwitchCommand(command))
                                {
                                    logError("An error was found in the config", "");
                                }
                            }
                            else
                            {
                                logInfo("sending command to node", "%s", command); // sending the command to the logical controller
                                if (sendNodeCommand(nodeName, command))
                                {
                                    logError("An error was found in the config", "");
                                }
                            }
                            memset(command, 0, 1024);
                        }
                    } while (buf[0] != '\n');
                }
                else
                {
                    logError("error", "");
                    fclose(file);
                    return;
                }
            }
            logSuccess("Simulation running", "setup finished");
        }
        else
        {
            logWarning("There was an error while opening the config file.", "Have you touched it?!");
        }
    }
    logSuccess("Done!", "");

    for (int i = 0; i < settings->numBindings; i++)
    {
        logInfo("Binding interface", "%s to %s", settings->interfaceBindings[i].deviceName, settings->interfaceBindings[i].bindingInterfaceName);
    }
}

void quit(settings_t *settings)
{
    if (settings->isSimulating)
        logInfo("be careful: it was simulating.", "Now we are stopping the simulation. You're welcome");
    CLIExit();
}

void stop(settings_t *settings)
{ // sending data, again, to logical controller, that stops the simulation
    if (!settings->isSimulating)
        return;
    settings->isSimulating = 0;
    settings->dragging_deactivated = 0;
    stopSimulation((interface_t *)settings->GUIdata, settings->numnodes, settings->numlink);
}

// the user requested to open a node/switch shell
void openShell(settings_t *settings)
{
    if (!settings->isSimulating || settings->isClient)
        return;
    settings->node_type == switch_t ? openSwitchShellWrapper() : openNodeShellWrapper(settings->node_name);
}

// delete the entire topology
void clearCanvas(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    if (settings->numlink > 0 || settings->numnodes > 0 || settings->numrectangles > 0 || settings->numTexts > 0)
    {
        getWriteLock(settings);
        settings->numnodes = 0;
        settings->numlink = 0;
        settings->numrectangles = 0;
        settings->numTexts = 0;
        interface_t *interface = (interface_t *)settings->GUIdata;
        if (settings->numlink){
            free(interface->links);
            interface->links = NULL;
        }
        if (settings->numnodes){
            free(interface->nodes);
            interface->nodes = NULL;
        }
        if (settings->numrectangles){
            free(interface->rectangles);
            interface->rectangles = NULL;
        }
        if (settings->numTexts){
            free(interface->texts);
            interface->texts = NULL;
        }
        releaseWriteLock(settings);
    }
    logSuccess("Canvas cleared", "");
}

// the user requested from the GUI to open a new project, so the GUI will ask for the project name
void prepareToOpenProject(settings_t *settings)
{
    settings->resetName = 1;
    settings->gettingName = 1;
    logInfo("Ready to read project name from GUI", "");
}

// try to open the given file
void openProject(settings_t *settings)
{
    if (settings->isClient)
        return;
    if (settings->isSimulating)
    { // if an experiment was running we need to kill the containers
        stop(settings);
    }

    clearCanvas(settings); // first, clearing the canvas

    if (settings->openProjectName)
    {
        free(settings->openProjectName); // freeing the old project name
        settings->openProjectName = NULL;
    }

    char *filename = (char *)calloc(MAX_FILENAME, sizeof(char));
    snprintf(filename, MAX_FILENAME, "./saves/%s.done", settings->filename);

    FILE *file = fopen(filename, "r");
    int numnodes, numlinks, numrectangles, numtextboxes;
    node_t *nodes;
    link_t *links;
    rectangle_t *rectangles;
    text_t *textboxes;

    if (file != NULL)
    {
        settings->openProjectName = strdup(filename); // updating the project name
        free(filename);
        filename = NULL;

        fscanf(file, "%d\n%d\n%d\n%d\n", &numnodes, &numlinks, &numrectangles, &numtextboxes);

        nodes = (node_t *)malloc(numnodes * sizeof(node_t));
        links = (link_t *)malloc(numlinks * sizeof(link_t));
        rectangles = (rectangle_t *)malloc(numrectangles * sizeof(rectangle_t));
        textboxes = (text_t *)malloc(numtextboxes * sizeof(text_t));

        char name[50]; // buffer
        char othername[200];
        int type, type2;

        for (int i = 0; i < numnodes; i++)
        { // reading all nodes

            fscanf(file, "%s\n%d\n%d %d\n", name, &type, &nodes[i].x, &nodes[i].y);
            nodes[i].type = type;
            // printf("%s\n", name);
            nodes[i].name = (char *)malloc((strlen(name) + 1) * sizeof(char)); // allocating memory for node name
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

        for (int i = 0; i < numrectangles; i++)
        { // reading all rectangles
            fscanf(file, "%d %d\n%d %d\n%d %d %d\n", &rectangles[i].x, &rectangles[i].y, &rectangles[i].x1, &rectangles[i].y1, (int *)&rectangles[i].r, (int *)&rectangles[i].g, (int *)&rectangles[i].b);
        }

        for (int i = 0; i < numtextboxes; i++)
        {
            // reading all textboxes
            fgets(othername, 200, file);
            othername[strlen(othername) - 1] = '\0';
            fscanf(file, "%d %d\n", &textboxes[i].x, &textboxes[i].y);
            textboxes[i].text = (char *)malloc(strlen(othername) * sizeof(char));
            strcpy(textboxes[i].text, othername);
        }

        // updating settings to hold the collected data

        getWriteLock(settings);
        interface_t *gui = settings->GUIdata;
        gui->nodes = nodes;
        gui->links = links;
        gui->rectangles = rectangles;
        gui->texts = textboxes;
        settings->numnodes = numnodes;
        settings->numlink = numlinks;
        settings->numrectangles = numrectangles;
        settings->numTexts = numtextboxes;
        settings->absoluteCount = numnodes;
        releaseWriteLock(settings);
        logSuccess("Loaded project", "");
    }
    else
    {
        settings->openProjectName = NULL; // updating the project name, since there has been an error while opening it
        logWarning("There was an error while opening the file.", "Perhaps the path is wrong?");
    }
}

// the user requested to specify the name of the project to save
void prepareToSaveProject(settings_t *settings)
{
    settings->resetName = 1;
    settings->gettingName = 2;
}

// actually write topology to file
void saveProject(settings_t *settings)
{
    if (settings->isSimulating)
    { // if an experiment was running we need to kill the containers
        stop(settings);
    }

    if (settings->openProjectName)
    {
        free(settings->openProjectName); // freeing the old project name
        settings->openProjectName = NULL;
    }

    char *filename = (char *)calloc(MAX_FILENAME, sizeof(char));
    snprintf(filename, MAX_FILENAME, "./saves/%s.done", settings->filename);
    FILE *file = fopen(filename, "w");

    if (file != NULL)
    {
        settings->openProjectName = filename;

        // creating the project file
        fprintf(file, "%d\n%d\n%d\n%d\n", settings->numnodes, settings->numlink, settings->numrectangles, settings->numTexts); // saving node number and link number + rectangle number at the top of the file

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

        for (int i = 0; i < settings->numrectangles; i++)
        { // saving every rectangle
            rectangle_t current_rect = gui->rectangles[i];
            fprintf(file, "%d %d\n%d %d\n%d %d %d\n", current_rect.x, current_rect.y, current_rect.x1, current_rect.y1, current_rect.r, current_rect.g, current_rect.b); // data for every link
        }

        for (int i = 0; i < settings->numTexts; i++)
        {
            // saving every textbox
            text_t current_text = gui->texts[i];
            fprintf(file, "%s\n%d %d\n", current_text.text, current_text.x, current_text.y);
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

            system("chmod 666 ./saves/*"); // permission fix

            fclose(file);

            logSuccess("File successfully saved.", "");
        }
        else
        {
            logWarning("There was an error while creating the config file.", "Perhaps the path is wrong?");
        }
        // free(filename); it crashes... why? i dont know
        // free(config_filename);
    }
    else
    {
        logWarning("There was an error while opening the file.", "Perhaps the path is wrong?");
    }
}

// the user requested to open the available docker networks dialog
void populateInterfaceOptionsWrapper(settings_t *settings)
{
    populateInterfaceOptions(settings);
}

// recording the docker network user choice
void trackChosenInterfBinding(settings_t *settings)
{
    __uint8_t found = 0;
    getWriteLock(settings);
    for (int i = 0; i < settings->numBindings && !found; i++) // looking if the interface setting was already specified, and if so, updating the binding interface name
    {
        if (!strcmp(settings->chosenNode, settings->interfaceBindings[i].deviceName))
        {
            strcpy(settings->interfaceBindings[i].bindingInterfaceName, settings->options[settings->chosenOption]);
            found = 1;
        }
    }

    if (!found)
    {
        settings->interfaceBindings = (binding_t *)realloc(settings->interfaceBindings, (settings->numBindings + 1) * sizeof(binding_t));
        settings->interfaceBindings[settings->numBindings].deviceName = (char *)malloc((strlen(settings->chosenNode) + 1) * sizeof(char));
        strcpy(settings->interfaceBindings[settings->numBindings].deviceName, settings->chosenNode);
        settings->interfaceBindings[settings->numBindings].bindingInterfaceName = (char *)malloc(strlen(settings->options[settings->chosenOption]) * sizeof(char));
        strcpy(settings->interfaceBindings[settings->numBindings].bindingInterfaceName, settings->options[settings->chosenOption]);
        settings->numBindings++;
    }

    settings->numOptions = 0;
    for (int i = 0; i < settings->numOptions; i++)
    {
        free(settings->options[i]);
    }
    free(settings->options);
    releaseWriteLock(settings);
}

int validateIP(settings_t *settings, char *providedIp)
{
    struct in_addr address;
    if (inet_pton(AF_INET, providedIp, &address))
    { // function to check if IP is valid IPv4 address
        // if valid IPv4 saves address into settings
        settings->serverIP = strdup(providedIp);
        return 1;
    }
    else
    {
        return 0;
    }
}

void getData(settings_t *settings, interface_t *interface)
{
    /*
        1. if i dont have the server addr ask for the ip to the user => settings
        2. get data (aka socket ecc)
        3. transform in structs/file
    */

    if (!settings->serverIP)
    {
        settings->gettingIp = 1;
        return;
    }
    fetchData(settings, interface);
}

void askToBecomeServer(settings_t *settings)
{
    if (!settings->isClient)
    {
        return;
    }
    switchFromClientToServer(settings);
}

void getWriteLock(settings_t *settings)
{
    if (settings->isServer == 1)
    {
        rwlock_acquire_writelock(settings->settingsLock);
    }
}

void releaseWriteLock(settings_t *settings)
{
    if (settings->isServer)
    {
        rwlock_release_writelock(settings->settingsLock);
    }
}

void getReadLock(settings_t *settings)
{
    rwlock_acquire_readlock(settings->settingsLock);
}

void releaseReadLock(settings_t *settings)
{
    rwlock_release_readlock(settings->settingsLock);
}