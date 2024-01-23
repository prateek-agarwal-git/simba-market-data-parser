#pragma once
#include "schema/bitmasks.h"
#include "schema/enums.h"
#include "schema/structs.h"
#include "schema/types.h"
#include <cstdint>
#include <vector>

namespace simba::messages::application_layer {
enum class MessageTypes : std::uint16_t {
  BestPrices = 14,
  OrderUpdate = 15,
  OrderExecution = 16,
  OrderBookSnapShot = 17,
};

struct BestPricesEntry {
  schema::types::Decimal5Null MktBidPx;
  schema::types::Decimal5Null MktOfferPx;
  schema::types::Int64NULL MktBidSize;
  schema::types::Int64NULL MktOfferSize;
  std::int32_t SecurityId;
  bool operator==(const BestPricesEntry &other) const {
    return MktBidPx == other.MktBidPx && MktOfferPx == other.MktOfferPx &&
           MktBidSize == other.MktBidSize && MktOfferSize == other.MktOfferSize&&SecurityId == other.SecurityId;
  }
};
struct BestPrices {
  schema::structs::SBEHeader S;
  schema::structs::groupSize NoMDEntries;
  std::vector<BestPricesEntry> Entries;
  bool operator==(const BestPrices &other) const {
    return S == other.S && NoMDEntries == other.NoMDEntries &&
           Entries == other.Entries;
  }
};

struct OrderUpdate {
  schema::structs::SBEHeader S;
  std::int64_t MDEntryId;
  schema::types::Decimal5 MDEntryPx;
  std::int64_t MDEntrySize;
  std::uint64_t MDFlags;
  std::uint64_t MDFlags2;
  std::int32_t SecurityId;
  std::uint32_t RptSeq;
  schema::enums::MDUpdateAction MDUpdateAction;
  schema::enums::MDEntryType MDEntryType;
  bool operator==(const OrderUpdate &other) const {
    return std::memcmp((const void *)this, (const void *)(&other),
                       sizeof(OrderUpdate)) == 0;
  }
} __attribute__((packed)); // 58 bytes

// same struct for full or partial execution as well as on calendar spreads.
// The / difference is in the values of respective fields.
struct OrderExecution {
  schema::structs::SBEHeader S;
  std::int64_t MDEntryId;
  schema::types::Decimal5Null MDEntryPx;
  schema::types::Int64NULL MDEntrySize;
  schema::types::Decimal5 LastPx;
  std::int64_t LastQty;
  std::int64_t TradeId;
  std::uint64_t MDFlags;
  std::uint64_t MDFlags2;
  std::int32_t SecurityId;
  std::uint32_t RptSeq;
  schema::enums::MDUpdateAction MDUpdateAction;
  schema::enums::MDEntryType MDEntryType;
  bool operator==(const OrderExecution &other) const {
    return (S == other.S) && MDEntryId == other.MDEntryId &&
           MDEntryPx == other.MDEntryPx && MDEntrySize == other.MDEntrySize &&
           LastPx == other.LastPx && LastQty == other.LastQty &&
           TradeId == other.TradeId && MDFlags == other.MDFlags &&
           MDFlags2 == other.MDFlags2 && SecurityId == other.SecurityId &&
           RptSeq == other.RptSeq && MDUpdateAction == other.MDUpdateAction &&
           MDEntryType == other.MDEntryType;
  }
};

struct SnapShotEntry {
  schema::types::Int64NULL MDEntryId;
  std::uint64_t TransactTime;
  schema::types::Decimal5Null MDEntryPx;
  schema::types::Int64NULL MDEntrySize;
  schema::types::Int64NULL TradeId;
  std::uint64_t MDFlags;
  std::uint64_t MDFlags2;
  schema::enums::MDEntryType MDEntryType;
  bool operator==(const SnapShotEntry &other) const {
    return MDEntryId == other.MDEntryId && MDEntryPx == other.MDEntryPx &&
           MDEntrySize == other.MDEntrySize && TradeId == other.TradeId &&
           MDFlags == other.MDFlags && MDFlags2 == other.MDFlags2 &&
           MDEntryType == other.MDEntryType;
  }
};

struct OrderBookSnapShot {
  schema::structs::SBEHeader S;
  std::int32_t SecurityId;
  std::uint32_t LastMsgSeqNumProcessed;
  std::uint32_t RptSeq;
  std::uint32_t ExchangeTradingSessionId;
  schema::structs::groupSize NoMDEntries;
  std::vector<SnapShotEntry> Entries;

  bool operator==(const OrderBookSnapShot &other) const {
    return S == other.S &&
           LastMsgSeqNumProcessed == other.LastMsgSeqNumProcessed &&
           RptSeq == other.RptSeq &&
           ExchangeTradingSessionId == other.ExchangeTradingSessionId &&
           NoMDEntries == other.NoMDEntries && Entries == other.Entries;
  }
};

} // namespace simba::messages::application_layer