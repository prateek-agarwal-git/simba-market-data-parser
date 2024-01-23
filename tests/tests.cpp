#include "tests.h"
#include "json_creator.h"
#include "pcap_reader.h"
#include "protocol_decoder.h"
#include "schema/bitmasks.h"
#include "schema/structs.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <vector>

int main() {
  std::string test_log = "test_" + std::to_string(getpid()) + ".txt";
  std::ofstream ofs(test_log, std::ios::out);
  tests::test_fixture F(ofs.is_open() ? ofs : std::cout);
  F.run_tests();
  return 0;
}

namespace tests {

void test_fixture::run_tests() {
  decoder_successful_compilation_test();
  run_packet_reader_tests();
  run_json_creator_tests();
}

void test_fixture::run_packet_reader_tests() {
  packet_reader_1();
  packet_reader_2();
  packet_reader_3();
  packet_reader_4();
  packet_reader_5();
}
void test_fixture::run_json_creator_tests() {
  json_order_update_test();
  json_mdp_header_test();
  json_incremental_header_test();
  json_order_execution_test();
  json_best_prices_test();
  json_order_book_snapshot_test();
}

void test_fixture::json_order_book_snapshot_test() {

  using namespace simba::messages::application_layer;
  using namespace simba::schema;
  std::vector<SnapShotEntry> entries(2);
  entries[0] = {.TransactTime = 1024,
                .MDFlags =
                    static_cast<uint64_t>(bitmasks::MDFlagsSet::IOC) |
                    static_cast<uint64_t>(bitmasks::MDFlagsSet::Synthetic),
                .MDEntryType = enums::MDEntryType::Bid};
  entries[1] = {.MDEntryId = 9845,
                .TransactTime = 2048,
                .MDEntryPx = {.mantissa = 19456321},
                .MDEntrySize = 184,
                .MDFlags =
                    static_cast<uint64_t>(bitmasks::MDFlagsSet::MultiLeg) |
                    static_cast<uint64_t>(bitmasks::MDFlagsSet::ActiveSide),
                .MDFlags2 = static_cast<uint64_t>(bitmasks::MDFlags2Set::Zero),
                .MDEntryType = enums::MDEntryType::Offer};
  OrderBookSnapShot order_book_snapshot{
      .S = S_,
      .SecurityId = 29,
      .LastMsgSeqNumProcessed = 100786,
      .RptSeq = 8402,
      .ExchangeTradingSessionId = 289423126,
      .NoMDEntries = {.blockLength = 57, .numInGroup = 1},
      .Entries = entries};
  std::string expected_json_output =
      R"({{"OrderBookSnapShot": {"SBEHeader": {"BlockLength": 10, "TemplateId": 30, "SchemaId": 20, "Version": 40}, "SecurityId": 29, "LastMsgSeqNumProcessed": 100786, "RptSeq": 8402, "ExchangeTradingSessionId": 289423126, "NoMDEntries": {"blockLength": 57, "numInGroup": 1}, , "Snapshots": [{"TransactTime": 1024, "MDFlags": "IOC|Synthetic", "MDFlags2": "", "MDEntryType": "Bid"}, {"MDEntryId": 9845, "TransactTime": 2048, "MDEntryPx": 194.56321, "MDEntrySize": 184, "MDFlags": "Multileg|ActiveSide", "MDFlags2": "Zero", "MDEntryType": "Offer"}]}}})";
  expected_json_output += "\n";
  std::string output;
  auto cb = [&output](const std::string &json) { output = json; };
  simba::JsonCreator json_creator(std::move(cb));
  json_creator(simba::tag_structs::start_packet{});
  json_creator(order_book_snapshot);
  json_creator(simba::tag_structs::end_packet{});
  std::cout << output << std::endl;
  std::cout << expected_json_output << std::endl;
  assert_true("json_order_book_snapshot", output == expected_json_output);
}
void test_fixture::decoder_successful_compilation_test() {
  std::string output;
  auto cb = [&output](const std::string &json) { output = json; };
  simba::JsonCreator json_creator(std::move(cb));
  simba::ProtocolDecoder pd(json_creator);
  reader::PcapReader reader(pd);
}

void test_fixture::packet_reader_1() {
  int count{};
  int expected_payload_length{86};
  int output_length{};
  auto cb = [&count, &output_length](const std::uint8_t *, int payload_length) {
    ++count;
    output_length = payload_length;
  };

  reader::PcapReader pcap_reader(std::move(cb));
  pcap_reader.read_packets(current_directory_path +
                           "tests/test_pcaps/order_update.pcap");
  assert_true("packet_reader_num_packets_test_1",
              (count == 1) && (expected_payload_length == output_length));
}
void test_fixture::packet_reader_2() {
  int count{};
  std::vector<int> expected_payload_lengths(8, 1354);
  expected_payload_lengths.push_back(898);
  std::vector<int> output_lengths;
  auto cb = [&count, &output_lengths](const std::uint8_t *,
                                      int payload_length) {
    ++count;
    output_lengths.push_back(payload_length);
  };

  reader::PcapReader pcap_reader(std::move(cb));
  pcap_reader.read_packets(current_directory_path +
                           "tests/test_pcaps/long_snapshot.pcap");
  assert_true("packet_reader_num_packets_test_2",
              (count == 9) && expected_payload_lengths == output_lengths);
}
void test_fixture::packet_reader_3() {
  int count{};
  std::vector<int> expected_payload_lengths{75, 250};
  std::vector<int> output_lengths;
  auto cb = [&count, &output_lengths](const std::uint8_t *,
                                      int payload_length) {
    ++count;
    output_lengths.push_back(payload_length);
  };

  reader::PcapReader pcap_reader(std::move(cb));
  pcap_reader.read_packets(current_directory_path +
                           "tests/test_pcaps/next_best_prices.pcap");
  assert_true("packet_reader_num_packets_test_3",
              (count == 2) && expected_payload_lengths == output_lengths);
}
void test_fixture::packet_reader_4() {
  int expected_payload_length{110};
  int output_length{};
  int count{};
  auto cb = [&count, &output_length](const std::uint8_t *, int payload_length) {
    ++count;
    output_length = payload_length;
  };

  reader::PcapReader pcap_reader(std::move(cb));
  pcap_reader.read_packets(current_directory_path +
                           "tests/test_pcaps/order_execution.pcap");
  assert_true("packet_reader_num_packets_test_4",
              (count == 1) && (expected_payload_length == output_length));
}
void test_fixture::packet_reader_5() {
  int count{};
  int output_length{};
  int expected_length{100};
  auto cb = [&count, &output_length](const std::uint8_t *, int payload_length) {
    ++count;
    output_length = payload_length;
  };
  reader::PcapReader pcap_reader(std::move(cb));
  pcap_reader.read_packets(
      current_directory_path +
      "tests/test_pcaps/snapshot_end_and_start_within_same_packet.pcap");
  assert_true("packet_reader_num_packets_test_5",
              count == 1 && (expected_length == output_length));
}
void test_fixture::json_best_prices_test() {
  using namespace simba::messages::application_layer;
  std::string expected_json_output =
      std::string(R"({{"BestPrices": )") + sbe_header_json_ +
      R"(, "NoMDEntries": {"blockLength": 36, "numInGroup": 2}, "Entries": [{"MktBidPx": 89.00000, "MktBidSize": 100, "SecurityId": 83}, {"MktOfferPx": 90.00000, "MktOfferSize": 50, "SecurityId": 84}]}}})" +
      "\n";
  std::vector<BestPricesEntry> v(2, BestPricesEntry{});
  v[0] = {
      .MktBidPx = {.mantissa = 89'00'000}, .MktBidSize = 100, .SecurityId = 83};
  v[1] = {.MktOfferPx = {.mantissa = 90'00'000},
          .MktOfferSize = 50,
          .SecurityId = 84};
  BestPrices best_prices{.S = S_,
                         .NoMDEntries = {.blockLength = 36, .numInGroup = 2},
                         .Entries = v};

  std::string output;
  auto cb = [&output](const std::string &json) { output = json; };
  simba::JsonCreator json_creator(std::move(cb));
  json_creator(simba::tag_structs::start_packet{});
  json_creator(best_prices);
  json_creator(simba::tag_structs::end_packet{});

  assert_true("json_best_prices_test", output == expected_json_output);
}
void test_fixture::json_order_execution_test() {
  simba::messages::application_layer::OrderExecution order_execution{
      .S = S_,
      .MDEntryId = 23,
      .MDEntryPx = std::nullopt,
      .MDEntrySize = 100,
      .LastPx = 8463210,
      .LastQty = 74,
      .TradeId = 56,
      .MDFlags =
          static_cast<uint64_t>(simba::schema::bitmasks::MDFlagsSet::IOC) |
          static_cast<uint64_t>(
              simba::schema::bitmasks::MDFlagsSet::DuringDiscreteAuction),
      .MDFlags2 = 0,
      .SecurityId = 90,
      .RptSeq = 1000'001,
      .MDUpdateAction = simba::schema::enums::MDUpdateAction::Delete,
      .MDEntryType = simba::schema::enums::MDEntryType::Bid};
  std::string expected_json_output =
      std::string(R"({{"OrderExecution": )") + sbe_header_json_ +
      R"(, "MDEntryId": 23, "MDEntrySize": 100, "LastPx": 84.63210, "LastQty": 74, "TradeId": 56, "MDFlags": "IOC|DuringDiscreteAuction", "MDFlags2": "", "SecurityId": 90, "RptSeq": 1000001, "MDUpdateAction": "Delete", "MDEntryType": "Bid"}}})" +
      "\n";
  std::string output;
  auto cb = [&output](const std::string &json) { output = json; };
  simba::JsonCreator json_creator(std::move(cb));
  json_creator(simba::tag_structs::start_packet{});
  json_creator(order_execution);
  json_creator(simba::tag_structs::end_packet{});
  assert_true("json_order_execution_test", output == expected_json_output);
}
void test_fixture::json_incremental_header_test() {

  simba::schema::structs::IncrementalPacketHeader incremental_header{
      .TransactTime = 1092, .ExchangeTradingSessionId = 2345864};

  std::string expected_json_output =
      R"({{"IncrementalHeader": {"TransactTime": 1092, "ExchangeTradingSessionId": 2345864}}})";
  expected_json_output += "\n";
  std::string output;
  auto cb = [&output](const std::string &json) { output = json; };
  simba::JsonCreator json_creator(std::move(cb));
  json_creator(simba::tag_structs::start_packet{});
  json_creator(incremental_header);
  json_creator(simba::tag_structs::end_packet{});
  assert_true("json_incremental_header_test", output == expected_json_output);
}

void test_fixture::json_mdp_header_test() {

  simba::schema::structs::MarketDataPacketHeader mdp_header{
      .MsgSeqNum = 19, .MsgSize = 84, .MsgFlags = 8, .SendingTime = 12344321};

  std::string expected_json_output =
      R"({{"MDPHeader": {"MsgSeqNum": 19, "MsgSize": 84, "MsgFlags": 8, "SendingTime": 12344321}}})";
  expected_json_output += "\n";
  std::string output;
  auto cb = [&output](const std::string &json) { output = json; };
  simba::JsonCreator json_creator(std::move(cb));
  json_creator(simba::tag_structs::start_packet{});
  json_creator(mdp_header);
  json_creator(simba::tag_structs::end_packet{});

  assert_true("json_mdp_header_test", output == expected_json_output);
}
void test_fixture::json_order_update_test() {
  simba::messages::application_layer::OrderUpdate order_update{
      .S = S_,
      .MDEntryId = 100,
      .MDEntryPx = {.mantissa = 1234567},
      .MDEntrySize = 25,
      .MDFlags = 0x1,
      .MDFlags2 = 0x1,
      .SecurityId = 84,
      .RptSeq = 123432,
      .MDUpdateAction = simba::schema::enums::MDUpdateAction::New,
      .MDEntryType = simba::schema::enums::MDEntryType::Offer};

  std::string expected_json_output =
      std::string(R"({{"OrderUpdate": )") + sbe_header_json_ +
      R"(, "MDEntryId": 100, "MDEntryPx": 12.34567, "MDEntrySize": 25, "MDFlags": "DAY", "MDFlags2": "Zero", "SecurityId": 84, "RptSeq": 123432, "MDUpdateAction": "New", "MDEntryType": "Offer"}}})";
  expected_json_output += "\n";
  std::string ans;
  auto cb = [&ans](const std::string &json) { ans = json; };
  simba::JsonCreator json_creator(std::move(cb));
  json_creator(simba::tag_structs::start_packet{});
  json_creator(order_update);
  json_creator(simba::tag_structs::end_packet{});
  assert_true("json_order_update_test", ans == expected_json_output);
}

void test_fixture::assert_true(const std::string &test_name, bool expression) {
  std::cout << test_name << ": " << (expression ? "SUCCESS" : "FAILURE")
            << std::endl;
}
} // namespace tests
