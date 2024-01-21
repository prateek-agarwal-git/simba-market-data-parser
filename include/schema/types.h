#include <cstdint>
#include <optional>
#include <string>
#include <limits>
#include <type_traits>
#include <concepts>

namespace simba::schema::types {
using Int64NULL = std::optional<int64_t>;
using uInt32NULL = std::optional<uint32_t>;
struct NullValues{
  static constexpr int64_t Int64 = std::numeric_limits<int64_t> ::max();
};

struct Decimal5 {
  std::int64_t mantissa;
  static constexpr double exponent = 1e-5;
  
  std::string to_string() const{
    return "";
  }
};
struct Decimal5Null {
  std::optional<int64_t> mantissa;
  static constexpr double exponent = 1e-5;
  bool has_value()const{
    return mantissa.has_value();

  }
  
  std::string to_string() const{
    return "";
  }
};

struct Decimal2Null {
  std::optional<int64_t> mantissa;
  static constexpr double exponent = 1e-2;
  bool has_value()const{
    return mantissa.has_value();
  }
  
  std::string to_string() const{
    return "";
  }
};


template <typename T>
concept is_decimal_t = std::is_same_v<T, Decimal2Null> ||std::is_same_v<T, Decimal5> || std::is_same_v <T, Decimal5Null >;


} // namespace simba::schema::types