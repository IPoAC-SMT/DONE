#!/bin/bash

# Base template PoC setup
# Two nodes in the same subnet connected through a switch


# launching containers
docker run -id --name node1 --network none node:1.0.0 /bin/ash
docker run -id --name node2 --network none node:1.0.0 /bin/ash


node1PID=`docker inspect -f "{{.State.Pid}}" node1`
node2PID=`docker inspect -f "{{.State.Pid}}" node2`


sudo mkdir /var/run/netns/ 2> /dev/null

# linking containers' namespaces to be used by ip netns
sudo ln -s "/proc/${node1PID}/ns/net" "/var/run/netns/${node1PID}"
sudo ln -s "/proc/${node2PID}/ns/net" "/var/run/netns/${node2PID}"

# creating an ovs-switch
sudo ovs-vsctl add-br s1

# creating the first virtual cable
sudo ip link add veth-node1 type veth peer name veth-node1-s1
# connecting the cable to the first container
sudo ip link set veth-node1 netns "${node1PID}"

# attaching the other side of the vcable to the switch
sudo ovs-vsctl add-port s1 veth-node1-s1

# turning on the interface
sudo ip link set dev veth-node1-s1 up 
sudo ip netns exec "${node1PID}" ip link set dev veth-node1 up

# setting an IP address to the container interface
sudo ip netns exec "${node1PID}" ip addr add 10.0.0.1/24 dev veth-node1

# creating the second vcable
sudo ip link add veth-node2 type veth peer name veth-node2-s1

# attaching the second vcable to the second container
sudo ip link set veth-node2 netns "${node2PID}"

# attaching the second vcable to the switch
sudo ovs-vsctl add-port s1 veth-node2-s1

# turning on the interfaces
sudo ip link set dev veth-node2-s1 up
sudo ip netns exec "${node2PID}" ip link set dev veth-node2 up

# setting an IP address also
sudo ip netns exec "${node2PID}" ip addr add 10.0.0.2/24 dev veth-node2

echo "pinging node2 from node1"
docker exec -it node1 ping -c 2 10.0.0.2

echo "pinging node1 from node2"
docker exec -it node2 ping -c 2 10.0.0.1

echo "done!"

