#pragma once
#include <charconv>
#include <concepts>
#include <cstdint>
#include <limits>
#include <optional>
#include <string>
#include <type_traits>

namespace simba::schema::types {
using Int64NULL = std::optional<int64_t>;
using uInt32NULL = std::optional<uint32_t>;
struct NullValues {
  static constexpr int64_t Int64 = std::numeric_limits<int64_t>::max();
};

struct Decimal5 {
  std::int64_t mantissa;
  static constexpr double exponent = 1e-5;

  std::string to_string() const {
    char A[32];
    auto [ptr, _] = std::to_chars(A, A + 32, mantissa* exponent,
                                  std::chars_format::fixed, 5);
    return std::string(A, ptr);
  }
};
struct Decimal5Null {
  std::optional<int64_t> mantissa;
  static constexpr double exponent = 1e-5;
  bool has_value() const { return mantissa.has_value(); }
  std::string to_string() const {
    char A[32];
    auto [ptr, _] = std::to_chars(A, A + 32, mantissa.value() * exponent,
                                  std::chars_format::fixed, 5);
    return std::string(A, ptr);
  }
};

struct Decimal2Null {
  std::optional<int64_t> mantissa;
  static constexpr double exponent = 1e-2;
  bool has_value() const { return mantissa.has_value(); }

  std::string to_string() const {
    char A[32];
    auto [ptr, _] = std::to_chars(A, A + 32, mantissa.value() * exponent,
                                  std::chars_format::fixed, 2);
    return std::string(A, ptr);
  }
};

template <typename T>
concept is_decimal_t = std::is_same_v<T, Decimal2Null> ||
    std::is_same_v<T, Decimal5> || std::is_same_v<T, Decimal5Null>;

} // namespace simba::schema::types