.PHONY: all 
CC=g++
INC_DIR=./include/
CPPFLAGS=-g -std=c++20  -Wall -I$(INC_DIR)
#OBJ_FILES=build/commandline_parser.o build/game.o build/human_player.o build/computer_player.o
all: build/main
build/main: src/main.cpp ${INC_DIR}/*
	$(CC) $(CPPFLAGS)  src/main.cpp -o build/main
 
clean:
	 rm -f  build/*
 
