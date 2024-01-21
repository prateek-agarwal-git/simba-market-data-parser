#include "json_creator.h"
#include <iostream>
#include <sstream>
namespace simba {
JsonCreator::JsonCreator(WriterCallback &&cb) : cb_(std::move(cb)) {}
void JsonCreator::operator()(
    const simba::messages::application_layer::BestPrices &best_prices) {
  std::string ans;
  ans += start_brace;
  ans += add_key("BestPrices");
  ans += start_brace;
  ans += get_json_string(best_prices.S);
  ans += comma;
  ans += get_json_string(best_prices.NoMDEntries);
  for (const auto &entry : best_prices.Entries) {
    auto entry_json = get_json_string(entry);
    // add array specific delimiters here
    ans += entry_json;
  }
}
void JsonCreator::operator()(
    const simba::messages::application_layer::OrderUpdate &order_update) {
  std::string ans;
  ans += start_brace;
  ans += add_key("OrderUpdate");
  ans += start_brace;
  ans += get_json_string(order_update.S);
  ans += comma;
  ans += add_key("MDEntryId");
  ans += add_numeric_value(order_update.MDEntryId);
  // ans += add_key("MDEntryPx");
  // ans += add_numeric_value(order_update.MDEntryPx);
  // ans += add_key("MDFlags");
  // ans += add_numeric_value(order_update.MDFlags);
  // ans += add_key("MDFlags2");
  // ans += add_numeric_value(order_update.MDFlags2);
  ans += add_key("SecurityId");
  ans += add_numeric_value(order_update.SecurityId);
  ans += add_key("RptSeq");
  ans += add_numeric_value(order_update.RptSeq);
  // ans += add_key("MDUpdateAction");
  // ans += add_numeric_value(order_update.MDUpdateAction);
  // ans += add_key("MDEntryType");
  // ans += add_numeric_value(order_update.MDEntryType);
  ans += end_brace;
  ans += end_brace;
  cb_(ans);
}
void JsonCreator::operator()(
    const simba::messages::application_layer::OrderExecution &order_execution) {
  std::string ans;
  ans += start_brace;
  ans += add_key("OrderExecution");
  ans += start_brace;
  ans += get_json_string(order_execution.S);
  ans += comma;
  ans += add_key("MDEntryId");
  ans += add_numeric_value(order_execution.MDEntryId);
  ans += add_optional_record("MDEntryPx", order_execution.MDEntryPx);
  ans += add_optional_record("MDEntrySize", order_execution.MDEntrySize);
  ans += add_key("LastPx");
  ans += add_numeric_value(order_execution.MDEntrySize.value());
  ans += add_key("LastQty");
  ans += add_numeric_value(order_execution.LastQty);
  ans += add_key("TradeId");
  ans += add_numeric_value(order_execution.TradeId);

  //  ans += add_key("MDFlags");
  //  ans += add_numeric_value(order_execution.MDFlags);
  //
  //  ans += add_key("MDFlags2");
  //  ans += add_numeric_value(order_execution.MDFlags2);

  ans += add_key("SecurityId");
  ans += add_numeric_value(order_execution.SecurityId);

  ans += add_key("RptSeq");
  ans += add_numeric_value(order_execution.RptSeq);
  // ans += add_key("MDUpdateAction");
  // ans += add_numeric_value(order_update.MDUpdateAction);
  // ans += add_key("MDEntryType");
  // ans += add_numeric_value(order_update.MDEntryType);

  ans += end_brace;
  ans += end_brace;
  cb_(ans);
}
void JsonCreator::operator()(
    const simba::messages::application_layer::OrderBookSnapShot
        &order_snapshot) {
  std::string ans;
  ans += start_brace;
  ans += add_key("OrderBookSnapShot");
  ans += start_brace;
  ans += get_json_string(order_snapshot.S);
  ans += comma;
  ans += add_key("SecurityId");
  ans += add_numeric_value(order_snapshot.SecurityId);

  ans += add_key("LastMsgSeqNumProcessed");
  ans += add_numeric_value(order_snapshot.LastMsgSeqNumProcessed);
  ans += add_key("RptSeq");
  ans += add_numeric_value(order_snapshot.RptSeq);
  ans += add_key("ExchangeTradingSessionId");
  ans += add_numeric_value(order_snapshot.ExchangeTradingSessionId);
  ans += get_json_string(order_snapshot.NoMDEntries);
  for (const auto &entry : order_snapshot.Entries) {
    auto entry_json = get_json_string(entry);
    // add array specific delimiters here
    ans += entry_json;
  }
}

std::string JsonCreator::get_json_string(const schema::structs::SBEHeader &S) {
  std::string ans;
  ans += add_key("SBEHeader");
  ans += start_brace;
  ans += add_key("BlockLength");
  ans += add_numeric_value(S.BlockLength);
  ans += add_key("TemplateId");
  ans += add_numeric_value(S.TemplateId);
  ans += add_key("SchemaId");
  ans += add_numeric_value(S.SchemaId);
  ans += add_key("Version");
  ans += add_numeric_value(S.Version, true /*is_last*/);
  ans += end_brace;
  return ans;
}

std::string JsonCreator::get_json_string(const schema::structs::groupSize &G) {
  std::string ans;
  ans += add_key("NoMDEntries");
  ans += start_brace;
  ans += add_key("blockLength");
  ans += add_numeric_value(G.blockLength);
  ans += add_key("numInGroup");
  ans += add_numeric_value(G.numInGroup);
  ans += end_brace;
  return ans;
}

std::string JsonCreator::get_json_string(
    const messages::application_layer::BestPricesEntry &entry) {
  std::string ans;
  ans += add_key("BestPricesEntry");
  ans += start_brace;
  ans += add_optional_record("MktBidPx", entry.MktBidPx);
  ans += add_optional_record("MktOfferPx", entry.MktOfferPx);
  ans += add_optional_record("MktBidSize", entry.MktBidSize);
  ans += add_optional_record("MktOfferSize", entry.MktOfferSize);
  ans += add_key("SecurityId");
  ans += add_numeric_value(entry.SecurityId);
  ans += end_brace;
  return ans;
}

std::string JsonCreator::get_json_string(
    const messages::application_layer::SnapShotEntry &entry) {
  std::string ans;
  ans += add_key("SnapShotEntry");
  ans += start_brace;
  ans += add_optional_record("MDEntryId", entry.MDEntryId);
  ans += add_key("TransactTime");
  ans += add_numeric_value(entry.TransactTime);
  ans += add_optional_record("MDEntryPx", entry.MDEntryPx);
  ans += add_optional_record("MDEntrySize", entry.MDEntrySize);
  ans += add_optional_record("TradeId", entry.TradeId);
  //  std::uint64_t MDFlags;
  //  std::uint64_t MDFlags2;
  //  schema::enums::MDEntryType MDEntryType;

  ans += end_brace;
  return ans;
}

} // namespace simba