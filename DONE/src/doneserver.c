#include "../lib/interface_t.h"
#include "../lib/log.h"
#include "../lib/functions.h"
#include <string.h>
#include <sys/socket.h>     // Include socket-related functions
#include <netinet/in.h>     // Include internet address structures
#include <arpa/inet.h>      // Include functions for manipulating IP addresses
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>

#define PORT_NUMBER 4242

settings_t * settingsPtr = NULL;

char *getLocalIP(){
    char *hostbuffer = (char *)calloc(256, sizeof(char));
    char *IPbuffer;
    struct hostent *host_entry;
 
    // To retrieve host information
    host_entry = gethostbyname(hostbuffer);
 
    // To convert an Internet network address into ASCII string
    IPbuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));

    free(hostbuffer);

    return IPbuffer;
}

void switchFromClientToServer(settings_t*settings){

    int socketFd = socket(AF_INET, SOCK_STREAM, 0);  // Create a socket using IPv4 and TCP protocol
    if(socketFd < 0){
        logError("Unable to create socket","");
        return;
    }

    struct sockaddr_in socketAddress;

    socketAddress.sin_family = AF_INET;                      // Set address family to IPv4
    inet_pton(AF_INET, settings->serverIP, &socketAddress.sin_addr);    // converting address to binary form
    socketAddress.sin_port = htons(PORT_NUMBER);              // Set the port number to bind 

    if(connect(socketFd, (struct sockaddr *)&socketAddress, sizeof(socketAddress)) < 0){
        logError("Error connecting to socket", "");
        return;
    }

    logSuccess("Successfully connected to server at", "%s", settings->serverIP);

    char tmp[17];
    snprintf(tmp,17,"1%s", getLocalIP());
    logInfo("new IP address of server:","%s",tmp+1);

    send(socketFd,tmp,strlen(tmp),0);

    logInfo("Switch to server mode requested","");

    // read data into char * data
    char * data = (char*)calloc(17,sizeof(char));

    read(socketFd,data,17);

    int length = 0;

    sscanf(data,"%01d", &length);
    data += 1;  // move pointer to the next byte

    if(length){     // the server we contacted is not an updated server anymore
        // parse ip address of next server
        settings->serverIP = strdup(data);
        logWarning("That server is not a server anymore, contacting", "%s", data);
        switchFromClientToServer(settings);     // retry contacting the provided ip
        return;
    } else {    // we can become server
        settings->isClient = 0;
        settings->serverIP = NULL;
        settings->nextServer = NULL;
        settings->isServer = 0;
        settings->hasToBeServer = 1;        // the main will take it from here
    }
     
    return;
}


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

    logInfo("data requested","");

    // read data into char * data
    char * data = (char*)calloc(65536,sizeof(char));

    read(socketFd,data,65536);
    /*logWarning("data","%s",data);*/


    /*logWarning("Allocation successful","");*/

    // parse data
    //getWriteLock(settings);

    /*logWarning("I'm crying too","");*/

    int length = 0, length1 = 0;

    sscanf(data,"%01d", &length);
    data += 1;  // move pointer to the next byte

    if(length){
        // parse ip address of next server
        settings->serverIP = strdup(data);
        return;
    }

    sscanf(data,"%03d%03d%03d%03d%03d%03d%03d",
        &settings->absoluteCount,
        &settings->numnodes,
        &settings->numlink,
        &settings->numrectangles,
        &settings->numTexts,
        &settings->numBindings,
        &length
    );
    
    /*logWarning("is the crash here?","%d %d %d %d %d %d %d",
            settings->absoluteCount,
            settings->numnodes,
            settings->numlink,
            settings->numrectangles,
            settings->numTexts,
            settings->numBindings,
            length
);*/

    data = data + 21; // mi sposto all'inizio del nome, se presente

    settings->openProjectName = strndup(data,length);
    sscanf(data+length,"%01c",&settings->ipoac);
    settings->ipoac-=48;
    data = data+length+1; // mi sposto oltre nome e flag (IPoAC)

    /*logWarning("no","");*/

    // ora devo leggere i bindings
    
    settings->interfaceBindings = (binding_t*)calloc(settings->numBindings,sizeof(binding_t));
    for(int i = 0; i<settings->numBindings;i++) {
        sscanf(data,"%03d",&length);
        data += 3;
        settings->interfaceBindings[i].deviceName = strndup(data,length);
        data += length;
        sscanf(data,"%03d",&length);
        data +=3;
        settings->interfaceBindings[i].bindingInterfaceName = strndup(data,length);
        data += length;
    }


    // leggere nodes
    interface->nodes = (node_t*) calloc(settings->numnodes,sizeof(node_t));
    for (int i = 0; i < settings->numnodes;i++) {
        sscanf(data,"%03d",&length);
        data += 3;
        interface->nodes[i].name = strndup(data,length);
        data += length;
        sscanf(data,"%01d%04d%04d",&length,&interface->nodes[i].x,&interface->nodes[i].y);
        interface->nodes[i].type = length;
        data += 9;
    }


    // leggere links
    interface->links = (link_t*) calloc(settings->numlink,sizeof(link_t));
    for (int i = 0; i<settings->numlink;i++) {
        sscanf(data, "%03d%03d",&length,&length1);
        data += 6;
        interface->links[i].node1 = strndup(data,length);
        data += length;
        interface->links[i].node2 = strndup(data,length1);
        data += length1;
        sscanf(data, "%01d%01d",&length,&length1);
        interface->links[i].node1_type = (component_type_t)length;
        interface->links[i].node2_type = (component_type_t)length1;
        data += 2;
    }


    // leggere rectangles
    
    interface->rectangles = (rectangle_t*) calloc(settings->numrectangles,sizeof(rectangle_t));
    for (int i = 0; i<settings->numrectangles; i++) {
        sscanf(data,"%04d%04d%04d%04d%03d%03d%03d",
                &interface->rectangles[i].x,
                &interface->rectangles[i].y,
                &interface->rectangles[i].x1,
                &interface->rectangles[i].y1,
                &interface->rectangles[i].r,
                &interface->rectangles[i].g,
                &interface->rectangles[i].b);
        data += 25;
        interface->rectangles[i].r = (interface->rectangles[i].r + 128) % 256;
        interface->rectangles[i].g = (interface->rectangles[i].g + 128) % 256;
        interface->rectangles[i].b = (interface->rectangles[i].b + 128) % 256;
    }


    // leggere texts

    interface->texts = (text_t*) calloc(settings->numTexts,sizeof(text_t));
    for (int i = 0; i < settings->numTexts; i++) {
        sscanf(data,"%04d%04d%03d",&interface->texts[i].x,&interface->texts[i].y,&length);
        data += 11;
        interface->texts[i].text = strndup(data,length);
        data += length;
    }
    


    // TODO update file
    
    //releaseWriteLock(settings);
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
        strcat(rectangles,tmp);     // rectangle x, y, x1, y1
        sprintf(tmp,"%03d%03d%03d",currentRectangle.r + 128,currentRectangle.g + 128, currentRectangle.b + 128);        // DON'T LIE TO THE COMPILER
        strcat(rectangles,tmp);     // rectangle r, g, b
    }

    //logWarning("pls let me finish coding before 3 am","");

    // parsing texts

    for(int i = 0; i < settingsPtr->numTexts; i++){
        char tmp[12];
        text_t currentText = ((interface_t *)settingsPtr->GUIdata)->texts[i];
        sprintf(tmp,"%04d%04d%03d",currentText.x,currentText.y,(int)strlen(currentText.text));
        strcat(texts,tmp);  // text x, y, strlen of text
        strncat(texts,currentText.text,(int)strlen(currentText.text));   // text
    }

    //logInfo("texts", "%s", texts);
    //logWarning("my soul left my body, see ya (in hell)!","");

    // constructing string to send, pls kill us :')

    char *stringToSend = calloc(65536,sizeof(char));    // %03d prints 3 digits from int and pads them with zeros if needed
    snprintf(stringToSend, 65536,
    "0%03d%03d%03d%03d%03d%03d%03d%s%01d%s%s%s%s%s",
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

    logSuccess("Successfully sent data","");
    
    return stringToSend;
}

