#pragma once
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>
namespace simba::schema::bitmasks {

enum class MDFlagsSet : std::uint64_t {
  DAY = 1,
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
  Synthetic = 1ULL << 45,
  RFS = 1ULL << 46,
  SyntheticPassive = 1ULL << 57,
  BOC = 1ULL << 60,
  DuringDiscreteAuction = 1ULL << 62
};

inline std::string to_string(MDFlagsSet flag) {
  switch (flag) {
  case MDFlagsSet::DAY:
    return "DAY";
  case MDFlagsSet::IOC:
    return "IOC";
  case MDFlagsSet::NonQuote:
    return "NonQuote";
  case MDFlagsSet::EndOfTransaction:
    return "EndOfTransaction";
  case MDFlagsSet::DueToCrossCancel:
    return "DueToCrossCancel";
  case MDFlagsSet::SecondLeg:
    return "SecondLeg";
  case MDFlagsSet::FOK:
    return "FOK";
  case MDFlagsSet::Replace:
    return "Replace";
  case MDFlagsSet::Cancel:
    return "Cancel";
  case MDFlagsSet::MassCancel:
    return "MassCancel";
  case MDFlagsSet::Negotiated:
    return "Negotiated";
  case MDFlagsSet::MultiLeg:
    return "Multileg";
  case MDFlagsSet::CrossTrade:
    return "CrossTrade";
  case MDFlagsSet::COD:
    return "COD";
  case MDFlagsSet::ActiveSide:
    return "ActiveSide";
  case MDFlagsSet::PassiveSide:
    return "PassiveSide";
  case MDFlagsSet::Synthetic:
    return "Synthetic";
  case MDFlagsSet::RFS:
    return "RFS";
  case MDFlagsSet::SyntheticPassive:
    return "SyntheticPassive";
  case MDFlagsSet::BOC:
    return "BOC";
  case MDFlagsSet::DuringDiscreteAuction:
    return "DuringDiscreteAuction";
  default:
    return "";
  }
  std::abort();
}

enum MDFlags2Set : std::uint64_t { Zero = 1 };

inline std::string to_string(MDFlags2Set flag) {
  switch (flag) {
  case MDFlags2Set::Zero:
    return "Zero";
  }
  std::abort();
}

template <typename T> std::string to_string(uint64_t);

template <> inline std::string to_string<MDFlags2Set>(uint64_t val) {
  std::vector<MDFlags2Set> v{MDFlags2Set::Zero};
  for (auto a : v) {
    if (val & static_cast<uint64_t>(a)) {
      return to_string(a);
    }
  }
  return "";
}

template <> inline std::string to_string<MDFlagsSet>(uint64_t val) {
  std::vector<MDFlagsSet> v{MDFlagsSet::DAY,
                            MDFlagsSet::IOC,
                            MDFlagsSet::NonQuote,
                            MDFlagsSet::EndOfTransaction,
                            MDFlagsSet::DueToCrossCancel,
                            MDFlagsSet::SecondLeg,
                            MDFlagsSet::FOK,
                            MDFlagsSet::Replace,
                            MDFlagsSet::Cancel,
                            MDFlagsSet::MassCancel,
                            MDFlagsSet::Negotiated,
                            MDFlagsSet::MultiLeg,
                            MDFlagsSet::CrossTrade,
                            MDFlagsSet::COD,
                            MDFlagsSet::ActiveSide,
                            MDFlagsSet::PassiveSide,
                            MDFlagsSet::Synthetic,
                            MDFlagsSet::RFS,
                            MDFlagsSet::SyntheticPassive,
                            MDFlagsSet::BOC,
                            MDFlagsSet::DuringDiscreteAuction};
  std::string ans;
  for (auto a : v) {
    if (val & static_cast<uint64_t>(a)) {
      ans += to_string(a) + "|";
    }
  }
  if (!ans.empty()) ans.pop_back();
  return ans;
}

} // namespace simba::schema::bitmasks