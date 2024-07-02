#include "../lib/interface_t.h"
#include "../lib/log.h"
#include "../lib/functions.h"
#include <string.h>
#include <sys/socket.h> // Include socket-related functions
#include <netinet/in.h> // Include internet address structures
#include <arpa/inet.h>  // Include functions for manipulating IP addresses
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <net/if.h>  // Include network interface definitions
#include <ifaddrs.h> // Include interface address structure definitions

#define PORT_NUMBER 4242

typedef struct netInterface_t
{
    char *interfaceName;
    char *IP;
    struct netInterface_t *next;
} netInterface_t;

settings_t *settingsPtr = NULL;

int startsWith(const char *prefix, const char *string)
{
    return (strncmp(prefix, string, strlen(prefix)) == 0) ? 1 : 0;
}

int startsWithInterestingInterfaces(const char *str)
{
    return (
               strncmp("eth", str, strlen("eth")) == 0 ||
               strncmp("enp", str, strlen("enp")) == 0 ||
               strncmp("wl", str, strlen("wl")) == 0)
               ? 1
               : 0; // returns 1 if at least one or is satisfied, else 0
}

int connectToSocket(char *serverIp)
{
    int socketFd = socket(AF_INET, SOCK_STREAM, 0); // Create a socket using IPv4 and TCP protocol
    if (socketFd < 0)
    {
        logError("Unable to create socket", "");
        return -1;
    }

    struct sockaddr_in socketAddress;

    socketAddress.sin_family = AF_INET;                    // Set address family to IPv4
    inet_pton(AF_INET, serverIp, &socketAddress.sin_addr); // converting address to binary form
    socketAddress.sin_port = htons(PORT_NUMBER);           // Set the port number to bind

    if (connect(socketFd, (struct sockaddr *)&socketAddress, sizeof(socketAddress)) < 0)
    {
        logError("Error connecting to socket", "");
        return -1;
    }

    return socketFd;
}

char *extractPureName(char *stringa)
{
    int i;
    char *tmp;
    for (i = strlen(stringa) - 1; i > 0; i--)
    {
        if (stringa[i] == '/')
        {
            tmp = strdup(stringa + i);
            break;
        }
    }
    tmp[strlen(tmp) - 5] = '\0';
    return tmp;
}

void synchFile(settings_t *settings)
{
    // request for the file
    int socketFd = connectToSocket(settings->serverIP);

    if (socketFd < 0)
    {
        return;
    }

    logSuccess("Successfully connected to server at", "%s", settings->serverIP);

    send(socketFd, "2", 1, 0);

    logInfo("data requested", "");

    // read data into char * data
    char *data = (char *)calloc(65536, sizeof(char));

    read(socketFd, data, 65536);

    // parse data

    // getWriteLock(settings);

    int length = 0;

    sscanf(data, "%01d", &length);
    data += 1; // move pointer to the next byte

    if (length)
    {

        // extract name without ./saves/ and .done
        // save it in settings->filename
        settings->filename = extractPureName(settings->openProjectName);
        saveProject(settings);

        // I have a config file, now I need to save it
        char tmp[100];
        snprintf(tmp, 99, "%s.conf", settings->openProjectName);

        FILE *ptr = fopen(tmp, "wb");
        printf("%s\n", data);

        sscanf(data, "%06d", &length);

        fwrite(data + 6, sizeof(char), length - 1, ptr);
        fwrite("\n", sizeof(char), 1, ptr);
        fflush(ptr);
    }

    //close(socketFd);

    return;
}
char *getLocalIP()
{

    struct ifaddrs *ifAdddrPtr, *ifAddr; // pointers to interface address structures
    netInterface_t *current = NULL, *all;

    // Get all network interfaces
    if (getifaddrs(&ifAdddrPtr) == 0)
    {
        for (ifAddr = ifAdddrPtr; ifAddr != NULL; ifAddr = ifAddr->ifa_next)
        { // Iterate through the list of interfaces
            if (ifAddr->ifa_addr && ifAddr->ifa_addr->sa_family == AF_INET && startsWithInterestingInterfaces(ifAddr->ifa_name))
            { // Check if the address is IPv4
                // currentInterface->interfaceName = ifAddr->ifa_name;
                // struct sockaddr_in *ipaddr = (struct sockaddr_in *)ifAddr->ifa_addr;
                // currentInterface->IP = inet_ntoa(ipaddr->sin_addr); // casting is necessary to turn it into a string

                if (!current)
                {
                    current = (netInterface_t *)calloc(1, sizeof(netInterface_t));
                    all = current;
                }
                else if (current->next == NULL)
                {
                    current->next = (netInterface_t *)calloc(1, sizeof(netInterface_t));
                    current = current->next;
                }
                current->interfaceName = ifAddr->ifa_name;
                struct sockaddr_in *ipaddr = (struct sockaddr_in *)ifAddr->ifa_addr;
                current->IP = inet_ntoa(ipaddr->sin_addr); // casting is necessary to turn it into a string
                current->next = NULL;
                printf("Found interface %s with IP address %s\n", current->interfaceName, current->IP);
            }
        }
        freeifaddrs(ifAdddrPtr); // free the memory allocated for the interface list
    }
    else
    {
        logError("Error while looking for interfaces", "");
    }

    // now looking for the best interface, preferrably ethernet

    for (netInterface_t *i = all; i; i = i->next)
    {
        if (startsWith("enp", i->interfaceName))
            return i->IP;
    }
    for (netInterface_t *i = all; i; i = i->next)
    {
        if (startsWith("eth", i->interfaceName))
            return i->IP;
    }
    for (netInterface_t *i = all; i; i = i->next)
    {
        if (startsWith("wl", i->interfaceName))
            return i->IP;
    }
    return "-1.-1.-1.-1";

    /*
    char *hostbuffer = (char *)calloc(256, sizeof(char));
    char *IPbuffer;
    struct hostent *host_entry;

    // Get the hostname
    gethostname(hostbuffer, sizeof(hostbuffer));

    // To retrieve host information
    host_entry = gethostbyname(hostbuffer);

    if (host_entry == NULL) {
        return NULL;
    }

    // To convert an Internet network address into ASCII string
    IPbuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
    free(hostbuffer);

    return IPbuffer;
    */
}

