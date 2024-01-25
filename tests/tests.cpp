#include "tests/tests.h"
#include "json_creator.h"
#include "pcap_reader.h"
#include "protocol_decoder.h"
#include "schema/bitmasks.h"
#include "schema/structs.h"
#include "stream_writer.h"
#include "tests/decoder_output_functor.h"
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <string_view>
#include <unistd.h>

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
  run_basic_stream_writer_test();
  run_integration_tests();
}

void test_fixture::run_integration_tests() {
  integration_test_1();
  integration_test_2();
  integration_test_3();
  integration_test_4();
  integration_test_5();
  integration_test_6();
}
void test_fixture::run_protocol_decoder_tests() {
  decode_order_update_test();
  decode_order_execution_test();
  decode_best_prices_test();
  decode_order_book_snapshot_test();
}

void test_fixture::integration_test_1() {
  std::string inpcap_file =
      current_directory_path + "/tests/test_pcaps/order_update.pcap";
  std::string json_file =
      current_directory_path + "/tests/expected_json_outputs/order_update.json";
  assert_true("integration_test_1", integration_test(inpcap_file, json_file));
}

bool test_fixture::integration_test(std::string_view inpcap_file,
                                    std::string_view json_file) {
  std::stringstream ss;
  writer::StreamWriter W(ss);
  simba::JsonCreator J(W);
  simba::ProtocolDecoder PD(J, os_);
  reader::PcapReader Reader(PD, os_);
  Reader.read_packets(inpcap_file);
  std::ifstream ifs(json_file.data(), std::ios::binary | std::ios::ate);
  const auto size = ifs.tellg();
  std::string expected_output(size, '\0');
  ifs.seekg(0);
  ifs.read(&expected_output[0], size);
  return expected_output == ss.str();
}
void test_fixture::integration_test_2() {
  std::string inpcap_file =
      current_directory_path + "/tests/test_pcaps/order_execution.pcap";
  std::string json_file = current_directory_path +
                          "/tests/expected_json_outputs/order_execution.json";
  assert_true("integration_test_2", integration_test(inpcap_file, json_file));
}
void test_fixture::integration_test_3() {
  std::string inpcap_file =
      current_directory_path + "/tests/test_pcaps/next_best_prices.pcap";
  std::string json_file = current_directory_path +
                          "/tests/expected_json_outputs/next_best_prices.json";
  assert_true("integration_test_3", integration_test(inpcap_file, json_file));
}
void test_fixture::integration_test_4() {
  std::string inpcap_file =
      current_directory_path + "/tests/test_pcaps/long_snapshot.pcap";
  std::string json_file = current_directory_path +
                          "/tests/expected_json_outputs/long_snapshot.json";
  assert_true("integration_test_4", integration_test(inpcap_file, json_file));
}
void test_fixture::integration_test_5() {
  std::string inpcap_file =
      current_directory_path + "/tests/test_pcaps/multiple_order_updates.pcap";
  std::string json_file =
      current_directory_path +
      "/tests/expected_json_outputs/multiple_order_updates.json";
  assert_true("integration_test_5", integration_test(inpcap_file, json_file));
}
void test_fixture::integration_test_6() {
  std::string inpcap_file =
      current_directory_path +
      "/tests/test_pcaps/snapshot_end_and_start_within_same_packet.pcap";
  std::string json_file = current_directory_path +
                          "/tests/expected_json_outputs/"
                          "snapshot_end_and_start_within_same_packet.json";
  assert_true("integration_test_6", integration_test(inpcap_file, json_file));
}

void test_fixture::run_basic_stream_writer_test() {
  std::stringstream ss;
  std::string expected_output = "Hello world\n";
  writer::StreamWriter<decltype(ss)> sw(ss);
  sw(expected_output);
  assert_true("run_basic_stream_writer_test", expected_output == ss.str());
}
void test_fixture::decode_order_update_test() {
  using namespace simba::messages::application_layer;
  using namespace simba::schema::structs;
  using namespace simba::schema::enums;
  MarketDataPacketHeader expected_mdp{.MsgSeqNum = 29158931,
                                      .MsgSize = 86,
                                      .MsgFlags = 9,
                                      .SendingTime = 1696935540002780516};
  IncrementalPacketHeader expected_inc{.TransactTime = 1696935540002741130,
                                       .ExchangeTradingSessionId = 6902};
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

  uint8_t buffer[86]{};
  uint8_t *tmp = buffer;
  update_buffer(tmp, expected_mdp);
  update_buffer(tmp, expected_inc);
  // order_update is POD so far. No std::optionals. This struct is also made
  // packed in application_layer_messages.h
  update_buffer(tmp, expected_order_update);

  DecoderOutputFunctor output_fn;
  simba::ProtocolDecoder pd(output_fn, os_);
  pd({buffer, 86});
  auto output_mdp = output_fn.mdp_header();
  auto output_inc = output_fn.inc_header();
  auto output_update = output_fn.order_update();
  assert_true("decoder_order_update",
              expected_mdp == output_mdp && output_inc == expected_inc &&
                  output_update == expected_order_update);
}

