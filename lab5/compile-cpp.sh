#!/bin/sh
echo "compiling naive6.cpp memory.cpp"
g++ naive6.cpp memory.cpp -o naive6-cpp
echo "compiling smart6.cpp memory.cpp"
g++ smart6.cpp memory.cpp -o smart6-cpp
echo "compiling naive16.cpp memory.cpp"
g++ naive16.cpp memory.cpp -o naive16-cpp
echo "compiling smart16.cpp memory.cpp"
g++ smart16.cpp memory.cpp -o smart16-cpp
echo "compiling naive25.cpp memory.cpp"
g++ naive25.cpp memory.cpp -o naive25-cpp
echo "compiling smart25.cpp memory.cpp"
g++ smart25.cpp memory.cpp -o smart25-cpp
