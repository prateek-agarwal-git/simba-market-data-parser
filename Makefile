.PHONY: all 
CC=g++
INC_DIR=./include/
CPPFLAGS=-g -std=c++20  -Wall -I$(INC_DIR)
OBJ_FILES=build/json_creator.o
all: build/main build/tests
build/main: src/main.cpp ${INC_DIR}/* ${INC_DIR}/*/*
	$(CC) $(CPPFLAGS)  src/main.cpp -o build/main

build/tests: tests/tests.cpp ${INC_DIR}/* ${INC_DIR}/*/* $(OBJ_FILES)
	$(CC) $(CPPFLAGS)  tests/tests.cpp $(OBJ_FILES) -o build/tests

build/json_creator.o: src/json_creator.cpp ${INC_DIR}/* ${INC_DIR}/*/*
	$(CC) $(CPPFLAGS) -c src/json_creator.cpp -o build/json_creator.o
 
clean:
	 rm -f  build/*
 
