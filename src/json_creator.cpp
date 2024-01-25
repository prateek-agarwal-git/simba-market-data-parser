#include "json_creator.h"
#include <iostream>
#include <sstream>
namespace simba {
JsonCreator::JsonCreator(WriterCallback &&cb) : cb_(std::move(cb)) {
  // reserved some memory for avoiding initial reallocations.
  current_json_string_.reserve(2048);
}

void JsonCreator::operator()(tag_structs::start_packet) {
  current_json_string_ += start_brace;
}
void JsonCreator::operator()(tag_structs::end_packet) {
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
  current_json_string_ += numeric_record_element("MsgSeqNum", mdp_header.MsgSeqNum);
  current_json_string_ += numeric_record_element("MsgSize", mdp_header.MsgSize);
  current_json_string_ += numeric_record_element("MsgFlags", mdp_header.MsgFlags);
  current_json_string_ +=
      numeric_record_element("SendingTime", mdp_header.SendingTime, true);
  end_main_element();
}
void JsonCreator::operator()(
    const schema::structs::IncrementalPacketHeader &incremental_header) {
  start_main_element("IncrementalHeader");
  current_json_string_ +=
      numeric_record_element("TransactTime", incremental_header.TransactTime);
  current_json_string_ +=
      numeric_record_element("ExchangeTradingSessionId",
                         incremental_header.ExchangeTradingSessionId, true);

  end_main_element();
}
void JsonCreator::operator()(
    const messages::application_layer::BestPrices &best_prices) {
  start_main_element("BestPrices");
  add_element(best_prices.S);
  current_json_string_ += comma;
  add_element(best_prices.NoMDEntries);
  add_array("Entries", best_prices.Entries);
  end_main_element();
}
void JsonCreator::operator()(
    const messages::application_layer::OrderUpdate &order_update) {
  start_main_element("OrderUpdate");
  add_element(order_update.S);
  current_json_string_ += comma;
  current_json_string_ +=
      numeric_record_element("MDEntryId", order_update.MDEntryId);
  current_json_string_ +=
      numeric_record_element("MDEntryPx", order_update.MDEntryPx);
  current_json_string_ +=
      numeric_record_element("MDEntrySize", order_update.MDEntrySize);
  current_json_string_ += bitmask_record_element<schema::bitmasks::MDFlagsSet>(
      "MDFlags", order_update.MDFlags);
  current_json_string_ += bitmask_record_element<schema::bitmasks::MDFlags2Set>(
      "MDFlags2", order_update.MDFlags2);
  current_json_string_ +=
      numeric_record_element("SecurityId", order_update.SecurityId);
  current_json_string_ += numeric_record_element("RptSeq", order_update.RptSeq);
  current_json_string_ +=
      enum_record_element("MDUpdateAction", order_update.MDUpdateAction);
  current_json_string_ +=
      enum_record_element("MDEntryType", order_update.MDEntryType, true);
  end_main_element();
}
void JsonCreator::operator()(
    const messages::application_layer::OrderExecution &order_execution) {
  start_main_element("OrderExecution");
  add_element(order_execution.S);
  current_json_string_ += comma;
  current_json_string_ +=
      numeric_record_element("MDEntryId", order_execution.MDEntryId);
  current_json_string_ +=
      optional_record_element("MDEntryPx", order_execution.MDEntryPx);
  current_json_string_ +=
      optional_record_element("MDEntrySize", order_execution.MDEntrySize);
  current_json_string_ += numeric_record_element("LastPx", order_execution.LastPx);
  current_json_string_ +=
      numeric_record_element("LastQty", order_execution.LastQty);
  current_json_string_ +=
      numeric_record_element("TradeId", order_execution.TradeId);

  current_json_string_ += bitmask_record_element<schema::bitmasks::MDFlagsSet>(
      "MDFlags", order_execution.MDFlags);
  current_json_string_ += bitmask_record_element<schema::bitmasks::MDFlags2Set>(
      "MDFlags2", order_execution.MDFlags2);
  current_json_string_ +=
      numeric_record_element("SecurityId", order_execution.SecurityId);

  current_json_string_ += numeric_record_element("RptSeq", order_execution.RptSeq);
  current_json_string_ +=
      enum_record_element("MDUpdateAction", order_execution.MDUpdateAction);
  current_json_string_ +=
      enum_record_element("MDEntryType", order_execution.MDEntryType, true);

  end_main_element();
}
void JsonCreator::operator()(
    const messages::application_layer::OrderBookSnapShot &order_snapshot) {
  start_main_element("OrderBookSnapShot");
  add_element(order_snapshot.S);
  current_json_string_ += comma;
  current_json_string_ +=
      numeric_record_element("SecurityId", order_snapshot.SecurityId);
  current_json_string_ += numeric_record_element(
      "LastMsgSeqNumProcessed", order_snapshot.LastMsgSeqNumProcessed);
  current_json_string_ += numeric_record_element("RptSeq", order_snapshot.RptSeq);
  current_json_string_ += numeric_record_element(
      "ExchangeTradingSessionId", order_snapshot.ExchangeTradingSessionId);
  add_element(order_snapshot.NoMDEntries);
  current_json_string_ += comma;
  add_array("Snapshots", order_snapshot.Entries);
  end_main_element();
}

void JsonCreator::add_element(const schema::structs::SBEHeader &S) {
  current_json_string_ += key_element("SBEHeader");
  current_json_string_ += start_brace;
  current_json_string_ += numeric_record_element("BlockLength", S.BlockLength);
  current_json_string_ += numeric_record_element("TemplateId", S.TemplateId);
  current_json_string_ += numeric_record_element("SchemaId", S.SchemaId);
  current_json_string_ += numeric_record_element("Version", S.Version, true);
  current_json_string_ += end_brace;
}

void JsonCreator::add_element(const schema::structs::groupSize &G) {
  current_json_string_ += key_element("NoMDEntries");
  current_json_string_ += start_brace;
  current_json_string_ += numeric_record_element("blockLength", G.blockLength);
  current_json_string_ +=
      numeric_record_element("numInGroup", G.numInGroup, true /*is_last*/);
  current_json_string_ += end_brace;
}

void JsonCreator::add_element(
    const messages::application_layer::BestPricesEntry &entry) {
  current_json_string_ += start_brace;
  current_json_string_ += optional_record_element("MktBidPx", entry.MktBidPx);
  current_json_string_ += optional_record_element("MktOfferPx", entry.MktOfferPx);
  current_json_string_ += optional_record_element("MktBidSize", entry.MktBidSize);
  current_json_string_ +=
      optional_record_element("MktOfferSize", entry.MktOfferSize);
  current_json_string_ +=
      numeric_record_element("SecurityId", entry.SecurityId, true /*is_last*/);
  current_json_string_ += end_brace;
}

void JsonCreator::add_element(
    const messages::application_layer::SnapShotEntry &entry) {
  current_json_string_ += start_brace;
  current_json_string_ += optional_record_element("MDEntryId", entry.MDEntryId);
  current_json_string_ +=
      numeric_record_element("TransactTime", entry.TransactTime);
  current_json_string_ += optional_record_element("MDEntryPx", entry.MDEntryPx);
  current_json_string_ += optional_record_element("MDEntrySize", entry.MDEntrySize);
  current_json_string_ += optional_record_element("TradeId", entry.TradeId);
  current_json_string_ += bitmask_record_element<schema::bitmasks::MDFlagsSet>(
      "MDFlags", entry.MDFlags);
  current_json_string_ += bitmask_record_element<schema::bitmasks::MDFlags2Set>(
      "MDFlags2", entry.MDFlags2);
  current_json_string_ +=
      enum_record_element("MDEntryType", entry.MDEntryType, true);
  current_json_string_ += end_brace;
}

void JsonCreator::end_main_element() {
  current_json_string_ += end_brace;
  current_json_string_ += end_brace;
  current_json_string_ += comma;
}

void JsonCreator::start_main_element(const std::string &key) {
  current_json_string_ += start_brace;
  current_json_string_ += key_element(key);
  current_json_string_ += start_brace;
}

} // namespace simba