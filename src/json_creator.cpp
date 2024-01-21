#include "json_creator.h"
#include <iostream>
#include <sstream>
namespace simba {
JsonCreator::JsonCreator(WriterCallback &&cb) : cb_(std::move(cb)) {
  current_json_string_.reserve(2048);
}

void JsonCreator::operator()(start_packet) {
  current_json_string_ += start_brace;
}
void JsonCreator::operator()(end_packet) {
  // Remove ending comma
  current_json_string_.pop_back();
  current_json_string_.pop_back();
  current_json_string_ += end_brace;
  current_json_string_ += "\n";
  cb_(current_json_string_);
  current_json_string_.clear();
}

void JsonCreator::operator()(
    const schema::structs::MarketDataPacketHeader &mdp_header) {
  start_main_element("MDPHeader");
  current_json_string_ += add_numeric_record("MsgSeqNum", mdp_header.MsgSeqNum);
  current_json_string_ += add_numeric_record("MsgSize", mdp_header.MsgSize);
  current_json_string_ += add_numeric_record("MsgFlags", mdp_header.MsgFlags);
  current_json_string_ +=
      add_numeric_record("SendingTime", mdp_header.SendingTime, true);
  end_main_element();
}
void JsonCreator::operator()(
    const schema::structs::IncrementalPacketHeader &incremental_header) {
  start_main_element("IncrementalHeader");
  current_json_string_ +=
      add_numeric_record("TransactTime", incremental_header.TransactTime);
  current_json_string_ +=
      add_numeric_record("ExchangeTradingSessionId",
                         incremental_header.ExchangeTradingSessionId, true);

  end_main_element();
}
void JsonCreator::operator()(
    const messages::application_layer::BestPrices &best_prices) {
  start_main_element("BestPrices");
  add_element(best_prices.S);
  current_json_string_ += comma;
  add_element(best_prices.NoMDEntries);
  current_json_string_ += comma;
  add_array("Entries", best_prices.Entries);
  end_main_element();
}
void JsonCreator::operator()(
    const messages::application_layer::OrderUpdate &order_update) {
  start_main_element("OrderUpdate");
  add_element(order_update.S);
  current_json_string_ += comma;
  current_json_string_ +=
      add_numeric_record("MDEntryId", order_update.MDEntryId);
  current_json_string_ +=
      add_numeric_record("MDEntryPx", order_update.MDEntryPx);
  current_json_string_ +=
      add_numeric_record("MDEntrySize", order_update.MDEntrySize);
  current_json_string_ += add_bitmask_record<schema::bitmasks::MDFlagsSet>(
      "MDFlags", order_update.MDFlags);
  current_json_string_ += add_bitmask_record<schema::bitmasks::MDFlags2Set>(
      "MDFlags2", order_update.MDFlags2);
  current_json_string_ +=
      add_numeric_record("SecurityId", order_update.SecurityId);
  current_json_string_ += add_numeric_record("RptSeq", order_update.RptSeq);
  current_json_string_ +=
      add_enum_record("MDUpdateAction", order_update.MDUpdateAction);
  current_json_string_ +=
      add_enum_record("MDEntryType", order_update.MDEntryType, true);

  end_main_element();
}
void JsonCreator::operator()(
    const messages::application_layer::OrderExecution &order_execution) {
  start_main_element("OrderExecution");
  add_element(order_execution.S);
  current_json_string_ += comma;
  current_json_string_ +=
      add_numeric_record("MDEntryId", order_execution.MDEntryId);
  current_json_string_ +=
      add_optional_record("MDEntryPx", order_execution.MDEntryPx);
  current_json_string_ +=
      add_optional_record("MDEntrySize", order_execution.MDEntrySize);
  current_json_string_ += add_numeric_record("LastPx", order_execution.LastPx);
  current_json_string_ +=
      add_numeric_record("LastQty", order_execution.LastQty);
  current_json_string_ +=
      add_numeric_record("TradeId", order_execution.TradeId);

  current_json_string_ += add_bitmask_record<schema::bitmasks::MDFlagsSet>(
      "MDFlags", order_execution.MDFlags);
  current_json_string_ += add_bitmask_record<schema::bitmasks::MDFlags2Set>(
      "MDFlags2", order_execution.MDFlags2);
  current_json_string_ +=
      add_numeric_record("SecurityId", order_execution.SecurityId);

  current_json_string_ += add_numeric_record("RptSeq", order_execution.RptSeq);
  current_json_string_ +=
      add_enum_record("MDUpdateAction", order_execution.MDUpdateAction);
  current_json_string_ +=
      add_enum_record("MDEntryType", order_execution.MDEntryType, true);

  end_main_element();
}
void JsonCreator::operator()(
    const messages::application_layer::OrderBookSnapShot &order_snapshot) {
  start_main_element("OrderBookSnapShot");
  add_element(order_snapshot.S);
  current_json_string_ += comma;
  current_json_string_ +=
      add_numeric_record("SecurityId", order_snapshot.SecurityId);
  current_json_string_ += add_numeric_record(
      "LastMsgSeqNumProcessed", order_snapshot.LastMsgSeqNumProcessed);
  current_json_string_ += add_numeric_record("RptSeq", order_snapshot.RptSeq);
  current_json_string_ += add_numeric_record(
      "ExchangeTradingSessionId", order_snapshot.ExchangeTradingSessionId);
  add_element(order_snapshot.NoMDEntries);
  current_json_string_ += comma;
  add_array("Snapshots", order_snapshot.Entries);
  end_main_element();
}

void JsonCreator::add_element(const schema::structs::SBEHeader &S) {
  current_json_string_ += add_key("SBEHeader");
  current_json_string_ += start_brace;
  current_json_string_ += add_numeric_record("BlockLength", S.BlockLength);
  current_json_string_ += add_numeric_record("TemplateId", S.TemplateId);
  current_json_string_ += add_numeric_record("SchemaId", S.SchemaId);
  current_json_string_ += add_numeric_record("Version", S.Version, true);
  current_json_string_ += end_brace;
}

void JsonCreator::add_element(const schema::structs::groupSize &G) {
  current_json_string_ += add_key("NoMDEntries");
  current_json_string_ += start_brace;
  current_json_string_ += add_numeric_record("blockLength", G.blockLength);
  current_json_string_ +=
      add_numeric_record("numInGroup", G.numInGroup, true /*is_last*/);
  current_json_string_ += end_brace;
}

void JsonCreator::add_element(
    const messages::application_layer::BestPricesEntry &entry) {
  current_json_string_ += add_key("BestPricesEntry");
  current_json_string_ += start_brace;
  current_json_string_ += add_optional_record("MktBidPx", entry.MktBidPx);
  current_json_string_ += add_optional_record("MktOfferPx", entry.MktOfferPx);
  current_json_string_ += add_optional_record("MktBidSize", entry.MktBidSize);
  current_json_string_ +=
      add_optional_record("MktOfferSize", entry.MktOfferSize);
  current_json_string_ +=
      add_numeric_record("SecurityId", entry.SecurityId, true /*is_last*/);
  current_json_string_ += end_brace;
}

void JsonCreator::add_element(
    const messages::application_layer::SnapShotEntry &entry) {
  current_json_string_ += add_key("SnapShotEntry");
  current_json_string_ += start_brace;
  current_json_string_ += add_optional_record("MDEntryId", entry.MDEntryId);
  current_json_string_ +=
      add_numeric_record("TransactTime", entry.TransactTime);
  current_json_string_ += add_optional_record("MDEntryPx", entry.MDEntryPx);
  current_json_string_ += add_optional_record("MDEntrySize", entry.MDEntrySize);
  current_json_string_ += add_optional_record("TradeId", entry.TradeId);
  current_json_string_ += add_bitmask_record<schema::bitmasks::MDFlagsSet>(
      "MDFlags", entry.MDFlags);
  current_json_string_ += add_bitmask_record<schema::bitmasks::MDFlags2Set>(
      "MDFlags2", entry.MDFlags2);
  current_json_string_ += add_enum_record("MDEntryType", entry.MDEntryType);
  current_json_string_ += end_brace;
}

void JsonCreator::end_main_element() {
  current_json_string_ += end_brace;
  current_json_string_ += end_brace;
  current_json_string_ += comma;
}

void JsonCreator::start_main_element(const std::string &key) {
  current_json_string_ += start_brace;
  current_json_string_ += add_key(key);
  current_json_string_ += start_brace;
}

} // namespace simba