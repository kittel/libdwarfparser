#!/bin/bash

DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
echo $DIR

LD_LIBRARY_PATH=$DIR/../../libdwarf/libdwarf 
echo $LD_LIBRARY_PATH

LD_LIBRARY_PATH=$LD_LIBRARY_PATH $DIR/tests
