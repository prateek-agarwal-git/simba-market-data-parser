#pragma once
#include "application_layer_messages.h"
#include "schema/structs.h"
#include <functional>
#include <sstream>
#include <string>
#include <string_view>
namespace simba {
struct JsonCreator {
  using WriterCallback = std::function<void(const std::string &)>;
  JsonCreator(WriterCallback &&cb);
  void operator()(const simba::messages::application_layer::BestPrices &);
  void operator()(const simba::messages::application_layer::OrderUpdate &);
  void operator()(const simba::messages::application_layer::OrderExecution &);
  void
  operator()(const simba::messages::application_layer::OrderBookSnapShot &);

private:
  std::string get_json_string(const schema::structs::SBEHeader &);
  std::string get_json_string(const schema::structs::groupSize &);
  std::string
  get_json_string(const messages::application_layer::BestPricesEntry &);

  std::string
  get_json_string(const messages::application_layer::SnapShotEntry &);
  static std::string add_key(const std::string &field_name) {
    return double_quote + field_name + double_quote + colon;
  }
  template <typename T>
  static std::string add_numeric_value(T value, bool is_last = false) {
    return std::to_string(value) + (is_last ? "" : comma);
  }

  static std::string add_numeric_value(schema::types::Decimal5 value,
                                bool is_last = false) {
    return std::to_string(value.value()) + (is_last ? "" : comma);
  }

  template <typename T>
  std::string add_optional_record(const std::string &key, T optional) {
    if (!optional.has_value())
      return "";
    std::string ans;
    ans += add_key("key");
    ans += add_numeric_value(optional.value());
    return ans;
  }
  static inline const std::string colon = ": ";
  static inline const std::string comma = ", ";
  static inline const std::string double_quote = "\"";
  static inline const std::string start_brace = "{";
  static inline const std::string end_brace = "}";
  static inline const std::string start_array = "[";
  static inline const std::string end_array = "]";

  WriterCallback cb_;
};
} // namespace simba