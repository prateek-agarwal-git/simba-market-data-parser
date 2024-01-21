#pragma once
#include "application_layer_messages.h"
#include "schema/structs.h"
#include <functional>
#include <string>
#include <sstream>
#include <string_view>
namespace simba{
struct JsonCreator {
  using WriterCallback = std::function<void(const std::string&)>;
  JsonCreator(WriterCallback &&cb);
  void operator()(const simba::messages::application_layer::BestPricesEntry &);
  void operator()(const simba::messages::application_layer::BestPrices &);
  void operator()(const simba::messages::application_layer::OrderUpdate &);
  void operator()(const simba::messages::application_layer::OrderExecution &);
  void operator()(const simba::messages::application_layer::OrderBookSnapShot &);
  void operator()(const simba::messages::application_layer::SnapShotEntry &);
private:
  std::string get_json_string(const schema::structs::SBEHeader& );
  static inline const std::string colon = ": ";
  static inline const std::string comma = ", ";
  static inline const std::string double_quote = "\"";
  static inline const std::string start_brace= "{";
  static inline const std::string end_brace= "}";
  static inline const std::string start_array= "[";
  static inline const std::string end_array= "]";

  WriterCallback cb_;
};
}