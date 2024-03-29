#include "./lib/GUI.h"

#define WIDTH 1900
#define HEIGHT 1000
#define TITLE "Docker Orchestrator for Network Emulation"

#include "./lib/netlib.h" // temp

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
            printf("tried to spawn the CLI but failed. so sad.\n");
            return -1;
        }
    }

    interface_t *interface = init_interface((button_t[NUMbuttons]){
        {24, 129, 100, 100, router_t, placehub},
        {24, 234, 100, 100, router_t, placeswitch},
        {24, 339, 100, 100, router_t, placerouter},
        {24, 444, 100, 100, router_t, placehost},
        {24, 549, 100, 100, router_t, placeexternalinterface},
        {24, 654, 100, 100, router_t, placeexternalnattedinterface},
        {24, 759, 100, 100, router_t, placelink},
        {24, 864, 100, 100, router_t, helloworld},
        {129, 24, 100, 100, router_t, start},       // first of the horizontal row
        {234, 24, 100, 100, router_t, stop},        // second of the horizontal row
        {339, 24, 100, 100, router_t, helloworld},
        {444, 24, 100, 100, router_t, helloworld},
        {549, 24, 100, 100, router_t, helloworld},
        {654, 24, 100, 100, router_t, helloworld},
        {759, 24, 100, 100, router_t, helloworld},
        {864, 24, 100, 100, router_t, helloworld}},
    NULL,NULL);


    InitWindow(WIDTH, HEIGHT, TITLE);
    SetTargetFPS(30);
 
    SetExitKey(KEY_NULL);

    //setSignalHandling(); // set up signal handling

    settings_t settings = {0, 0, 0, "",0,0,0,0,"",0,0,(void*)interface,0};


    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR); // colore di sfondo, per crearne altri si deve usare CLITERAL(Color){R,G,B,A}, numeri interi da 0 a 255

        DrawGUI(&settings, interface);

        EndDrawing();
    }

    return 0;
}
