#include "tests/tests.h"
#include "json_creator.h"
#include "pcap_reader.h"
#include "protocol_decoder.h"
#include "schema/bitmasks.h"
#include "schema/structs.h"
#include "tests/decoder_output_functor.h"
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
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
  run_packet_reader_tests();
  run_protocol_decoder_tests();
  run_json_creator_tests();
}

void test_fixture::run_protocol_decoder_tests() {
  decode_order_update_test();
  decode_order_execution_test();
  decode_best_prices_test();
  //  decode_order_book_snapshot_test();
}

void test_fixture::decode_order_update_test() {
  using namespace simba::messages::application_layer;
  using namespace simba::schema::structs;
  using namespace simba::schema::enums;
  DecoderOutputFunctor output_fn_;
  simba::ProtocolDecoder pd(output_fn_);
  uint8_t buffer[86]{};
  MarketDataPacketHeader expected_mdp{.MsgSeqNum = 29158931,
                                      .MsgSize = 86,
                                      .MsgFlags = 9,
                                      .SendingTime = 1696935540002780516};
  int offset = 0;
  std::memcpy(buffer, reinterpret_cast<void *>(&expected_mdp),
              sizeof(expected_mdp));
  offset += sizeof(expected_mdp);
  IncrementalPacketHeader expected_inc{.TransactTime = 1696935540002741130,
                                       .ExchangeTradingSessionId = 6902};

  std::memcpy(buffer + offset, reinterpret_cast<void *>(&expected_inc),
              sizeof(expected_inc));
  offset += sizeof(expected_inc);
  OrderUpdate expected_order_update{.S = {.BlockLength = 50,
                                          .TemplateId = 15,
                                          .SchemaId = 19780,
                                          .Version = 4},
                                    .MDEntryId = {1963036131447847527},
                                    .MDEntryPx = {.mantissa = 10525000000},
                                    .MDFlags = 2101249,
                                    .MDFlags2 = 0,
                                    .SecurityId = 2704557,
                                    .RptSeq = 5903684,
                                    .MDUpdateAction = MDUpdateAction::Delete,
                                    .MDEntryType = MDEntryType::Bid};
  // order_update is POD so far. No std::optionals. This struct is also made
  // packed in application_layer_messages.h
  std::memcpy(buffer + offset, reinterpret_cast<void *>(&expected_order_update),
              sizeof(expected_order_update));

  pd(buffer, 86);
  auto output_mdp = output_fn_.mdp_header();
  auto output_inc = output_fn_.inc_header();
  auto output_update = output_fn_.order_update();
  assert_true("decoder_order_update",
              expected_mdp == output_mdp && output_inc == expected_inc &&
                  output_update == expected_order_update);
}

