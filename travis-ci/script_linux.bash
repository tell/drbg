#!/bin/bash
set -ev
if [ "cpp" = "$MY_MODE" ]; then
    docker exec -ti $MY_NAME bash -c "set -ev;
        cd $MY_TARGET_MOUNT;
        export CPATH=$MY_CPATH;
        make CXX=$CXX;
        export LD_LIBRARY_PATH=/usr/local/lib;
        make check CXX=$CXX"
elif [ "python" = "$MY_MODE" ]; then
    docker exec -ti $MY_NAME bash -c "set -ev;
        export LD_LIBRARY_PATH=/usr/local/lib;
        cd $MY_TARGET_MOUNT;
        py.test-3"
else
    echo skip
fi
# vim: set expandtab :
