#!/bin/bash
set -ex
if [ "cpp" = "$MY_MODE" ]; then
    docker exec -ti $MY_NAME bash -c "echo $CC; $CC --version"
    docker exec -ti $MY_NAME bash -c "echo $CXX; $CXX --version"
else
    echo skip
fi
# vim : set expandtab ft=bash :
