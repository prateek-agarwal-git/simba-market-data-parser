#pragma once
#include <string>
#include <tuple>
#include <utility>

namespace commandline {
//   returns {infile_prefix, outfile_path, logfile_path};
std::tuple<std::string, std::string, std::string>
parse_commandline(int argc, char **argv);
} // namespace commandline