#include <cstdint>
namespace simba::schema::bitmasks {

enum class MDFlagsSet : std::uint64_t {
  DAY = (1 << 0),
  IOC = (1 << 1),
  NonQuote = (1 << 2),
  EndOfTransaction = 1 << 12,
  DueToCrossCancel = 1 << 13,
  SecondLeg = 1 << 14,
  FOK = 1ULL << 19,
  Replace = 1ULL << 20,
  Cancel = 1ULL << 21,
  MassCancel = 1ULL << 22,
  Negotiated = 1ULL << 26,
  MultiLeg = 1ULL << 27,
  CrossTrade = 1ULL << 29,
  COD = 1ULL << 32,
  ActiveSide = 1ULL << 41,
  PassiveSide = 1ULL << 42,
  Syntheic = 1ULL << 45,
  RFS = 1ULL << 46,
  SyntheticPassive = 1ULL << 57,
  BOC = 1ULL << 60,
  DuringDiscreteAuction = 1ULL << 62
};

enum class MDFlags2Set : std::uint64_t { Zero = 1 << 0 };

enum class FlagsSet : std::uint64_t {
  EveningOrMorningSession = 1 << 0,
  AnnonymousTrading = 1 << 4,
  PrivateTrading = 1 << 5,
  DaySession = 1 << 6,
  // name collision for multileg may be use enum class
  Multileg = 1 << 8,
  Collateral = 1 << 18,
  IntradayExercise = 1 << 19
};
enum class MsgFlagsSet : std::uint16_t {
  LastFragment = 1 << 0,
  StartOfSnapShot = 1 << 1,
  EndOfSnapShot = 1 << 2,
  IncrementalPacket = 1 << 3,
  PossDupFlag = 1 << 4
};

} // namespace simba_schema::bitmasks