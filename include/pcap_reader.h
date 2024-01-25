#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
namespace reader {
struct PcapReader {
  using DecoderCb =
      std::function<void(std::basic_string_view<uint8_t>)>;
  PcapReader(DecoderCb &&cb, std::ostream& log_os);
  void read_packets(std::string_view file_path);

private:
  static constexpr size_t n_payload_offset =
      14 /*ethernet header*/ + 20 /*IP header*/ + 8 /*UDP header*/;
  static constexpr size_t n_maximum_transmission_unit = 1536;
  DecoderCb cb_;
  uint8_t
      read_buffer_[n_maximum_transmission_unit]; // sufficient for a UDP payload
  std::ostream &l_;
};
} // namespace reader