void switchFromClientToServer(settings_t *settings)
{
    int socketFd = connectToSocket(settings->serverIP);

    if (socketFd < 0)
    {
        return;
    }

    logSuccess("Successfully connected to server at", "%s", settings->serverIP);

    char tmp[17];
    snprintf(tmp, 17, "1%s", getLocalIP());
    logInfo("new IP address of server:", "%s", tmp + 1);

    send(socketFd, tmp, strlen(tmp), 0);

    logInfo("Switch to server mode requested", "");

    // read data into char * data
    char *data = (char *)calloc(65536, sizeof(char));

    read(socketFd, data, 65536);

    int length = 0;

    sscanf(data, "%01d", &length);
    data += 1; // move pointer to the next byte

    if (length)
    { // the server we contacted is not an updated server anymore
        // parse ip address of next server
        settings->serverIP = strdup(data);
        logWarning("That server is not a server anymore, contacting", "%s", data);
        switchFromClientToServer(settings); // retry contacting the provided ip
        //close(socketFd);
        return;
    }
    else
    { // we can
        settings->filename = extractPureName(settings->openProjectName);
        saveProject(settings);
        char tmp[100];
        snprintf(tmp, 99, "%s.conf", settings->openProjectName);
        // printf("%s",tmp);
        FILE *ptr = fopen(tmp, "wb");
        printf("%s\n", data);
        sscanf(data, "%06d", &length);
        fwrite(data + 6, sizeof(char), length - 1, ptr);
        fwrite("\n", sizeof(char), 1, ptr);
        fflush(ptr);
        settings->isClient = 0;
        settings->serverIP = NULL;
        settings->nextServer = NULL;
        settings->isServer = 0;
        settings->hasToBeServer = 1; // the main will take it from here
    }

    //close(socketFd);

    return;
}