void sendData(int sock){
    char *data = serialize();
    write(sock, data, strlen(data));
    return; 
}

char *parseServerSwitchRequest(char *message){  // if client receives a 0, switch is successful, else it receives the new server ip
    // we have received the IP address of the new server
    message += 1;
    if(!settingsPtr->nextServer){       // the client can become server only if there is no other next server
        settingsPtr->nextServer = strdup(message);
        return "0";
    }

    char *tmp = (char *)calloc(17, sizeof(char));
    snprintf(tmp, 16, "1%s", settingsPtr->nextServer);  // if we have received a server switch request but we have already passed control to someone else
    return tmp;
}

void *connection_handler(void *socket_desc) {
    int sock = *(int*)socket_desc;
    char message[2048];
    int read_size;

    if ((read_size = recv(sock, message, 2048, 0)) > 0) {
        switch(message[0]){
            case '0': // respond with data
                if(!settingsPtr->nextServer){
                    logInfo("I am the server, I will grace thee with my beloved bits.","");
                    sendData(sock);
                } else {    // they are not the server anymore, so they communicate the new IP address of the server
                    char tmp[17];
                    snprintf(tmp, 16, "1%s", settingsPtr->nextServer);
                    write(sock,tmp,strlen(tmp));
                }
                break;
            case '1': // stop server and send ack
                logInfo("how dare thee try to overthrow my reign?","");
                char *response = parseServerSwitchRequest(message);
                write(sock,response,strlen(response));
                logInfo("response:","%s",response);
                break;
            default: // what are you trying to do? go away
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