#include <cstdint>

namespace simba::schema::enums {

enum class MDUpdateAction : std::uint8_t { New = 0, Change = 1, Delete = 2 };
enum MDEntryType : char {
  // verify this
  Bid = '0',
  Offer = '1',
  EmptyBook = 'J'

};

enum SecurityAltIdSource : char { ISIN = '4', ExchangeSymbol = '8' };

enum SecurityTradingStatus : std::uint8_t {
  // schema type is uint8_t NULL
  TradingHalt = 2,
  ReadyToTrade = 17,
  NotAvailableForTrading = 18,
  NotTradedonThisMarket = 19,
  UnknownOrInvalid = 20,
  PreOpen = 21,
  DiscreteAuctionOpen = 119,
  DiscreteAuctionClose = 121,
  InstrumentHalt = 122
};

enum TradingSessionId : std::uint8_t {
  // schema type is uint8_t NULL
  Day = 1,
  Morning = 3,
  Evening = 5
};

enum MarketSegmentId : char { Derivatives = 'D' };

enum TradeSesStatus : std::uint8_t {
  Halted = 1,
  Open = 2,
  Closed = 3,
  PreOpen = 4
};

enum TradeSesEvent :std::uint8_t{
  // schema type is uint8_t NULL
  TradingResumes = 0,
  ChangeOfTradingSession = 1,
  ChangeOfTradingStatus = 3
};

enum NegativePrices : std::uint8_t { NotEligibile = 0, Eligible = 1 };

} // namespace simba_schema