#include "schema/bitmasks.h"
#include "schema/enums.h"
#include "schema/structs.h"
#include "schema/types.h"
#include <cstdint>
#include <vector>

namespace simba::messages::application_layer {
enum class MessageTypes : std::uint16_t {
  BestPrices = 14,
  EmptyBook = 4,
  OrderUpdate = 15,
  OrderExecution = 16,
  OrderBookSnapShot = 17,
  SecurityDefinition = 18,
  SecurityStatus = 9,
  SecurityDefintionUpdateReport = 10,
  TradingSessionStatus = 11,
  MarketDataRequest = 1002,
  DiscreteAuction = 13,
  SecurityMassStatus = 19
};

struct BestPricesEntry {
  schema::types::Decimal5Null MktBidPx;
  schema::types::Decimal5Null MktOfferPx;
  schema::types::Int64NULL MktBidSize;
  schema::types::Int64NULL MktOfferSize;
  std::int32_t SecurityId;
};
struct BestPrices {
  schema::structs::SBEHeader S;
  schema::structs::RepeatingGroupDimensions NoMDEntries;
  std::vector<BestPricesEntry> Entries;
};

struct EmptyBook {
  schema::structs::SBEHeader S;
  schema::types::uInt32NULL LastMsgSeqNumProcessed;
};

struct OrderUpdate {
  schema::structs::SBEHeader S;
  std::int64_t MDEntryId;
  schema::types::Decimal5 MDentryPx;
  std::int64_t MDEntrySize;
  schema::bitmasks::MDFlagsSet MDFlags;
  schema::bitmasks::MDFlags2Set MDFlags2;
  std::int32_t SecurityId;
  std::uint32_t RptSeq;
  schema::enums::MDUpdateAction MDUpdateAction;
  schema::enums::MDEntryType MDEntryType;
}; // 58 bytes

// same struct for full or partial execution as well as on calendar spreads.
// The / difference is in the values of respective fields.
struct OrderExecution {
  schema::structs::SBEHeader S;
  std::int64_t MDEntryId;
  schema::types::Decimal5Null MDentryPx;
  schema::types::Int64NULL MDEntrySize;
  schema::types::Decimal5 LastPx;
  std::int64_t LastQty;
  std::int64_t TradeId;
  schema::bitmasks::MDFlagsSet MDFlags;
  schema::bitmasks::MDFlags2Set MDFlags2;
  std::int32_t SecurityId;
  std::uint32_t RptSeq;
  schema::enums::MDUpdateAction MDUpdateAction;
  schema::enums::MDEntryType MDEntryType;
};

struct SnapShotEntry {
  schema::types::Int64NULL MDEntryId;
  std::uint64_t TransactTime;
  schema::types::Decimal5Null MDEntryPx;
  schema::types::Int64NULL MDEntrySize;
  schema::types::Int64NULL TradeId;
  schema::bitmasks::MDFlagsSet MDFlags;
  schema::bitmasks::MDFlags2Set MDFlags2;
  schema::enums::MDEntryType MDEntryType;
};

struct OrderBookSnapShot {
  schema::structs::SBEHeader S;
  std::int32_t SecurityId;
  std::uint32_t LastMsgSeqNumProcessed;
  std::uint32_t RptSeq;
  std::uint32_t ExchangeTradingSessionId;
  schema::structs::RepeatingGroupDimensions NoMDEntries;
  std::vector<SnapShotEntry> Entries;
};

} // namespace simba::messages::application_layer