// Docker orchestration commands library
#include "../lib/netlib.h"

#include <sys/socket.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// This functions exclusively include the interactions with the Linux system and docker API
// All the logical controls about already existing nodes, switches and cables must be added outside of this library

// Initialize the environment by creating, if not already created, the netns folder
int createNetnsDir()
{
    return system("sudo mkdir -p /var/run/netns 2>/dev/null");
}

// Get the PID of a container by passing its name
char *getContainerPID(char *name)
{

    char command[MAX_COMMAND_SIZE];

    strcpy(command, "sudo docker inspect -f '{{.State.Pid}}' ");
    strcat(command, name);
    char *pid = (char *)calloc(MAX_PID_SIZE, sizeof(char));
    FILE *fp = popen(command, "r");
    if (fp != NULL)
    {
        fgets(pid, MAX_PID_SIZE, fp);
        pclose(fp);

        pid[strlen(pid) - 1] = '\0'; // removing the newline character
        return pid;
    }
    return NULL;
}

// Create a new node
// A node can be a host (h) or a router (r)
int addNode(char *name, char type)
{
    char command[MAX_COMMAND_SIZE];

    if (name != NULL && strlen(name) <= MAX_NAME_SIZE && (type == 'h' || type == 'r'))
    {

        snprintf(command, MAX_COMMAND_SIZE, "sudo docker run -id --hostname %s --cap-add=NET_ADMIN --name %s --network none node:1.0.0 /bin/bash", name, name);

        if (!system(command))
        { // once the node is created, adding the node's namespace to the netns folder

            // retrieving the container's PID
            snprintf(command, MAX_COMMAND_SIZE, "sudo docker inspect -f '{{.State.Pid}}' %s", name);
            char *pid = getContainerPID(name);

            if (pid != NULL)
            {
                snprintf(command, MAX_COMMAND_SIZE, "sudo ln -s /proc/%s/ns/net /var/run/netns/%s", pid, pid);

                if (!system(command))
                { // if the file creation was succesfull, eventually perfom the last step for routers, by enabling the IP forwarding
                    if (type == 'r')
                    {
                        snprintf(command, MAX_COMMAND_SIZE, "sudo ip netns exec %s sysctl net.ipv4.ip_forward=1", pid);
                        free(pid);

                        if (!system(command))
                        {
                            return 0;
                        }
                    }
                    return 0;
                }
            }
        }
    }
    return -1;
}

// Delete a node by passing its name
int delNode(char *name)
{
    char command[MAX_COMMAND_SIZE];

    if (name != NULL && strlen(name) <= MAX_NAME_SIZE)
    {
        snprintf(command, MAX_COMMAND_SIZE, "sudo docker inspect -f '{{.State.Pid}}' %s | xargs -I {} sudo rm /var/run/netns/{}", name);

        if (!system(command))
        {
            snprintf(command, MAX_COMMAND_SIZE, "sudo docker kill %s && sudo docker rm %s", name, name);

            return system(command);
        }
    }
    return -1;
}

// Add a new OpenVSwitch bridge
int addSwitch(char *name)
{
    char command[MAX_COMMAND_SIZE];

    if (name != NULL && strlen(name) <= MAX_NAME_SIZE)
    {
        snprintf(command, MAX_COMMAND_SIZE, "sudo ovs-vsctl add-br %s", name);

        return system(command);
    }
    return -1;
}

// Delete an OpenVSwitch bridge
int delSwitch(char *name)
{
    char command[MAX_COMMAND_SIZE];

    if (name != NULL && strlen(name) <= MAX_NAME_SIZE)
    {
        snprintf(command, MAX_COMMAND_SIZE, "sudo ovs-vsctl del-br %s", name);

        return system(command);
    }
    return -1;
}

