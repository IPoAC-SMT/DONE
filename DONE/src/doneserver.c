#include "../lib/interface_t.h"
#include "../lib/log.h"
#include <string.h>
#include <sys/socket.h>     // Include socket-related functions
#include <netinet/in.h>     // Include internet address structures
#include <arpa/inet.h>      // Include functions for manipulating IP addresses
#include <unistd.h>
#include <pthread.h>

#define PORT_NUMBER 4242

settings_t * settingsPtr = NULL;

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

    send(socketFd, "0",1,0);



    // TODO update structs
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

char* serialize(void){
    // TODO
    return NULL;
}

void *connection_handler(void *socket_desc) {
    int sock = *(int*)socket_desc;
    char message[2048];
    int read_size;

    if ((read_size = recv(sock, message, 2048, 0)) > 0) {
        switch(message[0]){
            case '0': // TODO respond with data
                write(sock,"you pressed 0\n",strlen("you pressed 0\n"));
                break;
            case '1': // TODO stop server and send ack
                write(sock,"you pressed 1\n",strlen("you pressed 1\n"));
                break;
            default: // what are you saying?
                write(sock,message,strlen(message));
        }
    }
    else if(read_size == 0) {
        logInfo("Client disconnected","");
    }
    else if(read_size == -1) {
        logError("recv failed","");
    }

    free(socket_desc);
    return 0;
}


void* serverFunction(void* settings_void){

    settingsPtr = (settings_t*)settings_void;

    // ORA CREO LA SOCKET
    logInfo("Ready to start server","");

    int socketFd = socket(AF_INET, SOCK_STREAM, 0);  // Create a socket using IPv4 and TCP protocol
    if(socketFd < 0){
        logError("Unable to create socket","");
        return NULL;
    }

    struct sockaddr_in socketAddress;

    socketAddress.sin_family = AF_INET;                      // Set address family to IPv4
    //socketAddress.sin_addr.s_addr = inet_addr(settings->serverIP); // Set the IP address to bind to
    socketAddress.sin_addr.s_addr = INADDR_ANY;
    socketAddress.sin_port = htons(PORT_NUMBER);              // Set the port number to bind 

    if(bind(socketFd,(struct sockaddr*)&socketAddress,sizeof(socketAddress)) <0) {
        logError("bind failed","");
        return NULL;
    }
    
    logSuccess("binding successful","");

    listen(socketFd,5);

    logInfo("waiting for incoming connections...","");

    int c = sizeof(struct sockaddr_in);
    int client_sock,*new_sock;
    struct sockaddr_in client;

    while((client_sock = accept(socketFd, (struct sockaddr*)&client, (socklen_t*)&c))) {
        logInfo("connection accepted","");

        pthread_t answer_thread;
        new_sock = calloc(1,sizeof(int));
        *new_sock = client_sock;

        if(pthread_create(&answer_thread,NULL,connection_handler, (void*)new_sock)<0 ) {
            logError("could not create thread","");
            return NULL;
        }
        logSuccess("handler assigned","");

    }

    if (client_sock < 0) {
        logError("accept failed","");
        return NULL;
    }

    return NULL;
}


