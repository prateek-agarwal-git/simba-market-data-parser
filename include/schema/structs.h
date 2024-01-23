#pragma once
#include <cstdint>
#include <cstring>
#include <iostream>
namespace simba::schema::structs {
struct MarketDataPacketHeader {
  std::uint32_t MsgSeqNum;
  std::uint16_t MsgSize;
  std::uint16_t MsgFlags;
  std::uint64_t SendingTime;
  bool operator==(const MarketDataPacketHeader &other) const {
    return std::memcmp((const void *)this, (const void *)(&other),
                       sizeof(MarketDataPacketHeader)) == 0;
  }
} __attribute__((packed));

inline std::ostream &operator<<(std::ostream &os,
                                const MarketDataPacketHeader &mdp) {
  os << "MsgSeqNum=" << mdp.MsgSeqNum << ",MsgSize=" << mdp.MsgSize
     << ",MsgFlags=" << mdp.MsgFlags << ",SendingTime=" << mdp.SendingTime
     << std::endl;
  return os;
}

struct IncrementalPacketHeader {
  std::uint64_t TransactTime;
  std::uint32_t ExchangeTradingSessionId;
  bool operator==(const IncrementalPacketHeader &other) const {
    return std::memcmp((const void *)this, (const void *)(&other),
                       sizeof(IncrementalPacketHeader)) == 0;
  }
} __attribute__((packed));

inline std::ostream &operator<<(std::ostream &os,
                                const IncrementalPacketHeader &inc) {
  os << "TransactTime=" << inc.TransactTime
     << ",ExchangeTradingSessionId=" << inc.ExchangeTradingSessionId
     << std::endl;
  return os;
}

struct SBEHeader {
  std::uint16_t BlockLength;
  std::uint16_t TemplateId;
  std::uint16_t SchemaId;
  std::uint16_t Version;
  bool operator==(const SBEHeader &other) const {
    return std::memcmp((const void *)this, (const void *)(&other),
                       sizeof(SBEHeader)) == 0;
  }
} __attribute__((packed));

inline std::ostream &operator<<(std::ostream &os,
                                const SBEHeader &sbe) {
  os << "BlockLength=" << sbe.BlockLength
     << ",TemplateId=" <<sbe.TemplateId  << ",SchemaId=" <<sbe.SchemaId <<",Version=" << sbe.Version
     << std::endl;
  return os;
}

struct groupSize {
  std::uint16_t blockLength;
  std::uint8_t numInGroup;
  bool operator==(const SBEHeader &other) const {
    return std::memcmp((void *)this, (const void *)(&other),
                       sizeof(groupSize)) == 0;
  }
} __attribute__((packed));
} // namespace simba::schema::structs