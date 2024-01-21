#include "json_creator.h"
#include <iostream>
#include <sstream>
namespace simba {
JsonCreator::JsonCreator(WriterCallback &&cb) : cb_(std::move(cb)) {}

void JsonCreator::operator()(
    const simba::messages::application_layer::BestPricesEntry &) {}
void JsonCreator::operator()(
    const simba::messages::application_layer::BestPrices &) {}
void JsonCreator::operator()(
    const simba::messages::application_layer::OrderUpdate &order_update) {
  // start_json element
  std::ostringstream ss;
  ss <<"hello world";
  //write(ss, order_update.S);
  cb_(ss.str());
}
void JsonCreator::operator()(
    const simba::messages::application_layer::OrderExecution &) {}
void JsonCreator::operator()(
    const simba::messages::application_layer::OrderBookSnapShot &) {}

void JsonCreator::operator()(
    const simba::messages::application_layer::SnapShotEntry &) {}
void JsonCreator::write(std::ostringstream &ss,
                       const schema::structs::SBEHeader &S) {

                        
                       }
} // namespace simba