int addExternalInterface(char *name, char *interface)
{
    // interface will be used further to connect the container to the external interface, by firstly creating a bridged network and then choosing it while creating the container
    char command[MAX_COMMAND_SIZE];

    if (name != NULL && strlen(name) <= MAX_NAME_SIZE && interface != NULL && strlen(interface) <= MAX_NAME_SIZE)
    {
        snprintf(command, MAX_COMMAND_SIZE, "sudo docker run -id --cap-add=NET_ADMIN --name %s --network bridge node:1.0.0 /bin/bash", name);

        if (!system(command))
        { // once the node is created, adding the node's namespace to the netns folder

            // retrieving the container's PID
            snprintf(command, MAX_COMMAND_SIZE, "sudo docker inspect -f '{{.State.Pid}}' %s", name);

            char *pid = getContainerPID(name);

            if (pid != NULL)
            {
                snprintf(command, MAX_COMMAND_SIZE, "sudo ln -s /proc/%s/ns/net /var/run/netns/%s", pid, pid);

                if (!system(command))
                { // if the file creation was succesfull, eventually perfom the last step for routers, by enabling the IP forwarding
                    snprintf(command, MAX_COMMAND_SIZE, "sudo ip netns exec %s sysctl net.ipv4.ip_forward=1", pid);
                    free(pid);

                    return system(command);
                }
            }
        }
    }
    return -1;
}

// Add a new cable between two nodes
// Used to connect:
// - two hosts
// - two routers
// - a host and a router
// - a host and an external interface
// - a host and an external NATted interface
int addCableBetweenNodes(char *firstNode, char *secondNode)
{
    
    char command[MAX_COMMAND_SIZE];
    char endpoint1[MAX_NAME_SIZE + 10]; // used to build the connector's name
    char endpoint2[MAX_NAME_SIZE + 10];

    if (firstNode != NULL && strlen(firstNode) <= MAX_NAME_SIZE && secondNode != NULL && strlen(secondNode) <= MAX_NAME_SIZE)
    {

        // creating the connector's name
        snprintf(endpoint1, MAX_NAME_SIZE + 10, "veth-%s-%s", firstNode, secondNode);

        snprintf(endpoint2, MAX_NAME_SIZE + 10, "veth-%s-%s", secondNode, firstNode);

        snprintf(command, MAX_COMMAND_SIZE, "sudo ip link add %s type veth peer name %s", endpoint1, endpoint2);

        if (!system(command))
        { // if the cable connection was successful, connect the cable to the containers

            char *pid1 = getContainerPID(firstNode);
            char *pid2 = getContainerPID(secondNode);

            // connecting cable to the first container
            snprintf(command, MAX_COMMAND_SIZE, "sudo ip link set %s netns %s", endpoint1, pid1);
            system(command);

            // connecting cable to the second container
            snprintf(command, MAX_COMMAND_SIZE, "sudo ip link set %s netns %s", endpoint2, pid2);
            system(command);

            // turning on the first container interface
            snprintf(command, MAX_COMMAND_SIZE, "sudo ip netns exec %s ip link set dev %s up", pid1, endpoint1);
            system(command);

            // turning on the second container interface
            snprintf(command, MAX_COMMAND_SIZE, "sudo ip netns exec %s ip link set dev %s up", pid2, endpoint2);

            system(command);
            free(pid1);
            free(pid2);
        }
    }
    return -1;
}

// Delete a cable between two nodes
int delCableBetweenNodes(char *firstNode, char *secondNode)
{
    char command[MAX_COMMAND_SIZE];

    if (firstNode != NULL && strlen(firstNode) <= MAX_NAME_SIZE && secondNode != NULL && strlen(secondNode) <= MAX_NAME_SIZE)
    {

        char endpoint1[MAX_NAME_SIZE + 10]; // used to build the connector's name
        char endpoint2[MAX_NAME_SIZE + 10];

        // creating the connector's name
        snprintf(endpoint1, MAX_NAME_SIZE + 10, "veth-%s-%s", firstNode, secondNode);

        snprintf(endpoint2, MAX_NAME_SIZE + 10, "veth-%s-%s", secondNode, firstNode);

        // deleting the cable from the first container
        snprintf(command, MAX_COMMAND_SIZE, "sudo ip netns exec %s ip link del %s", getContainerPID(firstNode), endpoint1);

        return system(command);
    }
    return -1;
}

