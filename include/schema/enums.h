#include <cstdint>
#include <string_view>

namespace simba::schema::enums {
enum class MDUpdateAction : std::uint8_t { New = 0, Change = 1, Delete = 2 };
enum class MDEntryType : char {
  Bid = '0',
  Offer = '1',
  EmptyBook = 'J'

};
inline std::string_view sv(MDUpdateAction action){
  switch (action){
    case MDUpdateAction::New :return "New";
    case MDUpdateAction::Change:return "Change";
    case MDUpdateAction::Delete:return "Delete";
  }
  std::abort();
}

inline std::string_view sv(MDEntryType entry){
  switch (entry){
    case MDEntryType ::Bid:return "Bid";
    case MDEntryType ::Offer:return "Offer";
    case MDEntryType ::EmptyBook:return "EmptyBook";
  }
  std::abort();
}
} // namespace simba_schema