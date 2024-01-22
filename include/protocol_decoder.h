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
  template <typename T> void get_value(const uint8_t *&buffer) {
    T value = *reinterpret_cast<const T *>(buffer);
    buffer += sizeof(T);
    return value;
  }
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
inline size_t ProtocolDecoder<OutputFunctor>::process_order_update(
    const std::uint8_t *buffer) {
  using namespace messages::application_layer;
  OrderUpdate order_update{
      .S = get_value<decltype(OrderUpdate::S)>(buffer),
      .MDEntryId = get_value<decltype(OrderUpdate::MDEntryId)>(buffer),
      .MDEntryPx = {.mantissa =
                        get_value<decltype(schema::types::Decimal5::mantissa)>(
                            buffer)},
      .MDEntrySize = get_value<decltype(OrderUpdate::MDEntrySize)>(buffer),
      .MDFlags = get_value<decltype(OrderUpdate::MDFlags)>(buffer),
      .MDFlags2 = get_value<decltype(OrderUpdate::MDFlags2)>(buffer),
      .SecurityId= get_value<decltype(OrderUpdate::SecurityId)>(buffer),
      .RptSeq =  get_value<decltype(OrderUpdate::RptSeq)>(buffer),
      .MDUpdateAction = get_value<decltype(OrderUpdate::MDUpdateAction)>(buffer),
      .MDEntryType= get_value<decltype(OrderUpdate::MDEntryType)>(buffer),
  };
  output_(order_update);
  output_(tag_structs::end_packet{});
  return sizeof(order_update);
}
// struct OrderExecution {
//   schema::structs::SBEHeader S;
//   std::int64_t MDEntryId;
//   schema::types::Decimal5Null MDEntryPx;
//   schema::types::Int64NULL MDEntrySize;
//   schema::types::Decimal5 LastPx;
//   std::int64_t LastQty;
//   std::int64_t TradeId;
//   std::uint64_t MDFlags;
//   std::uint64_t MDFlags2;
//   std::int32_t SecurityId;
//   std::uint32_t RptSeq;
//   schema::enums::MDUpdateAction MDUpdateAction;
//   schema::enums::MDEntryType MDEntryType;
// };

template <typename OutputFunctor>
inline size_t ProtocolDecoder<OutputFunctor>::process_order_execution(
    const std::uint8_t *buffer) {
  messages::application_layer::OrderExecution order_execution{
      .S = *(reinterpret_cast<const schema::structs::SBEHeader *>(buffer))};
  return {};
}

template <typename OutputFunctor>
inline size_t ProtocolDecoder<OutputFunctor>::process_best_prices(
    const std::uint8_t *buffer) {
  return {};
}
} // namespace simba