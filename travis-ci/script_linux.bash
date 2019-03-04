#!/bin/bash
set -ex
if [ "cpp" = "$MY_MODE" ]; then
    docker exec -ti $MY_NAME bash -c "cd $MY_TARGET_MOUNT; env CPATH=$MY_CPATH make CXX=$CXX"
    docker exec -ti $MY_NAME bash -c "cd $MY_TARGET_MOUNT; make check"
elif [ "python" = "$MY_MODE" ]; then
    docker exec -ti $MY_NAME bash -c "cd $MY_TARGET_MOUNT; py.test-3"
else
    echo skip
fi
# vim: set expandtab :