void test_fixture::decode_order_execution_test() {
  using namespace simba::messages::application_layer;
  using namespace simba::schema::structs;
  using namespace simba::schema::enums;
  using namespace simba::schema::bitmasks;
  DecoderOutputFunctor output_fn;
  simba::ProtocolDecoder pd(output_fn);
  uint8_t buffer[1024]{}; // kept larger than required as an another check
  uint8_t *tmp = buffer;
  MarketDataPacketHeader expected_mdp{.MsgSeqNum = 29315345,
                                      .MsgSize = 110,
                                      .MsgFlags = 9,
                                      .SendingTime = 1696935956631112575};
  IncrementalPacketHeader expected_inc{.TransactTime = 1696935956631021931,
                                       .ExchangeTradingSessionId = 6902};
  SBEHeader expected_S{
      .BlockLength = 74, .TemplateId = 16, .SchemaId = 19780, .Version = 4};
  update_buffer(tmp, expected_mdp);
  update_buffer(tmp, expected_inc);
  update_buffer(tmp, expected_S);
  std::int64_t MDEntryId = 2012575727345762677;
  update_buffer(tmp, MDEntryId);
  std::int64_t MDEntryPx = std::numeric_limits<std::int64_t>::max();
  update_buffer(tmp, MDEntryPx);
  std::int64_t MDEntrySize = 2;
  update_buffer(tmp, MDEntrySize);
  std::int64_t LastPx_mantissa = 335200;
  update_buffer(tmp, LastPx_mantissa);
  std::int64_t LastQty = 200;
  update_buffer(tmp, LastQty);
  std::int64_t TradeId = 900;
  update_buffer(tmp, TradeId);
  std::uint64_t MDFlags = 4398046515201;
  update_buffer(tmp, MDFlags);
  std::uint64_t MDFlags2 = 0;
  update_buffer(tmp, MDFlags2);
  std::int32_t SecurityId = 82;
  update_buffer(tmp, SecurityId);
  std::uint32_t RptSeq = 508234;
  update_buffer(tmp, RptSeq);
  std::uint8_t md_update_action = 1; // change
  update_buffer(tmp, md_update_action);
  char md_entry_type = '0'; // bid
  update_buffer(tmp, md_entry_type);
  OrderExecution expected_order_exec{
      .S = {.BlockLength = 74,
            .TemplateId = 16,
            .SchemaId = 19780,
            .Version = 4},
      .MDEntryId = MDEntryId,
      .MDEntrySize = 2,
      .LastPx = {.mantissa = LastPx_mantissa},
      .LastQty = LastQty,
      .TradeId = TradeId,
      .MDFlags = MDFlags,
      .MDFlags2 = MDFlags2,
      .SecurityId = SecurityId,
      .RptSeq = RptSeq,
      .MDUpdateAction = static_cast<MDUpdateAction>(md_update_action),
      .MDEntryType = static_cast<MDEntryType>(md_entry_type)};
  pd(buffer, 110);
  auto actual_output = output_fn.order_execution();
  assert_true("decode_order_execution_test",
              expected_order_exec == actual_output &&
                  expected_mdp == output_fn.mdp_header() &&
                  expected_inc == output_fn.inc_header());
}
void test_fixture::decode_best_prices_test() {
  using namespace simba::messages::application_layer;
  using namespace simba::schema::structs;
  using namespace simba::schema::enums;
  MarketDataPacketHeader expected_mdp{.MsgSeqNum = 29238096,
                                      .MsgSize = 111,
                                      .MsgFlags = 8,
                                      .SendingTime = 1696935901001191258};
  IncrementalPacketHeader expected_inc{.TransactTime = 1696935901001142462,
                                       .ExchangeTradingSessionId = 6902};
  BestPrices expected_best_prices{
      .S = {.BlockLength = 0,
            .TemplateId = 14,
            .SchemaId = 19780,
            .Version = 4},
      .NoMDEntries = {.blockLength = 36, .numInGroup = 2},
      .Entries = {{.MktBidPx = {.mantissa = 335700},
                   .MktOfferPx = {.mantissa = 336000},
                   .MktBidSize = 114,
                   .MktOfferSize = 1059,
                   .SecurityId = 3572611},
                  {.MktBidPx = {.mantissa = 25900},
                   .MktOfferPx = {.mantissa = 336000},
                   .MktBidSize = 1,
                   .MktOfferSize = 0,
                   .SecurityId = 4210017}}};

  uint8_t buffer[111]{};
  uint8_t *tmp = buffer;
  update_buffer(tmp, expected_mdp);
  update_buffer(tmp, expected_inc);
  update_buffer(tmp, expected_best_prices.S);
  update_buffer(tmp, expected_best_prices.NoMDEntries);
  for (const auto &entry : expected_best_prices.Entries) {
    auto bid_mantissa = entry.MktBidPx.mantissa.has_value()
                            ? entry.MktBidPx.mantissa.value()
                            : simba::schema::types::NullValues::Int64;
    update_buffer(tmp, bid_mantissa);
    auto offer_mantissa = entry.MktOfferPx.mantissa.has_value()
                              ? entry.MktOfferPx.mantissa.value()
                              : simba::schema::types::NullValues::Int64;
    update_buffer(tmp, offer_mantissa);
    auto bid_size = entry.MktBidSize.has_value()
                        ? entry.MktBidSize.has_value()
                        : simba::schema::types::NullValues::Int64;
    update_buffer(tmp, bid_size);
    auto offer_size = entry.MktOfferSize.has_value()
                          ? entry.MktOfferSize.has_value()
                          : simba::schema::types::NullValues::Int64;
    update_buffer(tmp, offer_size);
    update_buffer(tmp, entry.SecurityId);
  }

  DecoderOutputFunctor output_fn;
  simba::ProtocolDecoder pd(output_fn);
  pd(buffer, 111);
  assert_true("decoder_best_prices_test",
              expected_mdp == output_fn.mdp_header() &&
                  expected_inc == output_fn.inc_header());
}

void test_fixture::decode_order_book_snapshot_test() {
  DecoderOutputFunctor output_fn_;
  simba::ProtocolDecoder pd(output_fn_);
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
  assert_true("json_order_book_snapshot", output == expected_json_output);
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
