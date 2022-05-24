# Usage:
# make        # compile all binary
# make clean  # remove ALL binaries and objects

.PHONY = all clean run build

CC = g++.exe

LINKERFLAG = -lm

CFLAGS = -std=c++17 -Wall -Werror -Wextra -pedantic -O1
FLAGS = 
RELEASE =

SRCS := $(wildcard *.cpp)
# SRCS := main.cpp
OBJECTS := $(SRCS:c/%.cpp=o/%.o)

all: main

build: main

run: main
	main.exe

main: $(OBJECTS)
	@echo "Linking.."
	${CC} $(CFLAGS) ${LINKERFLAG} $(OBJECTS) -o $@ $(FLAGS) $(RELEASE)

o/%.o: c/%.cpp
	@echo "Creating object.."
	${CC} $(CFLAGS) -c $< -o $@ $(FLAGS)

# the 'del' probably needs to be changed for linux users. This was made for a windows system.