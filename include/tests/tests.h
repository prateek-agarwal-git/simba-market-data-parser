#pragma once
#include "schema/structs.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

namespace tests {

class test_fixture {
public:
  test_fixture(std::ostream &os) : os_(os) {}
  void run_tests();

private:
  // CURDIR is a compile time definition. See Makefile
  static inline const std::string current_directory_path =
      std::string(CURDIR) + "/";
  static constexpr simba::schema::structs::SBEHeader S_ = {
      .BlockLength = 10, .TemplateId = 30, .SchemaId = 20, .Version = 40};
  static inline const std::string sbe_header_json_ =
      R"({"SBEHeader": {"BlockLength": 10, "TemplateId": 30, "SchemaId": 20, "Version": 40})";

  void run_json_creator_tests();
  void run_packet_reader_tests();
  void run_protocol_decoder_tests();
  void run_stream_writer_tests();
  void run_basic_stream_writer_test();
  void run_integration_tests();
  void packet_reader_1();
  void packet_reader_2();
  void packet_reader_3();
  void packet_reader_4();
  void packet_reader_5();
  void json_best_prices_test();
  void json_order_execution_test();
  void json_incremental_header_test();
  void json_mdp_header_test();
  void json_order_update_test();
  void json_order_book_snapshot_test();

  void decode_best_prices_test();
  void decode_order_update_test();
  void decode_order_execution_test();
  void decode_order_book_snapshot_test();

  void integration_test_1();
  void integration_test_2();
  void integration_test_3();
  void integration_test_4();
  void integration_test_5();
  void assert_true(const std::string &test_name, bool expression);

  template <typename T> void update_buffer(uint8_t *&buffer, T val) {
    std::memcpy(buffer, &val, sizeof(T));
    buffer += sizeof(T);
  }

  std::ostream &os_;
};

} // namespace tests