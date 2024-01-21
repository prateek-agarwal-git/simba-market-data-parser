#include "json_creator.h"
#include "schema/structs.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <unistd.h>

namespace tests {
class test_fixture {
public:
  test_fixture(std::ostream &os) : os_(os) {}
  void run_tests() {
    json_order_update_test();
    json_mdp_header_test();
    json_incremental_header_test();
    json_order_execution_test();
    json_best_prices_test();
  }
  void json_best_prices_test() {
    using namespace simba::messages::application_layer;
    std::string expected_json_output;

    std::vector<BestPricesEntry> v(2, BestPricesEntry{});
    v[0] = {};
    v[1] = {};
    BestPrices best_prices{.S = S_,
                           .NoMDEntries = {.blockLength = 36, .numInGroup = 2},
                           .Entries = v};

    std::string output;
    auto cb = [&output](const std::string &json) { output = json; };
    simba::JsonCreator json_creator(std::move(cb));
    json_creator(simba::start_packet{});
    json_creator(best_prices);
    json_creator(simba::end_packet{});
    std::cout << output << std::endl;
    assert_true("json_best_prices_test", output == expected_json_output);
  }
  void json_order_execution_test() {

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
    json_creator(simba::start_packet{});
    json_creator(order_execution);
    json_creator(simba::end_packet{});
    assert_true("json_order_execution_test", output == expected_json_output);
  }
  void json_incremental_header_test() {

    simba::schema::structs::IncrementalPacketHeader incremental_header{
        .TransactTime = 1092, .ExchangeTradingSessionId = 2345864};

    std::string expected_json_output =
        std::string("{") + incremental_header_json_ + "}";
    expected_json_output += "\n";
    std::string output;
    auto cb = [&output](const std::string &json) { output = json; };
    simba::JsonCreator json_creator(std::move(cb));
    json_creator(simba::start_packet{});
    json_creator(incremental_header);
    json_creator(simba::end_packet{});
    assert_true("json_incremental_header_test", output == expected_json_output);
  }
  void json_mdp_header_test() {

    simba::schema::structs::MarketDataPacketHeader mdp_header{
        .MsgSeqNum = 19, .MsgSize = 84, .MsgFlags = 8, .SendingTime = 12344321};

    std::string expected_json_output =
        std::string(R"({)") + mdp_header_json_ + "}";
    expected_json_output += "\n";
    std::string output;
    auto cb = [&output](const std::string &json) { output = json; };
    simba::JsonCreator json_creator(std::move(cb));
    json_creator(simba::start_packet{});
    json_creator(mdp_header);
    json_creator(simba::end_packet{});

    assert_true("json_mdp_header_test", output == expected_json_output);
  }
  void json_order_update_test() {
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
    json_creator(simba::start_packet{});
    json_creator(order_update);
    json_creator(simba::end_packet{});
    assert_true("json_order_update_test", ans == expected_json_output);
  }

private:
  void assert_true(const std::string &test_name, bool expression) {
    std::cout << test_name << ": " << (expression ? "SUCCESS" : "FAILURE")
              << std::endl;
  }
  static constexpr simba::schema::structs::SBEHeader S_ = {
      .BlockLength = 10, .TemplateId = 30, .SchemaId = 20, .Version = 40};
  static inline const std::string sbe_header_json_ =
      R"({"SBEHeader": {"BlockLength": 10, "TemplateId": 30, "SchemaId": 20, "Version": 40})";
  static inline const std::string mdp_header_json_ =
      R"({"MDPHeader": {"MsgSeqNum": 19, "MsgSize": 84, "MsgFlags": 8, "SendingTime": 12344321}})";
  static inline const std::string incremental_header_json_ =
      R"({"IncrementalHeader": {"TransactTime": 1092, "ExchangeTradingSessionId": 2345864}})";
  std::ostream &os_;
};
} // namespace tests
int main() {
  std::string test_log = "test_" + std::to_string(getpid()) + ".txt";
  std::ofstream ofs(test_log, std::ios::out);
  tests::test_fixture F(ofs.is_open() ? ofs : std::cout);
  F.run_tests();
  return 0;
}
