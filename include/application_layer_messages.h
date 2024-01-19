#include "schema_bitmasks.h"
#include "schema_enums.h"
#include "schema_structs.h"
#include "schema_types.h"
#include <cstdint>
#include <vector>

namespace simba::messages::application_layer {
// Packing required - no.
struct BestPricesEntry {
  schema::types::Decimal5Null MktBidPx;   // 645
  schema::types::Decimal5Null MktOfferPx; // 646
  schema::types::Int64NULL MktBidSize;    // 22001
  schema::types::Int64NULL MktOfferSize;  // 22002
  std::int32_t SecurityId;                // 48
};
struct BestPrices {
  schema::structs::SBEHeader S;
  schema::structs::RepeatingGroupDimensions NoMDEntries; // tag = 256
  std::vector<BestPricesEntry> Entries;
};

struct EmptyBook {
  schema::structs::SBEHeader S;
  schema::types::uInt32NULL LastMsgSeqNumProcessed; // 369
};

struct OrderUpdate {
  schema::structs::SBEHeader S;
  std::int64_t MDEntryId;                       // 278
  schema::types::Decimal5 MDentryPx;            // 270
  std::int64_t MDEntrySize;                     // 271
  schema::bitmasks::MDFlagsSet MDFlags;         // 20017
  schema::bitmasks::MDFlags2Set MDFlags2;       // 20050
  std::int32_t SecurityId;                      // 48
  std::uint32_t RptSeq;                         // 83
  schema::enums::MDUpdateAction MDUpdateAction; // 279
  schema::enums::MDEntryType MDEntryType;       // 269
};

// same struct for full or partial execution as well as on calendar spreads. The
// difference is in the values of respective fields.
struct OrderExecution {
  schema::structs::SBEHeader S;
  std::int64_t MDEntryId;                       // 278
  schema::types::Decimal5Null MDentryPx;        // 270
  schema::types::Int64NULL MDEntrySize;         // 271
  schema::types::Decimal5 LastPx;               // 31
  std::int64_t LastQty;                         // 32
  std::int64_t TradeId;                         // 1003
  schema::bitmasks::MDFlagsSet MDFlags;         // 20017
  schema::bitmasks::MDFlags2Set MDFlags2;       // 20050
  std::int32_t SecurityId;                      // 48
  std::uint32_t RptSeq;                         // 83
  schema::enums::MDUpdateAction MDUpdateAction; // 279
  schema::enums::MDEntryType MDEntryType;       // 269
};
struct SnapShotEntry {
  schema::types::Int64NULL MDEntryId;     // 278
  std::uint64_t TransactTime;             // 60
  schema::types::Decimal5Null MDEntryPx;  // 270
  schema::types::Int64NULL MDEntrySize;   // 271
  schema::types::Int64NULL TradeId;       // 1003
  schema::bitmasks::MDFlagsSet MDFlags;   // 20017
  schema::bitmasks::MDFlags2Set MDFlags2; // 20050
  schema::enums::MDEntryType MDEntryType; // 269
};

struct OrderBookSnapShot {
  schema::structs::SBEHeader S;
  std::int32_t SecurityId;                               // 48
  std::uint32_t LastMsgSeqNumProcessed;                  // 369
  std::uint32_t RptSeq;                                  // 83
  std::uint32_t ExchangeTradingSessionId;                // 5842
  schema::structs::RepeatingGroupDimensions NoMDEntries; // tag = 268
  std::vector<SnapShotEntry> Entries;
};

struct MarketDataRequest {
  schema::structs::SBEHeader S;
  std::uint32_t ApplBegSeqNum; // 1182
  std::uint32_t ApplEndSeqNum; // 1183
};
struct DiscreteAuction {
  schema::structs::SBEHeader S;
  std::uint64_t TradSesOpenTime;                           // 342
  std::uint64_t TradSesCloseTimeFrom;                      // 20046
  std::uint64_t TradSesCloseTimeTill;                      // 20047
  std::int64_t AuctionId;                                  // 21002
  std::int32_t ExchangeTradingSessionId;                   // 5842
  std::int32_t EventIDOpen;                                // 20048
  std::int32_t EventIDClose;                               // 20049
  schema::structs::RepeatingGroupDimensions NoUnderlyings; //  711
  std::vector<schema::types::VarString> UnderyingSymbols;
};

struct SecurityMassStatusEntries {
  std::int32_t SecurityId; // 48
  // security id source;
  schema::enums::SecurityTradingStatus SecurityTradingStatus;
};
struct SecurityMassStatus {
  schema::structs::SBEHeader S;
  schema::types::GroupSize2 NoRelatedSymbols; // 146
};
} // namespace simba::messages::application_layer