void test_fixture::decode_order_execution_test() {
  using namespace simba::messages::application_layer;
  using namespace simba::schema::structs;
  using namespace simba::schema::enums;
  using namespace simba::schema::bitmasks;
  MarketDataPacketHeader expected_mdp{.MsgSeqNum = 29315345,
                                      .MsgSize = 110,
                                      .MsgFlags = 9,
                                      .SendingTime = 1696935956631112575};
  IncrementalPacketHeader expected_inc{.TransactTime = 1696935956631021931,
                                       .ExchangeTradingSessionId = 6902};
  OrderExecution expected_order_exec{.S = {.BlockLength = 74,
                                           .TemplateId = 16,
                                           .SchemaId = 19780,
                                           .Version = 4},
                                     .MDEntryId = 2012575727345762677,
                                     .MDEntrySize = 2,
                                     .LastPx = {.mantissa = 335200},
                                     .LastQty = 200,
                                     .TradeId = 900,
                                     .MDFlags = 4398046515201,
                                     .MDFlags2 = 0,
                                     .SecurityId = 82,
                                     .RptSeq = 508234,
                                     .MDUpdateAction = MDUpdateAction::Change,
                                     .MDEntryType = MDEntryType::Bid};

  uint8_t buffer[1024]{}; // kept larger than required as an another check
  uint8_t *tmp = buffer;
  update_buffer(tmp, expected_mdp);
  update_buffer(tmp, expected_inc);
  update_buffer(tmp, expected_order_exec.S);
  update_buffer(tmp, expected_order_exec.MDEntryId);
  auto md_entry_px = expected_order_exec.MDEntryPx.mantissa.has_value()
                         ? expected_order_exec.MDEntryPx.mantissa.value()
                         : simba::schema::types::NullValues::Int64;
  update_buffer(tmp, md_entry_px);
  update_buffer(tmp, *expected_order_exec.MDEntrySize);
  update_buffer(tmp, expected_order_exec.LastPx.mantissa);
  update_buffer(tmp, expected_order_exec.LastQty);
  update_buffer(tmp, expected_order_exec.TradeId);
  update_buffer(tmp, expected_order_exec.MDFlags);
  update_buffer(tmp, expected_order_exec.MDFlags2);
  update_buffer(tmp, expected_order_exec.SecurityId);
  update_buffer(tmp, expected_order_exec.RptSeq);
  update_buffer(tmp, expected_order_exec.MDUpdateAction);
  update_buffer(tmp, expected_order_exec.MDEntryType);
  DecoderOutputFunctor output_fn;
  simba::ProtocolDecoder pd(output_fn, os_);
  pd({buffer, 110});
  assert_true("decode_order_execution_test",
              expected_order_exec == output_fn.order_execution() &&
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
    auto offer_mantissa = entry.MktOfferPx.mantissa
                              ? *entry.MktOfferPx.mantissa
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
  simba::ProtocolDecoder pd(output_fn, os_);
  pd({buffer, 111});
  assert_true("decoder_best_prices_test",
              expected_mdp == output_fn.mdp_header() &&
                  expected_inc == output_fn.inc_header());
}

void test_fixture::decode_order_book_snapshot_test() {
  using namespace simba::messages::application_layer;
  using namespace simba::schema::structs;
  using namespace simba::schema::enums;
  using namespace simba::schema::bitmasks;
  MarketDataPacketHeader expected_mdp{.MsgSeqNum = 8394,
                                      .MsgSize = 157,
                                      .MsgFlags = 6,
                                      .SendingTime = 1696935540034865348};
  OrderBookSnapShot expected_order_book_snapshot{
      .S = {.BlockLength = 16,
            .TemplateId = 17,
            .SchemaId = 19780,
            .Version = 4},
      .SecurityId = 2863159,
      .LastMsgSeqNumProcessed = 29151376,
      .RptSeq = 10,
      .ExchangeTradingSessionId = 6902,
      .NoMDEntries = {.blockLength = 57, .numInGroup = 2},
      .Entries = {
          {.MDEntryId = 1892948862243438606,
           .TransactTime = 1696867113204251569,
           .MDEntryPx = {.mantissa = 86500000},
           .MDEntrySize = 1,
           .TradeId = 0,
           .MDFlags = static_cast<std::uint64_t>(MDFlagsSet::DAY) |
                      static_cast<std::uint64_t>(MDFlagsSet::EndOfTransaction),
           .MDFlags2 = static_cast<std::uint64_t>(MDFlags2Set::Zero),
           .MDEntryType = MDEntryType::Offer},
          {.TransactTime = 1696935006323394189,
           .MDEntryPx = {.mantissa = 409100000},
           .MDFlags = static_cast<std::uint64_t>(MDFlagsSet::DAY) |
                      static_cast<std::uint64_t>(MDFlagsSet::EndOfTransaction) |
                      static_cast<std::uint64_t>(MDFlagsSet::Replace),
           .MDFlags2 = static_cast<std::uint64_t>(MDFlags2Set::Zero),
           .MDEntryType = MDEntryType::Offer}}};

  uint8_t buffer[157]{};
  uint8_t *tmp = buffer;
  update_buffer(tmp, expected_mdp);
  update_buffer(tmp, expected_order_book_snapshot.S);
  update_buffer(tmp, expected_order_book_snapshot.SecurityId);
  update_buffer(tmp, expected_order_book_snapshot.LastMsgSeqNumProcessed);
  update_buffer(tmp, expected_order_book_snapshot.RptSeq);
  update_buffer(tmp, expected_order_book_snapshot.ExchangeTradingSessionId);
  update_buffer(tmp, expected_order_book_snapshot.NoMDEntries);

  for (const auto &snapshot : expected_order_book_snapshot.Entries) {
    auto md_entry_id = snapshot.MDEntryId
                           ? *snapshot.MDEntryId
                           : simba::schema::types::NullValues::Int64;
    update_buffer(tmp, md_entry_id);
    update_buffer(tmp, snapshot.TransactTime);
    auto px_mantissa = snapshot.MDEntryPx.mantissa
                           ? *snapshot.MDEntryPx.mantissa
                           : simba::schema::types::NullValues::Int64;
    update_buffer(tmp, px_mantissa);
    auto md_entry_size = snapshot.MDEntrySize
                             ? *snapshot.MDEntrySize
                             : simba::schema::types::NullValues::Int64;
    update_buffer(tmp, md_entry_size);
    auto trade_id = snapshot.TradeId ? *snapshot.TradeId
                                     : simba::schema::types::NullValues::Int64;
    update_buffer(tmp, trade_id);
    update_buffer(tmp, snapshot.MDFlags);
    update_buffer(tmp, snapshot.MDFlags2);
    char md_entry_type = static_cast<char>(snapshot.MDEntryType);
    update_buffer(tmp, md_entry_type);
  }

  DecoderOutputFunctor output_fn;
  simba::ProtocolDecoder pd(output_fn, os_);
  pd({buffer, 157});
  assert_true("decode_order_book_snapshot_test",
              expected_mdp == output_fn.mdp_header() &&
                  expected_order_book_snapshot ==
                      output_fn.order_book_snapshot());
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
  std::vector<int> expected_payload_lengths{86};
  std::string file_name =
      current_directory_path + "tests/test_pcaps/order_update.pcap";
  assert_true("packet_reader_num_packets_test_1",
              packet_reader_test(file_name, expected_payload_lengths));
}
void test_fixture::packet_reader_2() {
  std::vector<int> expected_payload_lengths(8, 1354);
  expected_payload_lengths.push_back(898);
  std::string file_name =
      current_directory_path + "tests/test_pcaps/long_snapshot.pcap";
  assert_true("packet_reader_num_packets_test_2",
              packet_reader_test(file_name, expected_payload_lengths));
}

void test_fixture::packet_reader_3() {
  std::vector<int> expected_payload_lengths{75, 250};
  std::string file_name =
      current_directory_path + "tests/test_pcaps/next_best_prices.pcap";
  assert_true("packet_reader_num_packets_test_3",
              packet_reader_test(file_name, expected_payload_lengths));
}

void test_fixture::packet_reader_4() {
  std::vector<int> expected_payload_lengths{110};
  std::string file_name = current_directory_path +
                           "tests/test_pcaps/order_execution.pcap";
  assert_true("packet_reader_num_packets_test_4",
              packet_reader_test(file_name, expected_payload_lengths));
}
void test_fixture::packet_reader_5() {
  std::vector<int> expected_payload_lengths{100};
  std::string file_name = current_directory_path +
                           "tests/test_pcaps/snapshot_end_and_start_within_same_packet.pcap";

  assert_true("packet_reader_num_packets_test_5",
              packet_reader_test(file_name, expected_payload_lengths));
}

bool test_fixture::packet_reader_test(
    std::string_view file_name, std::vector<int> expected_payload_lengths) {
  std::vector<int> output_lengths;
  auto cb = [&output_lengths](auto buf_sv) {
    output_lengths.push_back(buf_sv.size());
  };

  reader::PcapReader pcap_reader(std::move(cb), os_);
  pcap_reader.read_packets(file_name);
  return output_lengths == expected_payload_lengths;
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
