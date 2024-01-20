.PHONY: all 
CC=g++
CPPFLAGS=-g -std=c++20  -Wall
#OBJ_FILES=build/commandline_parser.o build/game.o build/human_player.o build/computer_player.o
all: build/rps build/rps_tests
# build/rps: main.cpp $(OBJ_FILES) include/*
	# $(CC) $(CPPFLAGS)  main.cpp $(OBJ_FILES) -o build/rps
# build/rps_tests:tests.cpp $(OBJ_FILES)
	# $(CC)  $(CPPFLAGS) tests.cpp $(OBJ_FILES)  -o build/rps_tests
# 
# build/commandline_parser.o: commandline_parser.cpp include/commandline_parser.h
	# ${CC} $(CPPFLAGS) -c  commandline_parser.cpp -o build/commandline_parser.o
# 
# build/game.o: game.cpp include/*
	# ${CC} ${CPPFLAGS} -c game.cpp -o build/game.o
# 
# build/human_player.o: human_player.cpp include/*
	# ${CC} ${CPPFLAGS} -c human_player.cpp -o build/human_player.o
# 
# build/computer_player.o:computer_player.cpp include/*
	# ${CC} ${CPPFLAGS} -c computer_player.cpp -o build/computer_player.o
# 
# 
# 
# 
# clean:
	# rm -f  build/*
# 