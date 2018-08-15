#!/bin/bash
# usage: ~ [ip] [port] [dir]
set -e
set -x
cd $3/infiniswap
git checkout Dashboard
cd setup
./ib_setup.sh $1
sleep 10
cd ../infiniswap_daemon
make clean && make && ./infiniswap-daemon $1 $2