#include "pcap_reader.h"
#include <iostream>
#include <pcap/pcap.h>
#include <cstring>

namespace reader {
PcapReader::PcapReader(DecoderCb &&cb) : cb_(cb) {}
void PcapReader::read_packets(const std::string &file_path) {
  char errbuf[PCAP_ERRBUF_SIZE];
  auto handle = pcap_open_offline(file_path.c_str(), errbuf);
  if (handle == nullptr) {
    std::cout << "PcapReader: Could not open file = " << file_path << std::endl;
    exit(EXIT_FAILURE);
  }

  pcap_set_snaplen(handle, n_maximum_transmission_unit);
  struct pcap_pkthdr *header;
  const uint8_t *packet;
  int result = pcap_next_ex(handle, &header, &packet);
  while (result != PCAP_ERROR_BREAK) {
    result = pcap_next_ex(handle, &header, &packet);
    // The packet points to an internal buffer of pcap library. For safety (to
    // future modifications in pcap) and possible use later in multithreaded
    // code etc., it might  be better to copy it in our own local buffer.
    std::memcpy(read_buffer_,packet+n_payload_offset, header->caplen - n_payload_offset);
    cb_(read_buffer_);
    std::memset(read_buffer_, 0, n_maximum_transmission_unit);
  }
}

} // namespace reader