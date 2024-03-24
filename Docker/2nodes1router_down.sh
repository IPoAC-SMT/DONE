#!/bin/bash

# Base template PoC removal

node1PID=`docker inspect -f "{{.State.Pid}}" node1`
node2PID=`docker inspect -f "{{.State.Pid}}" node2`
node3PID=`docker inspect -f "{{.State.Pid}}" node3`

sudo rm "/var/run/netns/${node1PID}"
sudo rm "/var/run/netns/${node2PID}"
sudo rm "/var/run/netns/${node3PID}"

docker stop node1 node2 node3 && docker rm node1 node2 node3

sudo ovs-vsctl del-br s1

