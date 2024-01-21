#include "json_creator.h"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <cassert>

namespace tests {
class test_fixture {
public:
  test_fixture(std::ostream &os) : os_(os) {}
  void run_tests() { basic_json_creator_test(); }
  void basic_json_creator_test() {
    simba::messages::application_layer::OrderUpdate order_update{
        .S = {.BlockLength = 10,
              .TemplateId = 30,
              .SchemaId = 20,
              .Version = 40},
        .MDEntryId = 100,
        .MDentryPx = {.mantissa = 1234567},
        .MDEntrySize = 25,
        .MDFlags = 0x1,
        .MDFlags2 = 0x1,
        .SecurityId = 84,
        .RptSeq = 123432,
        .MDUpdateAction = simba::schema::enums::MDUpdateAction::New,
        .MDEntryType = simba::schema::enums::MDEntryType::Offer};
        std::string expected_json_output = 
        R"({"OrderUpdate": {"SBEHeader": {"BlockLength": 10, "TemplateId": 30, "SchemaId": 20, "Version": 40}}})";
        ;
//    std::string expected_json_output =
//        R"({"OrderUpdate": {"SBEHeader":{"BlockLength": 10, "TemplateId": 30, "SchemaId": 20, "Version": 40}, "MDEntryId": 100, "MDEntryPx": 12.34567, "MDFlags":"DAY", "MDFlags2": "Zero", "SecurityId": 84, "RptSeq": 123432, "MDUpdateAction": "New", "MDEntryType": "Offer"}})";
    std::string ans;
    auto cb = [&ans](const std::string &json) { ans = json; };
    simba::JsonCreator json_creator(std::move(cb));

    json_creator(order_update);
    std::cout << ans << std::endl;
    std::cout << expected_json_output<< std::endl;
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
