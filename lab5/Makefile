# the compiler: gcc for C program, define as g++ for C++
CC = gcc
RM = rm

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS = -g -Wall -std=c99

# the build target executable:
#TARGET = naive6 naive16 naive25 smart6 smart16 smart25

all: naive6 naive16 naive25 smart6 smart16 smart25

naive6: naive6.c memory.c
	$(CC) $(CFLAGS) -o naive6 naive6.c memory.c

naive16: naive16.c memory.c
	$(CC) $(CFLAGS) -o naive16 naive16.c memory.c

naive25: naive25.c memory.c
	$(CC) $(CFLAGS) -o naive25 naive25.c memory.c

smart6: smart6.c memory.c
	$(CC) $(CFLAGS) -o smart6 smart6.c memory.c

smart16: smart16.c memory.c
	$(CC) $(CFLAGS) -o smart16 smart16.c memory.c

smart25: smart25.c memory.c
	$(CC) $(CFLAGS) -o smart25 smart25.c memory.c

clean:	
	$(RM) naive6 naive16 naive25 smart6 smart16 smart25
