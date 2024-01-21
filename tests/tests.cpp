#include "json_creator.h"
#include <iostream>
#include <unistd.h>
#include <fstream>

namespace tests {
class test_fixture {
    public:
     test_fixture(std::ostream &os):os_(os){}
    void run_tests(){
      basic_json_creator_test();
      

    }
    void basic_json_creator_test(){
      std::string ans;
      auto cb = [&ans](const std::string& json){
        ans = json;
      };
      simba::JsonCreator json_creator(std::move(cb));
      json_creator(simba::messages::application_layer::OrderUpdate{} );
      os_<< ans<<std::endl;
    }
    private:
    std::ostream &os_;

};
}
int main() {
  std::string test_log = "test_" + std::to_string(getpid()) + ".txt";
  std::ofstream ofs(test_log, std::ios::out);
  tests::test_fixture F(ofs.is_open() ? ofs : std::cout);
  F.run_tests();
  return 0;
}