void fetchData(settings_t *settings, interface_t *interface)
{ // CLIENT CODE
    int socketFd = connectToSocket(settings->serverIP);

    if (socketFd < 0)
    {
        return;
    }

    logSuccess("Successfully connected to server at", "%s", settings->serverIP);

    send(socketFd, "0", 1, 0);

    logInfo("data requested", "");

    // read data into char * data
    char *data = (char *)calloc(65536, sizeof(char));

    read(socketFd, data, 65536);
    // parse data

    // getWriteLock(settings);

    /*logWarning("I'm crying too","");*/
    int length = 0, length1 = 0, length2 = 0;

    sscanf(data, "%01d", &length);
    data += 1; // move pointer to the next byte

    if (length)
    {
        // parse ip address of next server
        settings->serverIP = strdup(data);
        return;
    }

    sscanf(data, "%03d%03d%03d%03d%03d%03d%03d",
           &settings->absoluteCount,
           &settings->numnodes,
           &settings->numlink,
           &settings->numrectangles,
           &settings->numTexts,
           &settings->numBindings,
           &length);

    data = data + 21; // mi sposto all'inizio del nome, se presente

    settings->openProjectName = strndup(data, length);
    sscanf(data + length, "%01c", &settings->ipoac);
    settings->ipoac -= 48;
    data = data + length + 1; // mi sposto oltre nome e flag (IPoAC)

    // ora devo leggere i bindings

    settings->interfaceBindings = (binding_t *)calloc(settings->numBindings, sizeof(binding_t));
    for (int i = 0; i < settings->numBindings; i++)
    {
        sscanf(data, "%03d", &length);
        data += 3;
        settings->interfaceBindings[i].deviceName = strndup(data, length);
        data += length;
        sscanf(data, "%03d", &length);
        data += 3;
        settings->interfaceBindings[i].bindingInterfaceName = strndup(data, length);
        data += length;
    }

    // leggere nodes
    interface->nodes = (node_t *)calloc(settings->numnodes, sizeof(node_t));
    for (int i = 0; i < settings->numnodes; i++)
    {
        sscanf(data, "%03d", &length);
        data += 3;
        interface->nodes[i].name = strndup(data, length);
        data += length;
        sscanf(data, "%01d%04d%04d", &length, &interface->nodes[i].x, &interface->nodes[i].y);
        interface->nodes[i].type = length;
        data += 9;
    }

    // leggere links
    interface->links = (link_t *)calloc(settings->numlink, sizeof(link_t));
    for (int i = 0; i < settings->numlink; i++)
    {
        sscanf(data, "%03d%03d", &length, &length1);
        data += 6;
        interface->links[i].node1 = strndup(data, length);
        data += length;
        interface->links[i].node2 = strndup(data, length1);
        data += length1;
        sscanf(data, "%01d%01d", &length, &length1);
        interface->links[i].node1_type = (component_type_t)length;
        interface->links[i].node2_type = (component_type_t)length1;
        data += 2;
    }

    // leggere rectangles

    interface->rectangles = (rectangle_t *)calloc(settings->numrectangles, sizeof(rectangle_t));
    for (int i = 0; i < settings->numrectangles; i++)
    {
        sscanf(data, "%04d%04d%04d%04d%03d%03d%03d",
               &interface->rectangles[i].x,
               &interface->rectangles[i].y,
               &interface->rectangles[i].x1,
               &interface->rectangles[i].y1,
               &length,
               &length1,
               &length2);
        data += 25;
        interface->rectangles[i].r = (length + 128) % 256;
        interface->rectangles[i].g = (length1 + 128) % 256;
        interface->rectangles[i].b = (length2 + 128) % 256;
    }

    // leggere texts

    interface->texts = (text_t *)calloc(settings->numTexts, sizeof(text_t));
    for (int i = 0; i < settings->numTexts; i++)
    {
        sscanf(data, "%04d%04d%03d", &interface->texts[i].x, &interface->texts[i].y, &length);
        data += 11;
        interface->texts[i].text = strndup(data, length);
        data += length;
    }

    close(socketFd);

    // releaseWriteLock(settings);
    return;
}

