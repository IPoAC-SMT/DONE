#!/bin/bash

sudo docker image rm node:1.0.0
sudo docker build -t node:1.0.0 .