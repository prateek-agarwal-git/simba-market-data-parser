# Simba-Market-Data-Parser

## Build
Makefile is provided for compiling two binaries ```market_data_parser``` and ```mdp_tests```. Makefile is provided for portability reasons. A build system generator like ```CMake``` may be preferred in real production systems. ```$ make``` on commandline will build both the targets.

## Usage

1. 


2. ```mdp_tests``` 
Example Usage: ```./mdp_tests```
//
//## Build
//
//Makefile is provided that can be used to compile ```rps``` and ```rps_tests```. Makefile is provided for portability reasons. A build system generator like ```CMake``` may be preferred in real production systems. ```$ make``` on commandline will build both the main and test targets.
//
//## Usage
//
//1. ```rps```
//Example Usage: ```./rps --num_runs=10 --infile_path=../sample_inputs/input_with_all_valid_tokens.txt --logfile_path=log.txt```
//All parameters are optional. The default value of ```num_runs``` is 10. If no infile_path is provided or the path is invalid, the input is taken from the terminal. The default logfile path is ```log.txt```.
//
//2. ```rps_tests```
//Example Usage: ```./rps_tests```
//The test log (```test_<pid>.txt```) is created in the same directory from where the binary is run.
//
//## Components
//
//1. ```commandline_parser```: A free method to parse the commandline and get ```infile_path``` , ```logfile_path```, ```num_runs```.
//
//2. ```Player```: The base class for ```HumanPlayer``` and ```ComputerPlayer```. Please note that **static polymorphism/CRTP** is used for the implementation.
//
//3. ```HumanPlayer```: This ```Player``` generates next move in the game through the input stream. The valid input strings are **rock**, **paper**, and **scissors** (*case insensitive match*).
//
//4. ```ComputerPlayer```: This  player generates the random sequence of moves using  uniform integer distribution and maps the generated integer to different hand_shapes. This uses ```std::random_device``` that gives no guarantee of repeatability of random sequences. For testing, two ```HumanPlayers``` are pitted against each other.
//
//5. ```Game```: This object facilitates the game and maintains the stats/counters for each set of runs. This can be reused with different players and multiple games can be played. The ```reset_stats``` public method may be called if required before each run. It also has a handle to the output_stream (logfile) to log stats/error messages.
//
//6. ```tests.cpp/tests.h```: It has test fixture and main routine to execute the tests.  The debug time asserts are used to match the output with the expected values. The tests checks the rules of the game, invalid input is handled cleanly, two computer players are pitted against each other for a longer run, some fixed sequence for  two players, the same set of moves producing all ties, the input sequence being  longer/shorter than the configured number of runs.
//
//7. ```common_defs.h```: It contains enum classes for computed result of a run and hand_shape and converter of them to string.
//
//## Implementation Notes
//
//Modern C++ features like ```enum class```es, ```std::optional```, ```structured binding``` and others are used in the implementation. Use of third party libraries for pasing config parameters, logging, testing etc. is avoided for portability and minimizing dependencies. This limits the flexibility/safety/performance improvement of the current implementation that is generally provided by the third party libraries.
//
