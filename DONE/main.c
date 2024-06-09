#include "./lib/GUI.h"
#include "./lib/log.h"
#include <unistd.h> 
#include <pthread.h> 

#define TITLE "Docker Orchestrator for Network Emulation"

int main(int argc, char **argv)
{
    if (argc > 1 && !strcmp(argv[1], "--no-gui"))
    {
        int a = 0;
        if (((a = fork()) == 0))
        {
            char *args[] = {"x-terminal-emulator", "-e", "python3", "-i", "./cli/netlib.py", NULL};
            execvp(args[0], args);
        }
        else if (a > 0)
        {
            logInfo("CLI running in other window", "");
            waitpid(a, NULL, 0);
            return 0;
        }
        else
        {
            logError("tried to spawn the CLI but failed.", "so sad :(");
            return -1;
        }
    }

    bool c = (argc > 1) && (!strcmp(argv[1], "--ipoac"));

    initEnvironment(); // set up signal handling, create netns dir if not present

    interface_t *interface = init_interface((button_t[NUMbuttons]){
        {24, 214, 50, 50, placeswitch, "Place a switch", 0, NULL, "switch", false},
        {24, 274, 50, 50, placerouter, c ? "Place an RFC1149-compatible \"router\"" : "Place a router", 0, NULL, c ? "IPoAC" : "router", false},
        {24, 334, 50, 50, placehost, "Place a host", 0, NULL, "host", false},
        {24, 394, 50, 50, placeexternalinterface, "Place an external interface", 0, NULL, "externalinterface", false},
        {24, 454, 50, 50, placeexternalnattedinterface, "Place an external NATted interface", 0, NULL, "externalnattedinterface", false},
        {24, 154, 50, 50, placelink, "Link two nodes together", 0, NULL, "link", false},
        {144, 24, 50, 50, start, "Start simulation!", 0, NULL, "start", false}, // first of the horizontal row
        {204, 24, 50, 50, stop, "Stop simulation", 0, NULL, "stop", true},     // second of the horizontal row
        {84, 24, 50, 50, prepareToOpenProject, "Open a file", 0, NULL, "openProject", false},
        {24, 24, 50, 50, prepareToSaveProject, "Save project", 0, NULL, "saveProject", false},
        {400, 24, 50, 50, exportDoneScript, "Export DoneScript", 0, NULL, "saveProject", false},
        {24, 634, 50, 50, placeText, "Place a Text", 0, NULL, "placeText", false},
        {549, 24, 50, 50, clearCanvas, "Clear Canvas", 0, NULL, "clearCanvas", false},
        {334, 24, 50, 50, deleteNode, "Delete a node or a link by clicking on it", 0, NULL, "deleteNode", false},
        {1634, 24, 50, 50, quit, "Quit", 0, NULL, "quit", true},
        {24, 574, 50, 50, placeRectangle, "Draw a Rectangle", 0, NULL, "rectangle", false},
        {1634, 154, 50, 50, becomeClient, "Start or stop client!", 0, NULL, "startstopclient", true},
        {1634, 274, 50, 50, becomeServer, "Start or stop server!", 0, NULL, "startstopserver", true},
        {1634, 400, 50, 50, askToBecomeServer, "Become the server!", 0, NULL, "becomeserver", true},
        },
    NULL, NULL, NULL, NULL);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE); // Window configuration flags
    int screenWidth = WIDTH, screenHeight = HEIGHT;
    InitWindow(screenWidth /*WIDTH*/, screenHeight /*HEIGHT*/, TITLE);
    SetTargetFPS(30);

    // Load icon image
    Image icon = LoadImage("./resources/icon.png");
    // Set icon
    SetWindowIcon(icon);

    // Unload image
    UnloadImage(icon);

    SetExitKey(KEY_NULL);

    settings_t settings = {NULL,0,0,0,"",0,0,0,0,"","",0,(void*)interface,0,0,0,0,NULL,NULL,0,0,0,c,0,0,0,NULL,NULL,NULL,0,0,0,0,0,NULL,0,0,NULL,0,NULL,0,0,0,NULL,0};
    settings.settingsLock = (rwlock_t*)calloc(1,sizeof(rwlock_t));
    rwlock_init(settings.settingsLock);

    system("clear");

    int framecount = 0;

    pthread_t thread_id;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR); // colore di sfondo, per crearne altri si deve usare CLITERAL(Color){R,G,B,A}, numeri interi da 0 a 255
        //printf("%d\t%d\n",time(NULL),!(time(NULL)%5));
        if(settings.isClient && !((framecount++)%30)) getData(&settings,interface);

        DrawGUI(&settings, interface);

        if(settings.hasToBeServer && !settings.isServer) {
            settings.isServer = 42;
            pthread_create(&thread_id,NULL,actAsServer,(void*)&settings);
        }


        EndDrawing();
    }

    if (settings.openProjectName)
        free(settings.openProjectName);

    return 0;
}
