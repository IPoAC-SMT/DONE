#!/bin/bash

# Base template PoC setup
# Two nodes in the same subnet and a default gateway to the Internet

# launching containers
docker run -id --name node1 --network none node:1.0.0 /bin/ash
docker run -id --name node2 --network none node:1.0.0 /bin/ash
docker run -id --name node3 node:1.0.0 /bin/ash


node1PID=`docker inspect -f "{{.State.Pid}}" node1`
node2PID=`docker inspect -f "{{.State.Pid}}" node2`
node3PID=`docker inspect -f "{{.State.Pid}}" node3`

sudo mkdir /var/run/netns/ 2> /dev/null

# linking containers' namespaces to be used by ip netns
sudo ln -s "/proc/${node1PID}/ns/net" "/var/run/netns/${node1PID}"
sudo ln -s "/proc/${node2PID}/ns/net" "/var/run/netns/${node2PID}"
sudo ln -s "/proc/${node3PID}/ns/net" "/var/run/netns/${node3PID}"

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

# adding a default gateway
sudo ip netns exec "${node1PID}" ip route add default via 10.0.0.3





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

# adding a default gateway
sudo ip netns exec "${node2PID}" ip route add default via 10.0.0.3




# creating the third vcable
sudo ip link add veth-node3 type veth peer name veth-node3-s1

# attaching the second vcable to the second container
sudo ip link set veth-node3 netns "${node3PID}"

# attaching the second vcable to the switch
sudo ovs-vsctl add-port s1 veth-node3-s1

# turning on the interfaces
sudo ip link set dev veth-node3-s1 up
sudo ip netns exec "${node3PID}" ip link set dev veth-node3 up

# setting an IP address also
sudo ip netns exec "${node3PID}" ip addr add 10.0.0.3/24 dev veth-node3

# turning on IPv4 forwarding
sudo ip netns exec "${node3PID}" sysctl net.ipv4.ip_forward=1

# adding a NAT rule for the outbound traffic from the LAN
sudo ip netns exec "${node3PID}" iptables -t nat -A POSTROUTING -s 10.0.0.0/24 -j MASQUERADE






echo "pinging node2 from node1"
docker exec -it node1 ping -c 2 10.0.0.2

echo "pinging node1 from node2"
docker exec -it node2 ping -c 2 10.0.0.1

echo "pinging the Internet from node1"
docker exec -it node1 ping -c 2 1.1.1.1

echo "node1 routing table"
docker exec -it node1 ip route

echo "node3 nat rule"
sudo ip netns exec "${node3PID}" iptables -t nat -L

echo "done!"

