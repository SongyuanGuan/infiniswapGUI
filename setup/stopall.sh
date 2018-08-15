#!/bin/bash

# run this script on the host machine and it will automatically kill all the running infiniswap and GUI programs

set -e
set -x

for line in `cat device.list`
do  
    echo $line
    host=`echo $line | cut -d : -f 1`
    ip=`echo $line | cut -d : -f 2`

    ./connect.exp ${host} ${ip} ST
done 