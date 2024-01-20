#include <cstdint>
//This class will read en entire UDP packet and call the Parser callback with one packet
template<typename ParserCb, std::size_t n_in_buffer_size = 2048>
class Reader{
 public:
    Reader(int fd, ParserCb cb )fd_(fd), cb_(cb): {}
 private:
 const int fd_;
 ParserCb cb_;
 char in_buffer_[n_in_buffer_size];
};