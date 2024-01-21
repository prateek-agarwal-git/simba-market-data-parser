#include <cstdint>
#include <optional>
#include <string>
#include <limits>

namespace simba::schema::types {
using String3 = char[3];
using String4 = char[4];
using String6 = char[6];
using String25 = char[25];
using String31 = char[31];
using String256 = char[256];
using Int64NULL = std::optional<int64_t>;
using uInt32NULL = std::optional<uint32_t>;
struct NullValues{
  static constexpr int64_t Int64 = std::numeric_limits<int64_t> ::max();
};

struct Decimal5 {
  std::int64_t mantissa;
  static constexpr std::int8_t exponent = -5;
};
struct Decimal5Null {
  // TODO save null value somewhere
  std::optional<int64_t> mantissa;
  static constexpr std::int8_t exponent = -5;
};

struct Decimal2Null {
  std::optional<int64_t> mantissa;
  static constexpr std::int8_t exponent = -2;
};
struct VarString {
  uint16_t length;
  std::string s;
};
struct GroupSize2{
std::uint16_t blockLength;
std::uint16_t numInGroup;
};
} // namespace simba::schema::types