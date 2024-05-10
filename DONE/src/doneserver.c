#include "../lib/interface_t.h"
#include "../lib/log.h"
#include <sys/socket.h>     // Include socket-related functions
#include <netinet/in.h>     // Include internet address structures
#include <arpa/inet.h>      // Include functions for manipulating IP addresses

#define PORT_NUMBER 4242


void fetchData(settings_t*settings,interface_t*interface){      // CLIENT CODE

    // TO TEST CONNECTION: nc -l -p 4242
    
    // get data from socket
    //printf("I should get data from sockets\n");

    int socketFd = socket(AF_INET, SOCK_STREAM, 0);  // Create a socket using IPv4 and TCP protocol
    if(socketFd < 0){
        logError("Unable to create socket","");
        return;
    }

    struct sockaddr_in socketAddress;

    socketAddress.sin_family = AF_INET;                      // Set address family to IPv4
    //socketAddress.sin_addr.s_addr = inet_addr(settings->serverIP); // Set the IP address to bind to
    inet_pton(AF_INET, settings->serverIP, &socketAddress.sin_addr);    // converting address to binary form
    socketAddress.sin_port = htons(PORT_NUMBER);              // Set the port number to bind 

    //printf("server at %s\n", settings->serverIP);
    //printf("IP address: %lx\n", socketAddress.sin_addr);
    //printf("PORT: %lx\n", socketAddress.sin_port);

    // connecting to the socket!
    if(connect(socketFd, (struct sockaddr *)&socketAddress, sizeof(socketAddress)) < 0){
        logError("Error connecting to socket", "");
        return;
    }

    logSuccess("Successfully connected to server at", "%s", settings->serverIP);

    // update structs
    return;
}


/*
    - accesso concorrenziali alla struttura dati
    
    QUANDO DIVENTA SERVER:
        ATTIVITÀ ONE-TIME:
            - apre socket
        ATTIVITÀ PERIODICHE:
            - ascolta richieste (potenzialmente multiple in contemporanea -> thread per rispondere)
            - serve richieste:
                - formatta dati
                - impacchetta dati
                - invia dati al client
    FINE RUOLO SERVER:
    - chiude ascolto
*/

