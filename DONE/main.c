#include "./lib/GUI.h"
#include "./lib/log.h"

#define WIDTH 1900
#define HEIGHT 1000
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
            logInfo("CLI running in other window","");
            waitpid(a, NULL, 0);
            return 0;
        }
        else
        {
            logError("tried to spawn the CLI but failed.","so sad :(");
            return -1;
        }
    }

    bool c = (argc > 1) && (!strcmp(argv[1], "--ipoac"));

    initEnvironment(); // set up signal handling, create netns dir if not present

    interface_t *interface = init_interface((button_t[NUMbuttons]){
        {24, 214, 50, 50, placeswitch, "Place a switch", 0, NULL, "switch"},
        {24, 274, 50, 50, placerouter, c ? "Place an RFC1149-compatible \"router\"" : "Place a router", 0, NULL, c ? "IPoAC" : "router"},
        {24, 334, 50, 50, placehost, "Place a host", 0, NULL, "host"},
        {24, 394, 50, 50, placeexternalinterface, "Place an external interface", 0, NULL, "externalinterface"},
        {24, 454, 50, 50, placeexternalnattedinterface, "Place an external NATted interface", 0, NULL, "externalnattedinterface"},
        {24, 154, 50, 50, placelink, "Link two nodes together", 0, NULL, "link"},
        {144, 24, 50, 50, start, "Start simulation!", 0, NULL, "start"}, // first of the horizontal row
        {204, 24, 50, 50, stop, "Stop simulation", 0, NULL, "stop"},     // second of the horizontal row
        {24, 24, 50, 50, prepareToOpenProject, "Open a file", 0, NULL, "openProject"},
        {84, 24, 50, 50, prepareToSaveProject, "Save project", 0, NULL, "saveProject"},
        {549, 24, 50, 50, clearCanvas, "Clear Canvas", 0, NULL, "clearCanvas"},
        {334, 24, 50, 50, deleteNode, "Delete a node or a link by clicking on it", 0, NULL, "deleteNode"},
        {874, 24, 50, 50, quit, "Quit", 0, NULL, "quit"},
        {24, 544, 50, 50, placeRectangle, "Draw a Rectangle", 0, NULL, "rectangle"}},
    NULL, NULL, NULL);

    InitWindow(WIDTH, HEIGHT, TITLE);
    SetTargetFPS(30);

    // Load icon image
    Image icon = LoadImage("./resources/icon.png");
    // Set icon
    SetWindowIcon(icon);

      // Unload image
    UnloadImage(icon);


    SetExitKey(KEY_NULL);

    settings_t settings = {0, 0, 0, "", 0, 0, 0, 0, "", 0, 0, (void *)interface, 0, 0, 0, 0, 0, NULL, 0, 0, 0, c, 0};

    system("clear");

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR); // colore di sfondo, per crearne altri si deve usare CLITERAL(Color){R,G,B,A}, numeri interi da 0 a 255

        DrawGUI(&settings, interface);

        EndDrawing();
    }

    return 0;
}
