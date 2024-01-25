# Simba-Market-Data-Parser

## Build

Makefile is provided for compiling two binaries ```market_data_parser``` and ```mdp_tests```. Makefile is provided for portability reasons. A build system generator like ```CMake``` may be preferred in real production systems. ```$ make``` on commandline will build both the targets.
Tested compilation with g++ 11.4.0. C++20 features used.

## Usage

1. ```market_data_parser```
Usage: ./<binary-name> --out_file=response.json --inpcap_file=infile.pcap --logfile_path=log.txt
inpcap_file is a compulsory argument. The default output file is response.json. The default logfile path is log.txt.

2. ```mdp_tests```
Example Usage: ```./mdp_tests```

## Components

1. ```PcapReader```
    This component reads the pcap file and calls the ProtocolDecoder call back with the payload.

2. ```ProtocolDecoder```
    Processes the incoming packet and creates message structs that are further passed on to the json creator functor.

3. ```JsonCreator```
    Creates json strings for the supported messages and calls the writer callback.

4. ```StreamWriter```
    This is a generic stream writer that writes the json string to the provided stream.

## Supported Messages

1. Order Update
2. Order Execution
3. Best Prices
4. Order Book Snapshot

## Other files

1. ```scripts/sbe_dissector.lua```
This script was used for basic dissection of the protocol packets and filter them for creating test cases (using Wireshark).

2. ```commandline_parser.h```
Contains a free method that is used to define config parameters ```inpcap_file```,```out_file``` and ```logfile_path``` for the main binary.

3. ```schema/enums.h, schema/types.h, schema/bitmasks.h, schema/structs.h```  
FIX  protocol enums/data types/bitmasks and common structs that are used in all the market data messages. The ones used here are only defined.

4. ```tag_structs.h```
Tag structs that are used for signalling the start and end of packet from decoder to its output processor (JsonCreator here)

5. ```application_layer_messages.h```
Definitions of all the structs that will be filled by the decoder after parsing raw packets.

## Testing

1. ```tests.h```
Defines the interface of test_fixture.

2. ```tests/tests.cpp```
Source file containing the implementation of tests and main routine.

3. ```tests/test_pcaps/*```
Contains testing pcaps.

4. ```tests/expected_json_outputs/*```
Contains Json output corresponding to the testing pcaps.

## Implementation Notes

Modern C++ 17, 20 features like ```std::optional, std::concepts, std::to_chars, enum classes, structured bindings```. Use of third party libraries for pasing config parameters, logging, testing etc. is avoided for portability and minimizing dependencies. This limits the flexibility/safety/performance improvement of the current implementation that is generally provided by the third party libraries.
