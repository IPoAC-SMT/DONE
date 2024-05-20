#include "../lib/interface_t.h"
#include "../lib/log.h"
#include "../lib/functions.h"
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

    /*  
    DATA TO SEND:
    - absoluteCount
    - numnodes
    - numlink
    - numrectangles
    - numTexts
    - numBindings
    - openProjectName
    - ipoac
    - interfaceBindings
    - GUIdata   -> nodes, links, rectangles, texts
    - config file
    */

    char *bindings = calloc(4096, sizeof(char));
    char *nodes = calloc(4096, sizeof(char));
    char *links = calloc(4096, sizeof(char));
    char *rectangles = calloc(4096, sizeof(char));
    char *texts = calloc(8192, sizeof(char));

    //logWarning("I wanna cry","");

    getReadLock(settingsPtr);

    // parsing bindings

    for(int i = 0; i < settingsPtr->numBindings; i++){
        char tmp[4];
        sprintf(tmp,"%03d",(int)strlen(settingsPtr->interfaceBindings[i].deviceName));
        strcat(bindings,tmp);   // strlen of deviceName
        strncat(bindings,settingsPtr->interfaceBindings[i].deviceName,strlen(settingsPtr->interfaceBindings[i].deviceName));  // deviceName
        sprintf(tmp,"%03d",(int)strlen(settingsPtr->interfaceBindings[i].bindingInterfaceName));  
        strcat(bindings,tmp);   // strlen of bindingInterfaceName
        strncat(bindings,settingsPtr->interfaceBindings[i].bindingInterfaceName,strlen(settingsPtr->interfaceBindings[i].bindingInterfaceName));    // bindingInterfaceName
    }

    //logWarning("pls help","");

    // parsing nodes

    for(int i = 0; i < settingsPtr->numnodes; i++){
        char tmp[10];
        node_t currentNode = ((interface_t *)settingsPtr->GUIdata)->nodes[i];
        sprintf(tmp,"%03d",(int)strlen(currentNode.name));
        strcat(nodes,tmp);  // strlen of node name
        strncat(nodes,currentNode.name,(int)strlen(currentNode.name));   // node name
        sprintf(tmp,"%01d%04d%04d",currentNode.type,currentNode.x,currentNode.y);  
        strcat(nodes,tmp);  // node type, x and y
    }

    //logWarning("tonight i wont sleep","");

    // parsing links

    for(int i = 0; i < settingsPtr->numlink; i++){
        char tmp[10];
        link_t currentLink = ((interface_t *)settingsPtr->GUIdata)->links[i];
        sprintf(tmp,"%03d%03d",(int)strlen(currentLink.node1),(int)strlen(currentLink.node2));
        strcat(links,tmp);  // node1 name length and node2 name length
        strncat(links,currentLink.node1,(int)strlen(currentLink.node1));     // node 1 name
        strncat(links,currentLink.node2,(int)strlen(currentLink.node2));     // node 2 name
        sprintf(tmp,"%01d%01d",currentLink.node1_type,currentLink.node2_type); 
        strcat(links,tmp);  // node1 type and node2 type
    }

    //logWarning("im gonna have nightmares","");

    // parsing rectangles 

    for(int i = 0; i < settingsPtr->numrectangles; i++){
        char tmp[26];
        rectangle_t currentRectangle = ((interface_t *)settingsPtr->GUIdata)->rectangles[i];
        sprintf(tmp,"%04d%04d%04d%04d",currentRectangle.x,currentRectangle.y,currentRectangle.x1,currentRectangle.y1);
        //logInfo("first","%s",tmp);
        strcat(rectangles,tmp);     // rectangle x, y, x1, y1
        sprintf(tmp,"%03u%03u%03u",(unsigned int)currentRectangle.r,(unsigned int)currentRectangle.g,(unsigned int)currentRectangle.b);
        logInfo("rgb:","%d %d %d",(int)currentRectangle.r,(int)currentRectangle.g,(int)currentRectangle.b);
        //logInfo("second","%s",tmp);
        strcat(rectangles,tmp);     // rectangle r, g, b
    }

    //logInfo("rectangles", "%s", rectangles);
    //logWarning("pls let me finish coding before 3 am","");

    // parsing texts

    for(int i = 0; i < settingsPtr->numTexts; i++){
        char tmp[12];
        text_t currentText = ((interface_t *)settingsPtr->GUIdata)->texts[i];
        sprintf(tmp,"%04d%04d%03d",currentText.x,currentText.y,(int)strlen(currentText.text));
        strcat(texts,tmp);  // text x, y, strlen of text
        strncat(texts,currentText.text,(int)strlen(currentText.text));   // text
    }

    logInfo("texts", "%s", texts);
    //logWarning("my soul left my body, see ya (in hell)!","");

    // constructing string to send, pls kill us :')

    char *stringToSend = calloc(65536,sizeof(char));    // %03d prints 3 digits from int and pads them with zeros if needed
    snprintf(stringToSend, 65536,
    "%03d%03d%03d%03d%03d%03d%03d%s%01d%s%s%s%s%s",
    settingsPtr->absoluteCount,
    settingsPtr->numnodes,
    settingsPtr->numlink,
    settingsPtr->numrectangles,
    settingsPtr->numTexts,
    settingsPtr->numBindings,
    settingsPtr->openProjectName ? (int)strlen(settingsPtr->openProjectName) : 0,
    settingsPtr->openProjectName ? settingsPtr->openProjectName : "",
    settingsPtr->ipoac != 0 ? 1 : 0,    // converted into 0 or 1 depending on value  
    bindings,
    nodes,
    links,
    rectangles,
    texts
    );
    

    //logWarning("did it really work?","");

    releaseReadLock(settingsPtr);

    logSuccess("We made it!","");

    
    return stringToSend;
}

void sendData(int sock){
    char *data = serialize();
    write(sock, data, strlen(data));
    return; 
}

void *connection_handler(void *socket_desc) {
    int sock = *(int*)socket_desc;
    char message[2048];
    int read_size;

    if ((read_size = recv(sock, message, 2048, 0)) > 0) {
        switch(message[0]){
            case '0': // TODO respond with data

                // CALL FUNCTION TO SEND DATA
                write(sock,"you pressed 0\n",strlen("you pressed 0\n"));
                sendData(sock);
                break;
            case '1': // TODO stop server and send ack
                // STOP BEING A SERVER
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


