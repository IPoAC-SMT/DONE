#include "../lib/logicalController.h"
#include "../lib/log.h"
#include <string.h>

#define MAX_FILENAME 50
/*
void helloworld(settings_t *settings)
{
    printf("%d\n", settings->numlink);
}*/

void exportDoneScript(settings_t *settings){
    settings->exportDoneScript = 1;
}

void placeInternet(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    settings->placing_link = 0;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 5;
    settings->deletingNodes = 0;
    settings->placing_text = 0;
    logInfo("Ready to place a node", "type the Internet");
}
void placeswitch(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    settings->placing_link = 0;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 0;
    settings->deletingNodes = 0;
    settings->placing_text = 0;
    logInfo("Ready to place a node", "type switch");
}

void placerouter(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    settings->placing_link = 0;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 1;
    settings->deletingNodes = 0;
    settings->placing_text = 0;
    logInfo("Ready to place a node", "type router");
}

void placehost(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    settings->placing_link = 0;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 2;
    settings->deletingNodes = 0;
    settings->placing_text = 0;
    logInfo("Ready to place a node", "type host");
}

void placeexternalinterface(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    settings->placing_link = 0;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 3;
    settings->deletingNodes = 0;
    settings->placing_text = 0;
    logInfo("Ready to place a node", "type external interface");
}

void deleteNode(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    settings->placing_link = 0;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 0;
    settings->node_type = 0;
    settings->deletingNodes = 1;
    settings->placing_text = 0;
    logInfo("Ready to delete a node", "");
}

void placeexternalnattedinterface(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    settings->placing_link = 0;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 1;
    settings->node_type = 4;
    settings->deletingNodes = 0;
    settings->placing_text = 0;
    logInfo("Ready to place a node", "type external natted interface");
}

void placelink(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 0;
    settings->placing_link = 1;
    settings->deletingNodes = 0;
    settings->placing_text = 0;
    logInfo("Ready to place a link", "");
}

