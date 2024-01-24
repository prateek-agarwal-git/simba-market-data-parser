#pragma once
#include <string>

namespace writer {
template <typename Stream> struct StreamWriter {
  StreamWriter(Stream &stream) : stream_(stream) {}
  void operator()(const std::string& output){
    stream_<<output;
  }

private:
  Stream &stream_;
};

} // namespace writer