char *serialize(void)
{

    char *bindings = calloc(4096, sizeof(char));
    char *nodes = calloc(4096, sizeof(char));
    char *links = calloc(4096, sizeof(char));
    char *rectangles = calloc(4096, sizeof(char));
    char *texts = calloc(8192, sizeof(char));

    // logWarning("I wanna cry","");

    getReadLock(settingsPtr);

    // parsing bindings

    for (int i = 0; i < settingsPtr->numBindings; i++)
    {
        char tmp[4];
        sprintf(tmp, "%03d", (int)strlen(settingsPtr->interfaceBindings[i].deviceName));
        strcat(bindings, tmp);                                                                                                 // strlen of deviceName
        strncat(bindings, settingsPtr->interfaceBindings[i].deviceName, strlen(settingsPtr->interfaceBindings[i].deviceName)); // deviceName
        sprintf(tmp, "%03d", (int)strlen(settingsPtr->interfaceBindings[i].bindingInterfaceName));
        strcat(bindings, tmp);                                                                                                                     // strlen of bindingInterfaceName
        strncat(bindings, settingsPtr->interfaceBindings[i].bindingInterfaceName, strlen(settingsPtr->interfaceBindings[i].bindingInterfaceName)); // bindingInterfaceName
    }

    // logWarning("pls help","");

    // parsing nodes

    for (int i = 0; i < settingsPtr->numnodes; i++)
    {
        char tmp[10];
        node_t currentNode = ((interface_t *)settingsPtr->GUIdata)->nodes[i];
        sprintf(tmp, "%03d", (int)strlen(currentNode.name));
        strcat(nodes, tmp);                                              // strlen of node name
        strncat(nodes, currentNode.name, (int)strlen(currentNode.name)); // node name
        sprintf(tmp, "%01d%04d%04d", currentNode.type, currentNode.x, currentNode.y);
        strcat(nodes, tmp); // node type, x and y
    }

    // logWarning("tonight i wont sleep","");

    // parsing links

    for (int i = 0; i < settingsPtr->numlink; i++)
    {
        char tmp[10];
        link_t currentLink = ((interface_t *)settingsPtr->GUIdata)->links[i];
        sprintf(tmp, "%03d%03d", (int)strlen(currentLink.node1), (int)strlen(currentLink.node2));
        strcat(links, tmp);                                                // node1 name length and node2 name length
        strncat(links, currentLink.node1, (int)strlen(currentLink.node1)); // node 1 name
        strncat(links, currentLink.node2, (int)strlen(currentLink.node2)); // node 2 name
        sprintf(tmp, "%01d%01d", currentLink.node1_type, currentLink.node2_type);
        strcat(links, tmp); // node1 type and node2 type
    }

    // logWarning("im gonna have nightmares","");

    // parsing rectangles

    for (int i = 0; i < settingsPtr->numrectangles; i++)
    {
        char tmp[26];
        rectangle_t currentRectangle = ((interface_t *)settingsPtr->GUIdata)->rectangles[i];
        sprintf(tmp, "%04d%04d%04d%04d", currentRectangle.x, currentRectangle.y, currentRectangle.x1, currentRectangle.y1);
        strcat(rectangles, tmp);                                                                                    // rectangle x, y, x1, y1
        sprintf(tmp, "%03d%03d%03d", currentRectangle.r + 128, currentRectangle.g + 128, currentRectangle.b + 128); // DON'T LIE TO THE COMPILER
        strcat(rectangles, tmp);                                                                                    // rectangle r, g, b
    }

    // logWarning("pls let me finish coding before 3 am","");

    // parsing texts

    for (int i = 0; i < settingsPtr->numTexts; i++)
    {
        char tmp[12];
        text_t currentText = ((interface_t *)settingsPtr->GUIdata)->texts[i];
        sprintf(tmp, "%04d%04d%03d", currentText.x, currentText.y, (int)strlen(currentText.text));
        strcat(texts, tmp);                                              // text x, y, strlen of text
        strncat(texts, currentText.text, (int)strlen(currentText.text)); // text
    }

    // logWarning("my soul left my body, see ya (in hell)!","");

    // constructing string to send, pls kill us :')

    char *stringToSend = calloc(65536, sizeof(char)); // %03d prints 3 digits from int and pads them with zeros if needed
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
             settingsPtr->ipoac != 0 ? 1 : 0, // converted into 0 or 1 depending on value
             bindings,
             nodes,
             links,
             rectangles,
             texts);

    // logWarning("did it really work?","");

    releaseReadLock(settingsPtr);

    logSuccess("Successfully sent data", "");

    return stringToSend;
}

void sendData(int sock)
{
    char *data = serialize();
    write(sock, data, strlen(data));
    return;
}

char *readConfigFile()
{
    if (settingsPtr->openProjectName)
    {
        char tmp[100];
        snprintf(tmp, 99, "%s.conf", settingsPtr->openProjectName);
        FILE *ptr = fopen(tmp, "rb");
        fseek(ptr, 0, SEEK_END); // searching the end of the file
        size_t size = ftell(ptr);
        rewind(ptr); // go back to the start
        char *configs = (char *)calloc(size, sizeof(char));
        size_t nBytesRead = fread(configs, sizeof(char), size, ptr);
        if (nBytesRead != size)
            return "";
        fclose(ptr);
        char *toReturn = (char *)calloc(size + 6, sizeof(char));
        snprintf(toReturn, size + 6, "%06d%s", (int)size, configs);
        return toReturn;
    }
    return "";
}

