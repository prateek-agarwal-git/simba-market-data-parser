#include "application_layer_messages.h"

struct JsonWriter {

  std::string
  operator()(const simba::messages::application_layer::BestPricesEntry &);
  std::string
  operator()(const simba::messages::application_layer::BestPrices &);
  std::string
  operator()(const simba::messages::application_layer::OrderUpdate &);
  std::string
  operator()(const simba::messages::application_layer::OrderExecution &);
  std::string
  operator()(const simba::messages::application_layer::OrderBookSnapShot&);

  std::string
  operator()(const simba::messages::application_layer::SnapShotEntry&);
};