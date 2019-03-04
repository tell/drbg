#!/bin/bash
set -ex
docker exec -ti $MY_NAME bash -c "apt install -y time wget build-essential git pkg-config libssl-dev python3-pytest" > /dev/null
# docker exec -ti $MY_NAME bash -c "apt install -y libsodium-dev" > /dev/null
docker exec -ti $MY_NAME bash -c "wget https://github.com/jedisct1/libsodium/releases/download/1.0.17/libsodium-1.0.17.tar.gz; tar xf libsodium-1.0.17.tar.gz; cd libsodium-1.0.17; ./configure; make -j; make -j install" > /dev/null
docker exec -ti $MY_NAME bash -c "cd $MY_TARGET_MOUNT; git clone https://github.com/herumi/cybozulib.git" > /dev/null
docker exec -ti $MY_NAME bash -c "export CPATH=$MY_CPATH" > /dev/null
