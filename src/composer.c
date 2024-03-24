// Docker orchestration commands library

#include "./headers/composer.h"

// This functions exclusively include the interactions with the Linux system and docker API
// All the logical controls about already existing nodes, switches and cables must be added outside of this library

// Create a new node
// A node can be a host (h) or a router (r)
int newNode(char *name, char type)
{

    char command[MAX_COMMAND_SIZE];

    if (name != NULL && strlen(name) <= MAX_NAME_SIZE && (type == 'h' || type == 'r'))
    {

        strcpy(command, "docker run -id --name ");
        strcat(command, name);
        if (type == 'h')
            strcat(command, " --network none ");
        else
            strcat(command, " --network none ");
        strcat(command, "node:1.0.0 /bin/ash");

        return system(command);
    }
    else
        return -1;
}