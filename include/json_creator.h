#pragma once
#include "application_layer_messages.h"
#include "schema/structs.h"
#include "tag_structs.h"
#include <concepts>
#include <functional>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
namespace simba {
struct JsonCreator {

  using WriterCallback = std::function<void(const std::string &)>;
  JsonCreator(WriterCallback &&cb);
  void operator()(tag_structs::start_packet);
  void operator()(tag_structs::end_packet);
  void operator()(const schema::structs::MarketDataPacketHeader &);
  void operator()(const schema::structs::IncrementalPacketHeader &);
  void operator()(const messages::application_layer::BestPrices &);
  void operator()(const messages::application_layer::OrderUpdate &);
  void operator()(const messages::application_layer::OrderExecution &);
  void operator()(const messages::application_layer::OrderBookSnapShot &);

private:
  void add_element(const schema::structs::SBEHeader &);
  void add_element(const schema::structs::groupSize &);
  void add_element(const messages::application_layer::BestPricesEntry &);
  void add_element(const messages::application_layer::SnapShotEntry &);
  void end_main_element();
  void start_main_element(const std::string &key);

  template <typename T>
  void add_array(const std::string &key, const std::vector<T> &entries) {
    if (entries.empty())
      return;

    current_json_string_ += comma;
    current_json_string_ += key_element(key);
    current_json_string_ += "[";
    for (const auto &entry : entries) {
      add_element(entry);
      current_json_string_ += comma;
    }
    current_json_string_.pop_back();
    current_json_string_.pop_back();
    current_json_string_ += "]";
  }
  static std::string key_element(const std::string &field_name) {
    return double_quote + field_name + double_quote + colon;
  }
  template <typename T>
  static std::string add_numeric_value(T value, bool is_last = false) {
    return std::to_string(value) + (is_last ? "" : comma);
  }

  template <typename T>
  std::string optional_record_element(const std::string &key, T optional,
                                      bool is_last = false) {
    if (!optional.has_value())
      return "";
    return key_element(key) + add_numeric_value(optional.value(), is_last);
  }

  // Decimal types require printing with precision (5 places). So std::to_chars
  // (c++17 onwards) is used for thatused for that and a member method to_string
  // is defined. It is expected to be performant compared to other alternatives.
  template <typename T>
  requires schema::types::is_decimal_t<T> std::string
  optional_record_element(const std::string &key, T optional,
                          bool is_last = false) {
    if (!optional.has_value())
      return "";
    return key_element(key) + optional.to_string() + (is_last ? "" : comma);
  }

  template <typename T>
  requires(!schema::types::is_decimal_t<T>) std::string
      numeric_record_element(const std::string &key, T value,
                             bool is_last = false) {
    return key_element(key) + add_numeric_value(value, is_last);
  }

  template <typename T>
  requires schema::types::is_decimal_t<T> std::string
  numeric_record_element(const std::string &key, T value,
                         bool is_last = false) {
    return key_element(key) + value.to_string() + (is_last ? "" : comma);
  }

  template <typename T>
  std::string enum_record_element(const std::string &key, T val,
                                  bool is_last = false) {
    return key_element(key) + double_quote + schema::enums::to_string(val) +
           double_quote + (is_last ? "" : comma);
  }

  template <typename T>
  std::string bitmask_record_element(const std::string &key, uint64_t val,
                                     bool is_last = false) {
    return key_element(key) + double_quote +
           schema::bitmasks::to_string<T>(val) + double_quote +
           (is_last ? "" : comma);
  }

  static inline const std::string colon = ": ";
  static inline const std::string comma = ", ";
  static inline const std::string double_quote = "\"";
  static inline const std::string start_brace = "{";
  static inline const std::string end_brace = "}";
  static inline const std::string start_array = "[";
  static inline const std::string end_array = "]";

  // the below string is reused across different calls to avoid repeated
  // allocations/deallocations with stack objects. Although that approach is
  // relatively safer.
  std::string current_json_string_;

  WriterCallback cb_;
};
} // namespace simba