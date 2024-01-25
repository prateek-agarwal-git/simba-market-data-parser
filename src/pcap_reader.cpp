#include "pcap_reader.h"
#include <iostream>
#include <pcap/pcap.h>
#include <cstring>

namespace reader {
PcapReader::PcapReader(DecoderCb &&cb, std::ostream& os) : cb_(cb), l_(os) {}
void PcapReader::read_packets(const std::string &file_path) {
  char errbuf[PCAP_ERRBUF_SIZE];
  auto handle = pcap_open_offline(file_path.c_str(), errbuf);
  if (handle == nullptr) {
    l_ << "PcapReader: Could not open file = " << file_path << std::endl;
    exit(EXIT_FAILURE);
  }

  pcap_set_snaplen(handle, n_maximum_transmission_unit);
  struct pcap_pkthdr *header;
  const uint8_t *packet;
  int result = pcap_next_ex(handle, &header, &packet);
  while (result != PCAP_ERROR_BREAK) {
    // The packet points to an internal buffer of pcap library. For safety (to
    // future modifications in pcap third party library) and possible use later in multithreaded
    // code etc., it might  be better to copy it in our own local buffer.
    const std::size_t payload_length = header->caplen - n_payload_offset;
    std::memcpy(read_buffer_,packet+n_payload_offset,payload_length);
    cb_({read_buffer_, payload_length});
    std::memset(read_buffer_, 0, sizeof(read_buffer_));
    result = pcap_next_ex(handle, &header, &packet);
  }
}

} // namespace reader