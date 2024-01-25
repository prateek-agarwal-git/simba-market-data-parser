#pragma once
#include "application_layer_messages.h"
#include "schema/structs.h"
#include "schema/types.h"
#include "tag_structs.h"
#include <cassert>
#include <cstdint>
#include <functional>
#include <string_view>

namespace simba {

template <typename OutputFunctor, typename LogStream> struct ProtocolDecoder {
  ProtocolDecoder(OutputFunctor &output, LogStream &l) : output_(output), l_(l) {}

  void operator()(std::basic_string_view<uint8_t> packet);

private:
  template <typename T> T get_value(const uint8_t *&buffer) {
    T value = *reinterpret_cast<const T *>(buffer);
    buffer += sizeof(T);
    return value;
  }

  void check_null_and_update(const uint8_t *&buffer,
                             std::optional<int64_t> &opt) {
    int64_t val = get_value<int64_t>(buffer);
    if (val != schema::types::NullValues::Int64) {
      opt = val;
    }
  }
  void decode_incremental_packet(std::basic_string_view<std::uint8_t> buffer);
  void decode_snapshot_packet(std::basic_string_view<std::uint8_t> buffer);

  std::size_t process_best_prices(const uint8_t *buffer);
  std::size_t process_order_update(const uint8_t *buffer);
  std::size_t process_order_execution(const uint8_t *buffer);
  // made this a reference so that it can be accessed later for testing
  // purposes. See tests/decoder_output_functor.h
  OutputFunctor &output_;
  LogStream &l_;
};
} // namespace simba
// implementation in the below file
#include "protocol_decoder-tmpl.h"