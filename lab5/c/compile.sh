#!/bin/sh
echo "compiling naive6.c memory.c"
gcc naive6.c memory.c -o naive6
echo "compiling smart6.c memory.c"
gcc smart6.c memory.c -o smart6
echo "compiling naive16.c memory.c"
gcc naive16.c memory.c -o naive16
echo "compiling smart16.c memory.c"
gcc smart16.c memory.c -o smart16
echo "compiling naive25.c memory.c"
gcc naive25.c memory.c -o naive25
echo "compiling smart25.c memory.c"
gcc smart25.c memory.c -o smart25
