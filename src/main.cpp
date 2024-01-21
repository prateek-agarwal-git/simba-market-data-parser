#include <fcntl.h>
#include <string_view>
#include <iostream>
#include "application_layer_messages.h"

static constexpr std::string_view usage = R"( 
Usage: ./<binary-name> --out_file=response.json --infile=infile.dat--logfile_path=log.txt
All parameters are optional. The default value of num_runs is 10. If no infile_path 
is provided or the path is invalid, the input is taken from the terminal. The default logfile path is log.txt
    )";



int main(int argc, char **argv) {
	std::cout <<"hello world"<< std::endl;
//  const auto [num_runs, infile_path, logfile_path] =
//      commandline::parse_commandline(argc, argv);
//  if (argc == 1) {
//    std::cout << usage << std::endl;
//  }
//  std::ifstream ifs(infile_path, std::ios::in);
//  const bool infile_opened = ifs.is_open();
//  player::HumanPlayer H(infile_opened ? ifs : std::cin);
//  if (infile_path.size() > 0 && !infile_opened) {
//    std::cout
//        << "Unable to open file for reading. Please give input on terminal.\n";
//  }
//  player::ComputerPlayer C;
//  std::ofstream ofs(logfile_path, std::ios::out);
//  game::Game g(ofs.is_open() ? ofs : std::cout);
//  g.play(H, C, num_runs, true /*log_every_run*/);
//  g.log_stats();
//  std::cout << "Thank you for playing rock-paper-scissors. The results are "
//               "present in the log file: "
//            << logfile_path << ".\n";
//  ifs.close();
//  ofs.close();
//  return 0;
}
