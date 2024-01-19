
#include <cstdint>
namespace simba::schema::structs {
struct MarketDataPacketHeader {
  std::uint32_t MsgSeqNum;
  std::uint16_t MsgSize;
  std::uint16_t MsgFlags;
  std::uint64_t SendingTime;
} __attribute__((packed));

struct IncrementalPacketHeader {
  std::uint64_t TransactTime;
  std::uint32_t ExchangeTradingSessionId;
} __attribute__((packed));

struct SBEHeader {
  std::uint16_t BlockLength;
  std::uint16_t TemplateId;
  std::uint16_t SchemaId;
  std::uint16_t Version;
} __attribute__((packed));

struct RepeatingGroupDimensions {
  std::uint16_t blockLength;
  std::uint8_t numInGroup;
} __attribute__((packed));
} // namespace simba_schema::structs