// Add a new cable between a node and a switch
int addCableBetweenNodeAndSwitch(char *nodeName, char *switchName)
{
    char command[MAX_COMMAND_SIZE];
    char hostEndpoint[MAX_NAME_SIZE + 10]; // used to build the connector's name
    char switchEndpoint[MAX_NAME_SIZE + 10];

    if (nodeName != NULL && strlen(nodeName) <= MAX_NAME_SIZE && switchName != NULL && strlen(switchName) <= MAX_NAME_SIZE)
    {

        // creating the connector's name
        snprintf(hostEndpoint, MAX_NAME_SIZE + 10, "sveth-%s-%s", nodeName, switchName);

        snprintf(switchEndpoint, MAX_NAME_SIZE + 10, "veth-%s-%s", nodeName, switchName);

        snprintf(command, MAX_COMMAND_SIZE, "sudo ip link add %s type veth peer name %s", hostEndpoint, switchEndpoint);

        if (!system(command))
        { // if the cable connection was successful, connect the cable to the switch and to the container

            char *pid = getContainerPID(nodeName);

            // connecting cable to the container
            snprintf(command, MAX_COMMAND_SIZE, "sudo ip link set %s netns %s", hostEndpoint, pid);
            system(command);

            // connecting cable to the switch
            snprintf(command, MAX_COMMAND_SIZE, "sudo ovs-vsctl add-port %s %s", switchName, switchEndpoint);
            system(command);

            // turning on the switch interface
            snprintf(command, MAX_COMMAND_SIZE, "sudo ip link set dev %s up", switchEndpoint);
            system(command);

            // turning on the container interface
            snprintf(command, MAX_COMMAND_SIZE, "sudo ip netns exec %s ip link set dev %s up", pid, hostEndpoint);
            system(command);
            free(pid);
        }
    }
    return -1;
}

// Delete a cable between a node and a switch
int delCableBetweenNodeAndSwitch(char *nodeName, char *switchName)
{
    char command[MAX_COMMAND_SIZE];

    if (nodeName != NULL && strlen(nodeName) <= MAX_NAME_SIZE && switchName != NULL && strlen(switchName) <= MAX_NAME_SIZE)
    {

        char hostEndpoint[MAX_NAME_SIZE + 10]; // used to build the connector's name
        char switchEndpoint[MAX_NAME_SIZE + 10];

        // creating the connector's name
        snprintf(hostEndpoint, MAX_NAME_SIZE + 10, "sveth-%s-%s", nodeName, switchName);

        snprintf(switchEndpoint, MAX_NAME_SIZE + 10, "veth-%s-%s", nodeName, switchName);

        // deleting the cable from the switch
        snprintf(command, MAX_COMMAND_SIZE, "sudo ovs-vsctl del-port %s %s", switchName, switchEndpoint);

        if (!system(command))
        { // if the cable deletion was successful, delete the cable from the container

            char *pid = getContainerPID(nodeName);

            // deleting the cable from the container, and so deleting the entire cable, now go home it's late
            snprintf(command, MAX_COMMAND_SIZE, "sudo ip netns exec %s ip link del %s", pid, hostEndpoint);

            free(pid);

            return system(command);
        }
    }
    return -1;
}

// Add a cable between two switches
int addCableBetweenSwitches(char *firstSwitch, char *secondSwitch)
{
    char command[MAX_COMMAND_SIZE];
    char endpoint1[MAX_NAME_SIZE + 10]; // used to build the connector's name
    char endpoint2[MAX_NAME_SIZE + 10];

    if (firstSwitch != NULL && strlen(firstSwitch) <= MAX_NAME_SIZE && secondSwitch != NULL && strlen(secondSwitch) <= MAX_NAME_SIZE)
    {

        // creating the connector's name
        snprintf(endpoint1, MAX_NAME_SIZE + 10, "patch-%s-%s", firstSwitch, secondSwitch);

        snprintf(endpoint2, MAX_NAME_SIZE + 10, "patch-%s-%s", secondSwitch, firstSwitch);

        // snprintf(command, MAX_COMMAND_SIZE, "sudo ip link add %s type veth peer name %s", endpoint1, endpoint2);
        snprintf(command, MAX_COMMAND_SIZE, "sudo ovs-vsctl add-port %s %s -- set interface %s type=patch options:peer=%s", firstSwitch, endpoint1, endpoint1, endpoint2);

        if (!system(command))
        { // if the first cable connection was successful, connect the second one

            snprintf(command, MAX_COMMAND_SIZE, "sudo ovs-vsctl add-port %s %s -- set interface %s type=patch options:peer=%s", secondSwitch, endpoint2, endpoint2, endpoint1);

            return system(command);
        }
    }
    return -1;
}

