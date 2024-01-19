#include <cstdint>

namespace simba::messages::message_types {
enum class SessionLayer : std::uint16_t {
  Logon = 1000,
  Logout = 1001,
  HeartBeat = 1,
  SequenceReset = 2
};

enum class ApplicationLayer : std::uint16_t {
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

}; // namespace simba::messages::message_types