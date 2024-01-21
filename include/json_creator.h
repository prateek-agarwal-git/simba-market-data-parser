#pragma once
#include "application_layer_messages.h"
#include "schema/structs.h"
#include <functional>
#include <string>
#include <sstream>
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
  void write(std::ostringstream& ,const schema::structs::SBEHeader& );
  WriterCallback cb_;
};
}