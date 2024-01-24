#pragma once
#include "application_layer_messages.h"
#include "schema/structs.h"
#include "tag_structs.h"

namespace tests {
// this class plays the role of json creator in the tests for testing protocol
// decoder
struct DecoderOutputFunctor {
  void operator()(simba::tag_structs::start_packet) {}
  void operator()(simba::tag_structs::end_packet) {}
  void
  operator()(const simba::schema::structs::MarketDataPacketHeader &mdp_header) {
    mdp_header_ = mdp_header;
  }
  void operator()(
      const simba::schema::structs::IncrementalPacketHeader &inc_header) {
    inc_header_ = inc_header;
  }
  void operator()(
      const simba::messages::application_layer::BestPrices &best_prices) {
    best_prices_ = best_prices;
  }
  void operator()(
      const simba::messages::application_layer::OrderUpdate &order_update) {
    order_update_ = order_update;
  }
  void operator()(const simba::messages::application_layer::OrderExecution
                      &order_execution) {
    order_execution_ = order_execution;
  }
  void operator()(const simba::messages::application_layer::OrderBookSnapShot
                      &order_book_snapshot) {
    order_book_snapshot_ = order_book_snapshot;
  }
  auto mdp_header() const { return mdp_header_; }
  auto inc_header() const { return inc_header_; }
  auto best_prices() const { return best_prices_; }
  auto order_update() const {return order_update_; }
  auto order_execution() const {return order_execution_; }
  auto order_book_snapshot() const {return order_book_snapshot_;}
private:
  simba::schema::structs::MarketDataPacketHeader mdp_header_{};
  simba::schema::structs::IncrementalPacketHeader inc_header_{};
  simba::messages::application_layer::BestPrices best_prices_{};
  simba::messages::application_layer::OrderUpdate order_update_{};
  simba::messages::application_layer::OrderExecution order_execution_{};
  simba::messages::application_layer::OrderBookSnapShot order_book_snapshot_{};
};
} // namespace tests