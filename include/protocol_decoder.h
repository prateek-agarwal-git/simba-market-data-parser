#pragma once
#include "application_layer_messages.h"
#include "schema/structs.h"
#include "schema/types.h"
#include "tag_structs.h"
#include <cassert>
#include <cstdint>
#include <functional>

namespace simba {

template <typename OutputFunctor> struct ProtocolDecoder {
  ProtocolDecoder(OutputFunctor output) : output_(output) {}
  void operator()(const std::uint8_t *payload, int payload_length);

private:
  void decode_incremental_packet(const uint8_t *buffer, int remaining_bytes);
  void decode_snapshot_packet(const uint8_t *buffer, int remaining_bytes);

  std::size_t process_best_prices(const uint8_t *buffer);
  std::size_t process_order_update(const uint8_t *buffer);
  std::size_t process_order_execution(const uint8_t *buffer);
  OutputFunctor output_;
};

template <typename OutputFunctor>
inline void
ProtocolDecoder<OutputFunctor>::operator()(const std::uint8_t *packet,
                                           int payload_length) {
  auto *mdp_header =
      reinterpret_cast<const schema::structs::MarketDataPacketHeader *>(packet);
  if (mdp_header->MsgSize != payload_length) {
    // TODO add error log if required
    return;
  }
  output_(tag_structs::start_packet{});
  output_(*mdp_header);
  const bool is_snapshot = (mdp_header->MsgFlags & 0x8) == 0;
  constexpr auto processed_bytes =
      sizeof(schema::structs::MarketDataPacketHeader);
  if (is_snapshot) {
    decode_snapshot_packet(packet + processed_bytes,
                           payload_length - processed_bytes);
  } else {
    decode_incremental_packet(packet + processed_bytes,
                              payload_length - processed_bytes);
  }
}

template <typename OutputFunctor>
inline void
ProtocolDecoder<OutputFunctor>::decode_snapshot_packet(const uint8_t *packet,
                                                       int remaining_bytes) {}

template <typename OutputFunctor>
inline void ProtocolDecoder<OutputFunctor>::decode_incremental_packet(
    const uint8_t *buffer, const int remaining_bytes) {
  auto incremental_header =
      reinterpret_cast<const schema::structs::IncrementalPacketHeader *>(
          buffer);
  output_(*incremental_header);
  int bytes_processed = sizeof(schema::structs::IncrementalPacketHeader);
  while (remaining_bytes > bytes_processed) {
    auto *sbe_header = reinterpret_cast<const schema::structs::SBEHeader *>(
        buffer + bytes_processed);
    using namespace simba::messages::application_layer;
    switch (static_cast<MessageTypes>(sbe_header->TemplateId)) {
    case MessageTypes::BestPrices: {
      assert(sbe_header->BlockLength == 0);
      //      auto delta = process_best_prices(buffer + bytes_processed);
      //      bytes_processed += delta;
    } break;
    case MessageTypes::OrderUpdate: {
      //      auto delta = process_order_update(buffer + bytes_processed);
      //      bytes_processed += delta;
    } break;
    case MessageTypes::OrderExecution: {
      //     auto delta = process_order_execution(buffer + bytes_processed);
      //     bytes_processed += delta;
    } break;
    default: {
      // do not parse it, yet increment the processed bytes
      bytes_processed +=
          sizeof(schema::structs::SBEHeader) + sbe_header->BlockLength;
    }
    }
  }
}

template <typename OutputFunctor>
inline size_t  
ProtocolDecoder<OutputFunctor>::process_order_update(const std::uint8_t *buffer){
    return {};
}

template <typename OutputFunctor>
inline size_t  
ProtocolDecoder<OutputFunctor>::process_order_execution(const std::uint8_t *buffer){
    return {};
}

template <typename OutputFunctor>
inline size_t  
ProtocolDecoder<OutputFunctor>::process_best_prices(const std::uint8_t *buffer){
    return {};
}
} // namespace simba