char *parseServerSwitchRequest(char *message)
{ // if client receives a 0, switch is successful, else it receives the new server ip
    // we have received the IP address of the new server
    message += 1;
    if (!settingsPtr->nextServer)
    { // the client can become server only if there is no other next server
        settingsPtr->nextServer = strdup(message);
        settingsPtr->isClient = 1;
        settingsPtr->serverIP = strdup(settingsPtr->nextServer);
        printf("%s\n", settingsPtr->serverIP);
        char *tmp = (char *)calloc(65536, sizeof(char));
        snprintf(tmp, 65536, "0%s", readConfigFile());
        return tmp;
    }

    char *tmp = (char *)calloc(17, sizeof(char));
    snprintf(tmp, 16, "1%s", settingsPtr->nextServer); // if we have received a server switch request but we have already passed control to someone else
    return tmp;
}

char *getFile()
{
    char *tmp = (char *)calloc(65536, sizeof(char));
    snprintf(tmp, 65536, "%01d%s", settingsPtr->openProjectName != NULL ? 1 : 0, readConfigFile());
    return tmp;
}

void *connection_handler(void *socket_desc)
{
    int sock = *(int *)socket_desc;
    char message[2048];
    int read_size;
    char *response;

    if ((read_size = recv(sock, message, 2048, 0)) > 0)
    {
        switch (message[0])
        {
        case '0': // respond with data
            if (!settingsPtr->nextServer)
            {
                logInfo("I am the server, I will grace thee with my beloved bits.", "");
                sendData(sock);
            }
            else
            { // they are not the server anymore, so they communicate the new IP address of the server
                char tmp[17];
                snprintf(tmp, 16, "1%s", settingsPtr->nextServer);
                write(sock, tmp, strlen(tmp));
            }
            break;
        case '1': // stop server and send ack
            logInfo("how dare thee try to overthrow my reign?", "");
            response = parseServerSwitchRequest(message);
            write(sock, response, strlen(response));
            logInfo("response:", "%s", response);
            break;
        case '2': // stopping client: they want the file
            logInfo("are you asking me for my configs?", "");
            response = getFile();
            write(sock, response, strlen(response));
            logInfo("sending config file content", "");
            break;
        default: // what are you trying to do? go away
            write(sock, message, strlen(message));
        }
    }
    else if (read_size == 0)
    {
        logInfo("Client disconnected", "");
    }
    else if (read_size == -1)
    {
        logError("recv failed", "");
    }

    free(socket_desc);
    return 0;
}

void *serverFunction(void *settings_void)
{

    settingsPtr = (settings_t *)settings_void;

    // ORA CREO LA SOCKET
    logInfo("Ready to start server", "");

    int socketFd = socket(AF_INET, SOCK_STREAM, 0); // Create a socket using IPv4 and TCP protocol
    if (socketFd < 0)
    {
        logError("Unable to create socket", "");
        return NULL;
    }

    struct sockaddr_in socketAddress;

    socketAddress.sin_family = AF_INET; // Set address family to IPv4
    socketAddress.sin_addr.s_addr = INADDR_ANY;
    socketAddress.sin_port = htons(PORT_NUMBER); // Set the port number to bind

    if (bind(socketFd, (struct sockaddr *)&socketAddress, sizeof(socketAddress)) < 0)
    {
        logError("bind failed", "");
        return NULL;
    }

    logSuccess("binding successful", "");

    listen(socketFd, 5);

    logInfo("waiting for incoming connections...", "");

    int c = sizeof(struct sockaddr_in);
    int client_sock, *new_sock;
    struct sockaddr_in client;

    while (!settingsPtr->deactivateServer && (client_sock = accept(socketFd, (struct sockaddr *)&client, (socklen_t *)&c)))
    {
        logInfo("connection accepted", "");

        pthread_t answer_thread;
        new_sock = calloc(1, sizeof(int));
        *new_sock = client_sock;

        if (pthread_create(&answer_thread, NULL, connection_handler, (void *)new_sock) < 0)
        {
            logError("could not create thread", "");
            return NULL;
        }
        logSuccess("handler assigned", "");
    }

    if (client_sock < 0)
    {
        logError("accept failed", "");
        return NULL;
    }

    close(socketFd);
    socketFd = -1;
    logInfo("socket closed", "");
    settingsPtr->deactivateServer = 0;

    return NULL;
}
