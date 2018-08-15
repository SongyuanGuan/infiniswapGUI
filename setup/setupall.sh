#!/bin/bash

# run this script on the host machine and it will automatically setup infiniswap on all the devices in the list

set -e
set -x

server_host=`cat server.list | cut -d : -f 1`
server_ip=`cat server.list | cut -d : -f 2`
tmux new -s server && tmux send -t server "./connect.exp ${server_host} ${server_ip}  GUI server" ENTER # setup server

sleep 5

for line in `cat device.list`
do
    host=`echo $line | cut -d : -f 1`
    ip=`echo $line | cut -d : -f 2`

    tmux new -s ${ip//./_}c && tmux send -t ${ip//./_}c "./connect.exp ${host} ${ip} GUI client" ENTER # setup client
    tmux new -s ${ip//./_}d && tmux send -t ${ip//./_}d "./connect.exp ${host} ${ip} DM 9400" ENTER # setup daemon
done

sleep 30

for line in `cat bd.list`
do  
    host=`echo $line | cut -d : -f 1`
    ip=`echo $line | cut -d : -f 2`

    tmux new -s ${ip//./_}b && tmux send -t ${ip//./_}b "./connect.exp ${host} ${ip} BD" ENTER # setup bd 
done 