void placeText(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    settings->drawing_rectangle = 0;
    settings->moving_node = 0;
    settings->placing_node = 0;
    settings->placing_link = 0;
    settings->deletingNodes = 0;
    settings->placing_text = -1;
    logInfo("Ready to place a text", "");
}
void placeRectangle(settings_t *settings)
{
    if (settings->isSimulating)
        return;
    settings->drawing_rectangle = -1;
    settings->moving_node = 0;
    settings->placing_node = 0;
    settings->placing_link = 0;
    settings->deletingNodes = 0;
    settings->placing_text = 0;
    logInfo("Ready to place a rectangle", "");
}
void initEnvironment()
{
    initEnv();
    logSuccess("Environment initialized successfully", "");
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
        logInfo("Open Project Name:", "%s", settings->openProjectName);
        strcat(config_filename, ".conf");
        logInfo("Config Filename:", "%s", config_filename);
        FILE *file = fopen(config_filename, "r");
        if (file != NULL)
        {
            char buf[200]; // buffer for reading the file
            char *nodeName;

            while (fgets(buf, 200, file)) // reading the node name line
            {
                nodeName = strtok(strdup(buf), ":"); // retrieving the node name from the config
                if (nodeName != NULL)
                {
                    do
                    {
                        if (!fgets(buf, 200, file))
                            break; // if i reached the end of the commands for this node, break and go to the next one
                        else if (buf[0] == '\n')
                            break; // if i reached the end of the commands for this node, break and go to the next one
                        if (nodeName[0] == 's')
                        {
                            logInfo("sending to switch:", "%s", buf);
                            if(sendSwitchCommand(buf)){
                                logError("An error was found in the config", "");
                            }
                        }
                        else
                        {
                            logInfo("sending command to node", "%s", buf); // sending the command to the logical controller
                            if(sendNodeCommand(nodeName, buf)){
                                logError("An error was found in the config", "");
                            }
                        }
                    } while (1);
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
}

void quit(settings_t *settings)
{
    if (settings->isSimulating)
        logInfo("be careful: it was simulating.", "Now we are stopping the simulation. You're welcome");
    forcedCLIExit();
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
    if (settings->numlink > 0 || settings->numnodes > 0 || settings->numrectangles > 0 || settings->numTexts > 0)
    {
        settings->numnodes = 0;
        settings->numlink = 0;
        settings->numrectangles = 0;
        settings->numTexts = 0;
        interface_t *interface = (interface_t *)settings->GUIdata;
        if (settings->numlink)
            free(interface->links);
        if (settings->numnodes)
            free(interface->nodes);
        if (settings->numrectangles)
            free(interface->rectangles);
        if(settings->numTexts)
            free(interface->texts);
    }
    logSuccess("Canvas cleared", "");
}

void prepareToOpenProject(settings_t *settings)
{
    settings->resetName = 1;
    settings->gettingName = 1;
    logInfo("Ready to read project name from GUI", "");
}

void openProject(settings_t *settings)
{
    if (settings->isSimulating)
    { // if an experiment was running we need to kill the containers
        stop(settings);
    }

    clearCanvas(settings); // first, clearing the canvas

    if (settings->openProjectName)
    {
        free(settings->openProjectName); // freeing the old project name
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
        settings->openProjectName = filename; // updating the project name

        fscanf(file, "%d\n%d\n%d\n%d\n", &numnodes, &numlinks, &numrectangles, &numtextboxes);

        nodes = (node_t *)malloc(numnodes * sizeof(node_t));
        links = (link_t *)malloc(numlinks * sizeof(link_t));
        rectangles = (rectangle_t *)malloc(numrectangles * sizeof(rectangle_t));
        textboxes = (text_t *)malloc(numtextboxes * sizeof(text_t));

        char name[50]; // buffer
        char othername[200];
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

        for (int i = 0; i < numrectangles; i++)
        { // reading all rectangles
            fscanf(file, "%d %d\n%d %d\n%d %d %d\n", &rectangles[i].x, &rectangles[i].y, &rectangles[i].x1, &rectangles[i].y1, (int *)&rectangles[i].r, (int *)&rectangles[i].g, (int *)&rectangles[i].b);
        }

        for(int i = 0; i < numtextboxes; i++){
            // reading all textboxes
            fgets(othername,200,file);
            fscanf(file, "%d %d\n", &textboxes[i].x, &textboxes[i].y);
            printf("read: %s at %d %d\n", othername, textboxes[i].x, textboxes[i].y);
            textboxes[i].text = (char *)malloc(strlen(othername) * sizeof(char));
            strcpy(textboxes[i].text, othername);
            printf("now textboxes is %s\n", textboxes[i].text);
        }

        // updating settings to hold the collected data

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
        logSuccess("Loaded project", "");
    }
    else
    {
        logWarning("There was an error while opening the file.", "Perhaps the path is wrong?");
    }
}

void prepareToSaveProject(settings_t *settings)
{
    settings->resetName = 1;
    settings->gettingName = 2;
}

void saveProject(settings_t *settings)
{
    if (settings->isSimulating)
    { // if an experiment was running we need to kill the containers
        stop(settings);
    }

    if (settings->openProjectName)
    {
        // free(settings->openProjectName); // freeing the old project name
    }

    char *filename = (char *)calloc(MAX_FILENAME, sizeof(char));
    snprintf(filename, MAX_FILENAME, "./saves/%s.done", settings->filename);
    FILE *file = fopen(filename, "w");

    if (file != NULL)
    {
        settings->openProjectName = filename;

        // creating the project file
        fprintf(file, "%d\n%d\n%d\n%d\n", settings->numnodes, settings->numlink, settings->numrectangles, settings->numTexts); // saving node number and link number + rectangle number at the top of the file
        //fprintf(file, "%d\n%d\n%d\n", settings->numnodes, settings->numlink, settings->numrectangles); // saving node number and link number + rectangle number at the top of the file


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

        
        for(int i = 0; i < settings->numTexts; i++){
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

            system("chmod 666 ./saves/*"); // temporary fix, i'd like to cry (Access control skill issues)

            fclose(file);

            logSuccess("File successfully saved.","");
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


void populateInterfaceOptionsWrapper(settings_t *settings){
    populateInterfaceOptions(settings);
}
