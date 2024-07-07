FROM debian:latest

# Required packages
RUN apt update && apt upgrade -y
RUN apt install -y curl iproute2 iputils-ping net-tools tcpdump iptables netcat-openbsd vim python3 dsniff isc-dhcp-server isc-dhcp-client traceroute frr man systemctl bind9 bind9-utils vlan apache2 dnsutils
COPY config/ /root/