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
    basic_order_update_test();
    basic_mdp_header_test();
  }
  void basic_mdp_header_test() {

    simba::schema::structs::MarketDataPacketHeader mdp_header{
        .MsgSeqNum = 19, .MsgSize = 84, .MsgFlags = 8, .SendingTime = 12344321};

    std::string expected_json_output =
        R"({{"MDPHeader": {"MsgSeqNum": 19, "MsgSize": 84, "MsgFlags": 8, "SendingTime": 12344321}}})";
    expected_json_output += "\n";
    std::string output;
    auto cb = [&output](const std::string &json) { output = json; };
    simba::JsonCreator json_creator(std::move(cb));
    json_creator(simba::start_packet{});
    json_creator(mdp_header);
    json_creator(simba::end_packet{});
    assert(output == expected_json_output);
  }
  void basic_order_update_test() {
    simba::messages::application_layer::OrderUpdate order_update{
        .S = {.BlockLength = 10,
              .TemplateId = 30,
              .SchemaId = 20,
              .Version = 40},
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
        R"({{"OrderUpdate": {"SBEHeader": {"BlockLength": 10, "TemplateId": 30, "SchemaId": 20, "Version": 40}, "MDEntryId": 100, "MDEntryPx": 12.34567, "MDEntrySize": 25, "MDFlags": "DAY", "MDFlags2": "Zero", "SecurityId": 84, "RptSeq": 123432, "MDUpdateAction": "New", "MDEntryType": "Offer"}}})";
    expected_json_output += "\n";
    std::string ans;
    auto cb = [&ans](const std::string &json) { ans = json; };
    simba::JsonCreator json_creator(std::move(cb));
    json_creator(simba::start_packet{});
    json_creator(order_update);
    json_creator(simba::end_packet{});
    assert(ans == expected_json_output);
  }

private:
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
