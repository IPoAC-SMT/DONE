#include "./lib/GUI.h"

#define WIDTH 1900
#define HEIGHT 1000
#define TITLE "Docker Orchestrator for Network Emulation"

/*
char*completePath(char*name){ // took this from SO, not my responsibility
    char *result = malloc(strlen("./resources/icons/") + strlen(name) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strncpy(result, "./resources/icons/",strlen("./resources/icons/"));
    strncat(result, name, strlen(name));
    return result;
}*/

int main(int argc, char** argv)
{
    if(argc>1 && !strcmp(argv[1],"--no-gui")) {
        int a = 0;
        if (((a = fork()) == 0)) {
            char * args[]={"konsole","-e","python3","-i","./cli/netlib.py",NULL};
            execvp(args[0],args);
        }
        else if(a>0) {
            printf("CLI running in other window\n");
            waitpid(a,NULL,0);
            return 0;
        }
        else {
            printf("tried to spawn the CLI but failed. so sad :(\n");
            return -1;
        }
    }

    initEnvironment(); // set up signal handling, create netns dir if not present
    
    interface_t *interface = init_interface((button_t[NUMbuttons]){
        {24, 129, 100, 100, placehub,"place a hub",0,NULL,"hub"},
        {24, 234, 100, 100, placeswitch,"place a switch",0,NULL,"switch"},
        {24, 339, 100, 100, placerouter,"place a router",0,NULL,"router"},
        {24, 444, 100, 100, placehost,"place a host",0,NULL,"host"},
        {24, 549, 100, 100, placeexternalinterface,"place an external interface",0,NULL,"externalinterface"},
        {24, 654, 100, 100, placeexternalnattedinterface,"place an external NATted interface",0,NULL,"externalnattedinterface"},
        {24, 759, 100, 100, placelink,"link two nodes together",0,NULL,"link"},
        {129, 24, 100, 100, start,"start simulation!",0,NULL,"start"},       // first of the horizontal row
        {234, 24, 100, 100, stop,"end simulation",0,NULL,"stop"},        // second of the horizontal row
        {339, 24, 100, 100, openProject,"Open a file",0,NULL,"openProject"},
        {444, 24, 100, 100, saveProject,"Save project",0,NULL,"saveProject"},
        {549, 24, 100, 100, clearCanvas,"Clear Canvas",0,NULL,"clearCanvas"},
        {654, 24, 100, 100, deleteNode,"Delete a node or a link by clicking on it",0,NULL,"link"},
        //{759, 24, 100, 100, helloworld,"helloworld",0,NULL,"helloworld"},
        {864, 24, 100, 100, placeRectangle,"Draw a Rectangle",0,NULL,"rectangle"}},
    NULL,NULL,NULL);


    InitWindow(WIDTH, HEIGHT, TITLE);
    SetTargetFPS(30);
 
    SetExitKey(KEY_NULL);

    settings_t settings = {0,0, 0,"",0,0,0,0,"",0,0,(void*)interface,0,0,0,0,0,NULL,1,1,0};

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR); // colore di sfondo, per crearne altri si deve usare CLITERAL(Color){R,G,B,A}, numeri interi da 0 a 255

        DrawGUI(&settings, interface);

        EndDrawing();
    }

    return 0;
}
