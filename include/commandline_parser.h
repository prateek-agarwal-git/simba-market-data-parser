#include <cstring>
#include <getopt.h>
#include <iostream>
#include <unistd.h>
#include <tuple>
namespace commandline {
std::tuple<std::string, std::string, std::string>
parse_commandline(int argc, char **argv) {
  std::string inpcap_file;
  std::string outfile_path{"response.json"};
  std::string logfile_path("log.txt");
  // man 3 getopt
  while (true) {
    int option_index = 0;
    static struct option long_options[] = {
        {"inpcap_file", required_argument, 0, 0},
        {"outfile_path", required_argument, 0, 0},
        {"logfile_path", required_argument, 0, 0},
        {0, 0, 0, 0}};
    int c = getopt_long(argc, argv, "", long_options, &option_index);
    if (c == -1)
      break;
    switch (c) {
    case 0:
      if (strcmp(long_options[option_index].name, "inpcap_file") == 0) {
        inpcap_file = optarg;
      } else if (strcmp(long_options[option_index].name, "outfile_path") == 0) {
        outfile_path = optarg;
      } else if (strcmp(long_options[option_index].name, "logfile_path") == 0) {
        logfile_path = optarg;
      }
      break;
    default:
      break;
    }
  }
  if (inpcap_file.empty()){
    std::cout <<"inpcap_file is a compulsory argument." << std::endl;
  }
  return {inpcap_file, outfile_path, logfile_path};
}
} // namespace commandline