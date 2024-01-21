#include <cstdint>

namespace simba::schema::enums {
enum class MDUpdateAction : std::uint8_t { New = 0, Change = 1, Delete = 2 };
enum class MDEntryType : char {
  // verify this
  Bid = '0',
  Offer = '1',
  EmptyBook = 'J'

};
} // namespace simba_schema