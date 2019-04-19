# the compiler: gcc for C program, define as g++ for C++
#CC = gcc
CC = g++
RM = rm

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS = -g -Wall

# the build target executable:
#TARGET = naive6 naive16 naive25 smart6 smart16 smart25

all: naive6 naive16 naive25 smart6 smart16 smart25

naive6: naive6.cpp memory.cpp
	$(CC) $(CFLAGS) -o naive6 naive6.cpp memory.cpp

naive16: naive16.cpp memory.cpp
	$(CC) $(CFLAGS) -o naive16 naive16.cpp memory.cpp

naive25: naive25.cpp memory.cpp
	$(CC) $(CFLAGS) -o naive25 naive25.cpp memory.cpp

smart6: smart6.cpp memory.cpp
	$(CC) $(CFLAGS) -o smart6 smart6.cpp memory.cpp

smart16: smart16.cpp memory.cpp
	$(CC) $(CFLAGS) -o smart16 smart16.cpp memory.cpp

smart25: smart25.cpp memory.cpp
	$(CC) $(CFLAGS) -o smart25 smart25.cpp memory.cpp

clean:	
	$(RM) naive6 naive16 naive25 smart6 smart16 smart25
