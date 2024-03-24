#include "./lib/GUI.h"

#define WIDTH 1900
#define HEIGHT 1000
#define TITLE "Docker Orchestrator for Networks Emulation"
#define STD_FONT_SIZE 19

/*
char*completePath(char*name){ // took this from SO, not my responsibility
    char *result = malloc(strlen("./resources/icons/") + strlen(name) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strncpy(result, "./resources/icons/",strlen("./resources/icons/"));
    strncat(result, name, strlen(name));
    return result;
}*/

int main(int argc, const char* argv[]){

    interface_t * interface = init_interface((pulsante_t[NUMPULSANTI]){
        {24,129,100,100,"router.png",helloworld},
        {24,234,100,100,"router.png",helloworld},
        {24,339,100,100,"router.png",helloworld},
        {24,444,100,100,"router.png",helloworld},
        {24,549,100,100,"router.png",helloworld},
        {24,654,100,100,"router.png",helloworld},
        {24,759,100,100,"router.png",helloworld},
        {24,864,100,100,"router.png",helloworld},
        {129,24,100,100,"router.png",helloworld},
        {234,24,100,100,"router.png",helloworld},
        {339,24,100,100,"router.png",helloworld},
        {444,24,100,100,"router.png",helloworld},
        {549,24,100,100,"router.png",helloworld},
        {654,24,100,100,"router.png",helloworld},
        {759,24,100,100,"router.png",helloworld},
        {864,24,100,100,"router.png",helloworld}
    },NULL,NULL);


	InitWindow(WIDTH, HEIGHT, TITLE);
    SetTargetFPS(30);

    settings_t settings = {1};

	while (!WindowShouldClose()) {
		BeginDrawing();
	
		ClearBackground(RAYWHITE); //colore di sfondo, per crearne altri si deve usare CLITERAL(Color){R,G,B,A}, numeri interi da 0 a 255
        
        DrawGUI(&settings,interface);

		EndDrawing();
	}

	return 0;
}