// Delete a cable between two switches
int delCableBetweenSwitches(char *firstSwitch, char *secondSwitch)
{
    char command[MAX_COMMAND_SIZE];

    if (firstSwitch != NULL && strlen(firstSwitch) <= MAX_NAME_SIZE && secondSwitch != NULL && strlen(secondSwitch) <= MAX_NAME_SIZE)
    {

        char endpoint1[MAX_NAME_SIZE + 10]; // used to build the connector's name
        char endpoint2[MAX_NAME_SIZE + 10];

        // creating the connector's name
        snprintf(endpoint1, MAX_NAME_SIZE + 10, "veth-%s-%s", firstSwitch, secondSwitch);

        snprintf(endpoint2, MAX_NAME_SIZE + 10, "veth-%s-%s", secondSwitch, firstSwitch);

        // deleting the cable from the first switch
        snprintf(command, MAX_COMMAND_SIZE, "sudo ovs-vsctl del-port %s %s", firstSwitch, endpoint1);

        if (!system(command))
        { // if the cable deletion was successful, delete the cable from the second switch

            snprintf(command, MAX_COMMAND_SIZE, "sudo ovs-vsctl del-port %s %s", secondSwitch, endpoint2);

            return system(command);
        }
    }
    return -1;
}

// Send a command to a container's network namespace
int sendNodeSetupCommand(char *name, char *command)
{
    char fullCommand[MAX_COMMAND_SIZE];

    if (name != NULL && command != NULL && strlen(name) <= MAX_PID_SIZE)
    {
        snprintf(fullCommand, MAX_COMMAND_SIZE, "docker exec %s %s", name, command);
        return system(fullCommand);
    }
    return -1;
}

// Send a command to a switch
int sendSwitchSetupCommand(char *command)
{
    char fullCommand[MAX_COMMAND_SIZE];

    if (command != NULL)
    {
        snprintf(fullCommand, MAX_COMMAND_SIZE, "%s", command);

        return system(fullCommand);
    }
    return -1;
}

// open a shell in a device (host or switch)
int openNodeShell(char *name)
{
    char command[MAX_COMMAND_SIZE];

    if (name != NULL && strlen(name) <= MAX_NAME_SIZE)
    {
        snprintf(command, MAX_COMMAND_SIZE, "konsole -e sudo docker exec -it %s /bin/bash", name);
        pid_t pid = fork();
        /*if (pid == 0)
        {
            system(command);
        }*/
    }
    return -1;
}

// open a shell for the switch
int openSwitchShell()
{
    char command[MAX_COMMAND_SIZE];

    snprintf(command, MAX_COMMAND_SIZE, "konsole");
    pid_t pid = fork();
    if (pid == 0)
    {
        system(command);
        exit(0);
    }
    return -1;
}

interfaces *getNetInterfaces()
{
    struct ifaddrs *ifap, *ifa;
    interfaces *res = (interfaces *)calloc(1, sizeof(interfaces));

    if (getifaddrs(&ifap) == -1)
    {
        perror("getifaddrs");
        return NULL;
    }

    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
        {
            continue;
        }
        res->interfaces++; // counting the available interfaces
    }

    res->interfaces_name = (char **)calloc(res->interfaces, sizeof(char *)); // the last element is NULL
    for (int i = 0; i < res->interfaces; i++)
    {
        res->interfaces_name[i] = (char *)calloc(MAX_INTERFACE_SIZE, sizeof(char));
    }

    int i = 0; 
    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
        {
            continue;
        }

        int family = ifa->ifa_addr->sa_family;
        if (family == AF_INET || family == AF_INET6)
        {
            strcpy(res->interfaces_name[i++], ifa->ifa_name);
        }
    }

    freeifaddrs(ifap);
    return res;
}

void printNetInterfaces(interfaces *interfaces)
{
    for (int i = 0; i < interfaces->interfaces; i++)
    {
        printf("%s\n", interfaces->interfaces_name[i]);
    }
}

void freeInterfaces(interfaces *interfaces){
    for(int i = 0; i < interfaces->interfaces; i++){
        free(interfaces->interfaces_name[i]);
    }
    free(interfaces->interfaces_name);
    free(interfaces);
}