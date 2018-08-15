#!/bin/bash
# usage: ~ [dir]

set -x

cd $1
if [ ! -d "infiniswapGUI" ]; then
    git clone https://github.com/SongyuanGuan/infiniswapGUI
else
    cd infiniswapGUI
    git checkout linux
    git checkout .
    git pull origin linux
fi



cd $1
if [ ! -d "infiniswap" ]; then
git clone https://github.com/SongyuanGuan/infiniswap
else
    cd infiniswap
    git checkout Dashboard
    git checkout .
    git pull origin Dashboard
fi