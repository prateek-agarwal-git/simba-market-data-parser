#pragma once
#include <cstdint>
#include <string_view>
#include <string>
#include <iostream>

namespace simba::schema::enums {
enum class MDUpdateAction : std::uint8_t { New = 0, Change = 1, Delete = 2 };
enum class MDEntryType : char {
  Bid = '0',
  Offer = '1',
  EmptyBook = 'J'

};
inline std::string to_string(MDUpdateAction action){
  switch (action){
    case MDUpdateAction::New :return "New";
    case MDUpdateAction::Change:return "Change";
    case MDUpdateAction::Delete:return "Delete";
  }
  std::abort();
}

inline std::string to_string(MDEntryType entry){
  switch (entry){
    case MDEntryType ::Bid:return "Bid";
    case MDEntryType ::Offer:return "Offer";
    case MDEntryType ::EmptyBook:return "EmptyBook";
  }
  std::abort();
}

inline std::ostream& operator<<(std::ostream& os, MDUpdateAction update_action){
  os<< to_string(update_action);
  return os;

}
} // namespace simba_schema