#!/bin/bash

ip4_gateway=$(ip r | grep default | awk '{print $3}' | head -1)
Device=$(ip r | grep default | awk '{print $5}' | head -1)
ipv4=$(ip a | grep -A2 "$Device" | grep "inet " | awk '{print($2)}')

if [ -z "$Device" ]; then
  echo "Error: Device value is empty."
  exit 1
fi

echo "Gateway: $ip4_gateway"
echo "Device: $Device"
echo ""
echo "Current IPv4: $ipv4"
read -p "Please enter the desired static IPv4 address: " new_ipv4

yaml_path="/etc/netplan/"
yaml_name=$(sudo ls $yaml_path | grep .yaml)
echo "$yaml_name"

sudo echo ' network:
  renderer: networkd
  ethernets:
    '$Device':
      dhcp4: no
      addresses:
        - '$new_ipv4'
      routes:
        - to: default
          via: '$ip4_gateway'
      nameservers:
        addresses: [8.8.8.8, 8.8.4.4]
  version: 2
 ' > "$yaml_path$yaml_name"

sudo netplan apply