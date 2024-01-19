#include <cstdint>
namespace simba::schema::bitmasks {

enum MDFlagsSet : std::uint64_t {
  DAY = (1 << 0),
  IOC = (1 << 1),
  NonQuote = (1 << 2),
  EndOfTransaction = 1 << 12,
  DueToCrossCancel = 1 << 13,
  SecondLeg = 1 << 14,
  FOK = 1 << 19,
  Replace = 1 << 20,
  Cancel = 1 << 21,
  MassCancel = 1 << 22,
  Negotiated = 1 << 26,
  MultiLeg = 1 << 27,
  CrossTrade = 1 << 29,
  COD = 1 << 32,
  ActiveSide = 1 << 41,
  PassiveSide = 1 << 42,
  Syntheic = 1 << 45,
  RFS = 1 << 46,
  SyntheticPassive = 1 << 57,
  BOC = 1 << 60,
  DuringDiscreteAuction = 1 << 62
};

enum MDFlags2Set : std::uint64_t { Zero = 1 << 0 };

enum FlagsSet : std::uint64_t {
  EveningOrMorningSession = 1 << 0,
  AnnonymousTrading = 1 << 4,
  PrivateTrading = 1 << 5,
  DaySession = 1 << 6,
  // name collision for multileg may be use enum class
  Multileg = 1 << 8,
  Collateral = 1 << 18,
  IntradayExercise = 1 << 19
};
enum MsgFlagsSet : std::uint16_t {
  LastFragment = 1 << 0,
  StartOfSnapShot = 1 << 1,
  EndOfSnapShot = 1 << 2,
  IncrementalPacket = 1 << 3,
  PossDupFlag = 1 << 4
};

} // namespace simba_schema::bitmasks