#pragma once
#include <cstdint>
#include <functional>
#include <string>
namespace reader {
struct PcapReader {
  using DecoderCb = std::function<void(const std::uint8_t *, int payload_length)>;
  PcapReader(DecoderCb &&cb);
  void read_packets(const std::string &file_path);

private:
  static constexpr size_t n_payload_offset =
      14 /*ethernet header*/ + 20 /*IP header*/ + 8 /*UDP header*/;
  static constexpr size_t n_maximum_transmission_unit = 1536;
  DecoderCb cb_;
  uint8_t
      read_buffer_[n_maximum_transmission_unit]; // sufficient for a UDP payload
};
} // namespace reader