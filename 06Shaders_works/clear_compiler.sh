#! /bin/bash

if [ -d "./build/" ]; then
    echo "remove the build dir"
    safe-rm -rf ./build/ 
fi