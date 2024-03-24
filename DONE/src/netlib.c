// Docker orchestration commands library

#include "../lib/netlib.h"

// This functions exclusively include the interactions with the Linux system and docker API
// All the logical controls about already existing nodes, switches and cables must be added outside of this library

// Initialize the environment by creating, if not already created, the netns folder
int initEnv()
{
    return system("sudo mkdir -p /var/run/netns");
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

        strcpy(command, "sudo docker run -id --name ");
        strcat(command, name);
        strcat(command, " --network none node:1.0.0 /bin/ash");

        if (!system(command))
        { // once the node is created, adding the node's namespace to the netns folder

            // retrieving the container's PID
            strcpy(command, "sudo docker inspect -f '{{.State.Pid}}' ");
            strcat(command, name);
            char *pid = getContainerPID(name);

            if (pid != NULL)
            {

                strcpy(command, "sudo ln -s /proc/");
                strcat(command, pid);
                strcat(command, "/ns/net /var/run/netns/");
                strcat(command, pid);

                if (!system(command))
                { // if the file creation was succesfull, eventually perfom the last step for routers, by enabling the IP forwarding
                    if (type == 'r')
                    {
                        strcpy(command, "sudo ip netns exec ");
                        strcat(command, pid);
                        strcat(command, " sysctl net.ipv4.ip_forward=1");

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
        strcpy(command, "sudo docker inspect -f '{{.State.Pid}}' ");
        strcat(command, name);
        strcat(command, " | xargs -I {} sudo rm /var/run/netns/{}");

        if (!system(command))
        {
            strcpy(command, "sudo docker stop ");
            strcat(command, name);
            strcat(command, " && sudo docker rm ");
            strcat(command, name);

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
        strcpy(command, "sudo ovs-vsctl add-br ");
        strcat(command, name);

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
        strcpy(command, "sudo ovs-vsctl del-br ");
        strcat(command, name);

        return system(command);
    }
    return -1;
}

int addExternalInterface(char *name, char *interface)
{

    char command[MAX_COMMAND_SIZE];

    if (name != NULL && strlen(name) <= MAX_NAME_SIZE && interface != NULL && strlen(interface) <= MAX_NAME_SIZE)
    {
        strcpy(command, "sudo docker run -id --name ");
        strcat(command, name);
        strcat(command, " --network ");
        strcat(command, interface);
        strcat(command, " node:1.0.0 /bin/ash");

        if (!system(command))
        { // once the node is created, adding the node's namespace to the netns folder

            // retrieving the container's PID
            strcpy(command, "sudo docker inspect -f '{{.State.Pid}}' ");
            strcat(command, name);
            char *pid = getContainerPID(name);

            if (pid != NULL)
            {

                strcpy(command, "sudo ln -s /proc/");
                strcat(command, pid);
                strcat(command, "/ns/net /var/run/netns/");
                strcat(command, pid);

                if (!system(command))
                { // if the file creation was succesfull, eventually perfom the last step for routers, by enabling the IP forwarding
                    strcpy(command, "sudo ip netns exec ");
                    strcat(command, pid);
                    strcat(command, " sysctl net.ipv4.ip_forward=1");

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
        strcpy(endpoint1, "veth-");
        strcat(endpoint1, firstNode);

        strcpy(endpoint2, "veth-");
        strcat(endpoint2, secondNode);

        strcpy(command, "sudo ip link add ");
        strcat(command, endpoint1);
        strcat(command, " type veth peer name ");
        strcat(command, endpoint2);

        if (!system(command))
        { // if the cable connection was successful, connect the cable to the containers

            char *pid1 = getContainerPID(firstNode);
            char *pid2 = getContainerPID(secondNode);

            // connecting cable to the first container
            strcpy(command, "sudo ip link set ");
            strcat(command, endpoint1);
            strcat(command, " netns ");
            strcat(command, pid1);
            system(command);

            // connecting cable to the second container
            strcpy(command, "sudo ip link set ");
            strcat(command, endpoint2);
            strcat(command, " netns ");
            strcat(command, pid2);
            system(command);

            // turning on the first container interface
            strcpy(command, "sudo ip netns exec ");
            strcat(command, pid1);
            strcat(command, " ip link set dev ");
            strcat(command, endpoint1);
            strcat(command, " up");
            system(command);

            // turning on the second container interface
            strcpy(command, "sudo ip netns exec ");
            strcat(command, pid2);
            strcat(command, " ip link set dev ");
            strcat(command, endpoint2);
            strcat(command, " up");

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
        strcpy(endpoint1, "veth-");
        strcat(endpoint1, firstNode);

        strcpy(endpoint2, "veth-");
        strcat(endpoint2, secondNode);

        // deleting the cable from the first container
        strcpy(command, "sudo ip netns exec ");
        strcat(command, getContainerPID(firstNode));
        strcat(command, " ip link del ");
        strcat(command, endpoint1);

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
        strcpy(hostEndpoint, "veth-");
        strcat(hostEndpoint, nodeName);

        strcpy(switchEndpoint, hostEndpoint);
        strcat(switchEndpoint, "-");
        strcat(switchEndpoint, switchName);

        strcpy(command, "sudo ip link add ");
        strcat(command, hostEndpoint);
        strcat(command, " type veth peer name ");
        strcat(command, switchEndpoint);

        if (!system(command))
        { // if the cable connection was successful, connect the cable to the switch and to the container

            char *pid = getContainerPID(nodeName);

            // connecting cable to the container
            strcpy(command, "sudo ip link set ");
            strcat(command, hostEndpoint);
            strcat(command, " netns ");
            strcat(command, pid);
            system(command);

            // connecting cable to the switch
            strcpy(command, "sudo ovs-vsctl add-port ");
            strcat(command, switchName);
            strcat(command, " ");
            strcat(command, switchEndpoint);
            system(command);

            // turning on the switch interface
            strcpy(command, "sudo ip link set dev ");
            strcat(command, switchEndpoint);
            strcat(command, " up");
            system(command);

            // turning on the container interface
            strcpy(command, "sudo ip netns exec ");
            strcat(command, pid);
            strcat(command, " ip link set dev ");
            strcat(command, hostEndpoint);
            strcat(command, " up");

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
        strcpy(hostEndpoint, "veth-");
        strcat(hostEndpoint, nodeName);

        strcpy(switchEndpoint, hostEndpoint);
        strcat(switchEndpoint, "-");
        strcat(switchEndpoint, switchName);

        // deleting the cable from the switch
        strcpy(command, "sudo ovs-vsctl del-port ");
        strcat(command, switchName);
        strcat(command, " ");
        strcat(command, switchEndpoint);

        printf("%s\n", command);

        if (!system(command))
        { // if the cable deletion was successful, delete the cable from the container

            char *pid = getContainerPID(nodeName);

            // deleting the cable from the container, and so deleting the entire cable, now go home it's late
            strcpy(command, "sudo ip netns exec ");
            strcat(command, pid);
            strcat(command, " ip link del ");
            strcat(command, hostEndpoint);

            printf("%s\n", command);

            free(pid);

            return system(command);
        }
    }
    return -1;
}

// Send a command to a container's network namespace
int sendNetworkSetupCommand(char *pid, char *command)
{
    char fullCommand[MAX_COMMAND_SIZE];

    if (pid != NULL && command != NULL && strlen(pid) <= MAX_PID_SIZE)
    {
        strcpy(fullCommand, "sudo ip netns exec ");
        strcat(fullCommand, pid);
        strcat(fullCommand, " ");
        strcat(fullCommand, command);

        return system(fullCommand);
    }
    return -1;
}
