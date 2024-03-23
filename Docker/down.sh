#!/bin/bash

# Base template PoC removal

node1PID=`docker inspect -f "{{.State.Pid}}" node1`
node2PID=`docker inspect -f "{{.State.Pid}}" node2`

sudo rm "/var/run/netns/${node1PID}"
sudo rm "/var/run/netns/${node2PID}"

docker stop node1 node2 && docker rm node1 node2

sudo ovs-vsctl del-br s1

