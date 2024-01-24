#pragma once
#include "schema/bitmasks.h"
#include "schema/enums.h"
#include "schema/structs.h"
#include "schema/types.h"
#include <cstdint>
#include <iostream>
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
           MktBidSize == other.MktBidSize &&
           MktOfferSize == other.MktOfferSize && SecurityId == other.SecurityId;
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
} __attribute__((packed)); // 58 bytes kept packed for ease of use and all the
                           // members are trivially copyable

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
    return MDEntryId == other.MDEntryId && TransactTime == other.TransactTime &&
           MDEntryPx == other.MDEntryPx && MDEntrySize == other.MDEntrySize &&
           TradeId == other.TradeId && MDFlags == other.MDFlags &&
           MDFlags2 == other.MDFlags2 && MDEntryType == other.MDEntryType;
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
inline std::ostream &operator<<(std::ostream &os, const BestPricesEntry &bpe) {
  os << "BestPricesEntry: ";
  if (bpe.MktBidPx.has_value()) {
    os << "MktBidPx=" << bpe.MktBidPx.to_string();
  }
  if (bpe.MktOfferPx.has_value()) {
    os << "MktOfferPx=" << bpe.MktOfferPx.to_string();
  }

  if (bpe.MktBidSize.has_value())
    os << "MktBidSize=" << *bpe.MktBidSize;
  if (bpe.MktOfferSize.has_value()) {
    os << "MktOfferSize=" << *bpe.MktOfferSize;
  }
  os << "SecurityId=" << bpe.SecurityId;
  return os;
}

inline std::ostream &operator<<(std::ostream &os, const BestPrices &bp) {
  os << "BestPrices: " << bp.S << "," << bp.NoMDEntries;
  for (const auto &entry : bp.Entries) {
    os << entry;
  }
  return os;
}

inline std::ostream &operator<<(std::ostream &os,
                                const OrderUpdate &order_update) {
  os << "OrderUpdate: ";
  os << order_update.S;
  os << ",MDEntryId=" << order_update.MDEntryId;
  os << ",MDEntryPx=" << order_update.MDEntryPx.to_string();
  os << ",MDEntrySize=" << order_update.MDEntrySize;
  os << ",MDEntryFlags="
     << schema::bitmasks::to_string<schema::bitmasks::MDFlagsSet>(
            order_update.MDFlags);
  os << ",MDEntryFlags2="
     << schema::bitmasks::to_string<schema::bitmasks::MDFlags2Set>(
            order_update.MDFlags2);
  os << ",SecurityId=" << order_update.SecurityId;
  os << ",RptSeq=" << order_update.RptSeq;
  os << ",MDUpdateAction=" << to_string(order_update.MDUpdateAction);
  os << ",MDEntryType=" << to_string(order_update.MDEntryType);
  return os;
}
inline std::ostream &operator<<(std::ostream &os,
                                const OrderExecution &order_execution) {
  os << "OrderExecution: ";
  os << order_execution.S;
  os << ",MDEntryId=" << order_execution.MDEntryId;
  if (order_execution.MDEntryPx.has_value()) {
    os << ",MDEntryPx=" << order_execution.MDEntryPx.to_string();
  }
  if (order_execution.MDEntrySize.has_value()) {
    os << ",MDEntrySize=" << order_execution.MDEntrySize.value();
  }
  os << ",LastPx=" << order_execution.LastPx.to_string();
  os << ",LastQty=" << order_execution.LastQty;
  os << ",TradeId=" << order_execution.TradeId;

  os << ",MDEntryFlags="
     << schema::bitmasks::to_string<schema::bitmasks::MDFlagsSet>(
            order_execution.MDFlags);
  os << ",MDEntryFlags2="
     << schema::bitmasks::to_string<schema::bitmasks::MDFlags2Set>(
            order_execution.MDFlags2);
  os << ",SecurityId=" << order_execution.SecurityId;
  os << ",RptSeq=" << order_execution.RptSeq;
  os << ",MDUpdateAction=" << to_string(order_execution.MDUpdateAction);
  os << ",MDEntryType=" << to_string(order_execution.MDEntryType);
  return os;
}

inline std::ostream &operator<<(std::ostream &os, const SnapShotEntry &s) {
  os << "SnapshotEntry: ";
  if (s.MDEntryId.has_value()) {
    os << "MDEntryId=" << s.MDEntryId.value();
  }
  os << ",TransactTime=" << s.TransactTime;
  if (s.MDEntryPx.has_value()) {
    os << ",MDEntryPx=" << s.MDEntryPx.to_string();
  }
  if (s.MDEntrySize.has_value()) {
    os << ",MDEntrySize=" << s.MDEntrySize.value();
  }
  if (s.TradeId.has_value()) {
    os << ",TradeId=" << s.TradeId.value();
  }
  os << ",MDFlags="
     << simba::schema::bitmasks::to_string<schema::bitmasks::MDFlagsSet>(
            s.MDFlags);
  os << ",MDFlags2="
     << simba::schema::bitmasks::to_string<schema::bitmasks::MDFlags2Set>(
            s.MDFlags);
  os << ",MDEntryType=" << schema::enums::to_string(s.MDEntryType);
  return os;
}

inline std::ostream &operator<<(std::ostream &os,
                                const OrderBookSnapShot &obs) {
  os << "OrderBookSnapshot: ";
  os << obs.S;
  os << ",SecurityId=" << obs.SecurityId;
  os << ",LastMsgSeqNumProcessed=" << obs.LastMsgSeqNumProcessed;
  os << ",RptSeq=" << obs.RptSeq;
  os << ",ExchangeTradingSessionId=" << obs.ExchangeTradingSessionId;
  os << ",NoMDEntries=" << obs.NoMDEntries;
  for (const auto &entry : obs.Entries) {
    os << entry;
  }
  return os;
}

} // namespace simba::messages::application_layer