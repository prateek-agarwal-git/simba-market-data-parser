#include <cstdint>
#include <optional>
#include <string>
#include <limits>

namespace simba::schema::types {
using Int64NULL = std::optional<int64_t>;
using uInt32NULL = std::optional<uint32_t>;
struct NullValues{
  static constexpr int64_t Int64 = std::numeric_limits<int64_t> ::max();
};

struct Decimal5 {
  std::int64_t mantissa;
  static constexpr double exponent = 1e-5;
  double value() const{
    return exponent* mantissa;
  }
};
struct Decimal5Null {
  std::optional<int64_t> mantissa;
  static constexpr double exponent = 1e-5;
  bool has_value()const{
    return mantissa.has_value();

  }
  double value() const{
    return exponent* mantissa.value();
  }
};

struct Decimal2Null {
  std::optional<int64_t> mantissa;
  static constexpr double exponent = 1e-2;
  bool has_value()const{
    return mantissa.has_value();
  }
  double value() const{
    return exponent* mantissa.value();
  }
};

} // namespace simba::schema::types