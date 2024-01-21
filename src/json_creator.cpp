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
  std::string ans;
  ans+= start_brace+"\"OrderUpdate\"" + colon;
  ans += get_json_string(order_update.S);
  ans+=end_brace;
  cb_(ans);
}
void JsonCreator::operator()(
    const simba::messages::application_layer::OrderExecution &) {}
void JsonCreator::operator()(
    const simba::messages::application_layer::OrderBookSnapShot &) {}

void JsonCreator::operator()(
    const simba::messages::application_layer::SnapShotEntry &) {}
std::string JsonCreator::get_json_string(const schema::structs::SBEHeader &S) {
  return start_brace + "\"SBEHeader\"" + colon + start_brace +
         "\"BlockLength\"" + colon + std::to_string(S.BlockLength) + comma +
         "\"TemplateId\"" + colon + std::to_string(S.TemplateId) + comma +
         "\"SchemaId\"" + colon + std::to_string(S.SchemaId) + comma +
         "\"Version\"" + colon + std::to_string(S.Version) + end_brace +
         end_brace;
}
} // namespace simba