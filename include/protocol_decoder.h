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
  template <typename T> T get_value(const uint8_t *&buffer) {
    T value = *reinterpret_cast<const T *>(buffer);
    buffer += sizeof(T);
    return value;
  }

  void check_null_and_update(const uint8_t *&buffer,
                             std::optional<int64_t> &opt) {
    int64_t val = get_value<int64_t>(buffer);
    if (val != schema::types::NullValues::Int64) {
      opt = val;
    }
  }
  void decode_incremental_packet(const uint8_t *buffer, int remaining_bytes);
  void decode_snapshot_packet(const uint8_t *buffer, std::size_t remaining_bytes);

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
ProtocolDecoder<OutputFunctor>::decode_snapshot_packet(const uint8_t *buffer,
                                                       size_t remaining_bytes) {

  auto *sbe_header =
      reinterpret_cast<const schema::structs::SBEHeader *>(buffer);
  using namespace simba::messages::application_layer;
  assert(static_cast<MessageTypes>(sbe_header->TemplateId) ==
         MessageTypes::OrderBookSnapShot);
  OrderBookSnapShot order_book_snapshot{
      .S = get_value<decltype(OrderBookSnapShot::S)>(buffer),
      .SecurityId = get_value<decltype(OrderBookSnapShot::SecurityId)>(buffer),
      .LastMsgSeqNumProcessed =
          get_value<decltype(OrderBookSnapShot::LastMsgSeqNumProcessed)>(
              buffer),
      .RptSeq = get_value<decltype(OrderBookSnapShot::RptSeq)>(buffer),
      .ExchangeTradingSessionId =
          get_value<decltype(OrderBookSnapShot::ExchangeTradingSessionId)>(
              buffer),
      .NoMDEntries =
          get_value<decltype(OrderBookSnapShot::NoMDEntries)>(buffer)};

  auto &entries = order_book_snapshot.Entries;
  entries.resize(order_book_snapshot.NoMDEntries.numInGroup, {});
  for (auto &entry : entries) {
    check_null_and_update(buffer, entry.MDEntryId);
    entry.TransactTime = get_value<decltype(entry.TransactTime)>(buffer);
    check_null_and_update(buffer, entry.MDEntryPx.mantissa);
    check_null_and_update(buffer, entry.MDEntrySize);
    check_null_and_update(buffer, entry.TradeId);
    entry.MDFlags = get_value<decltype(entry.MDFlags)>(buffer);
    entry.MDFlags2 = get_value<decltype(entry.MDFlags2)>(buffer);
    entry.MDEntryType = get_value<decltype(entry.MDEntryType)>(buffer);
  }

  output_(order_book_snapshot);
  output_(tag_structs::end_packet{});
  //assert(remaining_bytes == sizeof(OrderBookSnapShot::NoMDEntries) +
  //                              sizeof(OrderBookSnapShot::S) +
  //                              order_book_snapshot.S.BlockLength +
  //                              order_book_snapshot.NoMDEntries.blockLength *
  //                                  order_book_snapshot.NoMDEntries.numInGroup);
  return;
}

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
      auto delta = process_best_prices(buffer + bytes_processed);
      bytes_processed += delta;
    } break;
    case MessageTypes::OrderUpdate: {
      auto delta = process_order_update(buffer + bytes_processed);
      bytes_processed += delta;
    } break;
    case MessageTypes::OrderExecution: {
      auto delta = process_order_execution(buffer + bytes_processed);
      bytes_processed += delta;
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
      .SecurityId = get_value<decltype(OrderUpdate::SecurityId)>(buffer),
      .RptSeq = get_value<decltype(OrderUpdate::RptSeq)>(buffer),
      .MDUpdateAction =
          get_value<decltype(OrderUpdate::MDUpdateAction)>(buffer),
      .MDEntryType = get_value<decltype(OrderUpdate::MDEntryType)>(buffer),
  };
  output_(order_update);
  output_(tag_structs::end_packet{});
  return sizeof(order_update.S) + order_update.S.BlockLength;
}

template <typename OutputFunctor>
inline size_t ProtocolDecoder<OutputFunctor>::process_order_execution(
    const std::uint8_t *buffer) {
  using namespace messages::application_layer;
  OrderExecution order_execution{
      .S = get_value<decltype(OrderExecution::S)>(buffer),
      .MDEntryId = get_value<decltype(OrderExecution::MDEntryId)>(buffer),
  };
  check_null_and_update(buffer, order_execution.MDEntryPx.mantissa);
  check_null_and_update(buffer, order_execution.MDEntrySize);

  order_execution.LastPx.mantissa =
      get_value<decltype(schema::types::Decimal5::mantissa)>(buffer);
  order_execution.LastQty =
      get_value<decltype(OrderExecution::LastQty)>(buffer);
  order_execution.TradeId =
      get_value<decltype(OrderExecution::TradeId)>(buffer);
  order_execution.MDFlags =
      get_value<decltype(OrderExecution::MDFlags)>(buffer);
  order_execution.MDFlags2 =
      get_value<decltype(OrderExecution::MDFlags2)>(buffer);
  order_execution.SecurityId =
      get_value<decltype(OrderExecution::SecurityId)>(buffer);
  order_execution.RptSeq = get_value<decltype(OrderExecution::RptSeq)>(buffer);
  order_execution.MDUpdateAction =
      get_value<decltype(OrderExecution::MDUpdateAction)>(buffer);
  order_execution.MDEntryType =
      get_value<decltype(OrderExecution::MDEntryType)>(buffer);

  output_(order_execution);
  output_(tag_structs::end_packet{});
  return sizeof(order_execution.S) + order_execution.S.BlockLength;
}

template <typename OutputFunctor>
inline size_t ProtocolDecoder<OutputFunctor>::process_best_prices(
    const std::uint8_t *buffer) {

  using namespace messages::application_layer;
  BestPrices best_prices{
      .S = get_value<decltype(BestPrices::S)>(buffer),
      .NoMDEntries = get_value<decltype(BestPrices::NoMDEntries)>(buffer)};
  auto &entries = best_prices.Entries;
  entries.resize(best_prices.NoMDEntries.numInGroup, {});
  // TODO handle this magic constant
  assert(best_prices.NoMDEntries.blockLength == 36);
  for (auto &entry : entries) {
    check_null_and_update(buffer, entry.MktBidPx.mantissa);
    check_null_and_update(buffer, entry.MktOfferPx.mantissa);
    check_null_and_update(buffer, entry.MktBidSize);
    check_null_and_update(buffer, entry.MktOfferSize);
    entry.SecurityId = get_value<decltype(entry.SecurityId)>(buffer);
  }

  output_(best_prices);
  output_(tag_structs::end_packet{});
  size_t total_bytes_processed =
      sizeof(BestPrices::NoMDEntries) + sizeof(BestPrices::S) +
      best_prices.NoMDEntries.blockLength * best_prices.NoMDEntries.numInGroup;
  return total_bytes_processed;
}
} // namespace simba