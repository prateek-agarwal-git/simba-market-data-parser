#pragma once
#include "application_layer_messages.h"
#include "schema/structs.h"
#include <concepts>
#include <functional>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
namespace simba {
struct start_packet {};
struct end_packet {};
struct JsonCreator {

  using WriterCallback = std::function<void(const std::string &)>;
  JsonCreator(WriterCallback &&cb);
  void operator()(start_packet);
  void operator()(end_packet);
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
    current_json_string_ += add_key(key);
    current_json_string_ += "[";
    for (const auto &entry : entries) {
      add_element(entry);
      current_json_string_ += comma;
    }
    current_json_string_.pop_back();
    current_json_string_.pop_back();
    current_json_string_ += "]";
  }
  static std::string add_key(const std::string &field_name) {
    return double_quote + field_name + double_quote + colon;
  }
  template <typename T>
  static std::string add_numeric_value(T value, bool is_last = false) {
    return std::to_string(value) + (is_last ? "" : comma);
  }

  template <typename T>
  std::string add_optional_record(const std::string &key, T optional) {
    if (!optional.has_value())
      return "";
    return add_key(key) + add_numeric_value(optional.value());
  }
  template <typename T>
  requires schema::types::is_decimal_t<T> std::string
  add_optional_record(const std::string &key, T optional) {
    if (!optional.has_value())
      return "";
    return add_key(key) + optional.to_string();
  }

  template <typename T>
  requires(!schema::types::is_decimal_t<T>) std::string
      add_numeric_record(const std::string &key, T value,
                         bool is_last = false) {
    return add_key(key) + add_numeric_value(value, is_last);
  }

  template <typename T>
  requires schema::types::is_decimal_t<T> std::string
  add_numeric_record(const std::string &key, T value, bool is_last = false) {
    return add_key(key) + value.to_string() + (is_last ? "" : comma);
  }

  template <typename T>
  std::string add_enum_record(const std::string &key, T val,
                              bool is_last = false) {
    return add_key(key) + double_quote + schema::enums::to_string(val) +
           double_quote + (is_last ? "" : comma);
  }

  template <typename T>
  std::string add_bitmask_record(const std::string &key, uint64_t val,
                                 bool is_last = false) {
    return add_key(key) + double_quote + schema::bitmasks::to_string<T>(val) +
           double_quote + (is_last ? "" : comma);
  }

  static inline const std::string colon = ": ";
  static inline const std::string comma = ", ";
  static inline const std::string double_quote = "\"";
  static inline const std::string start_brace = "{";
  static inline const std::string end_brace = "}";
  static inline const std::string start_array = "[";
  static inline const std::string end_array = "]";
  std::string current_json_string_;

  WriterCallback cb_;
};
} // namespace simba