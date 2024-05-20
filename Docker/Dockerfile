FROM debian:latest

# Required packages
RUN apt update && apt upgrade -y
RUN apt install -y iproute2 iputils-ping net-tools tcpdump iptables netcat-openbsd vim python3 dsniff isc-dhcp-server isc-dhcp-client traceroute frr man systemctl lolcat
COPY config/ /root/