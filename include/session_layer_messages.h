#include "schema_structs.h"
#include "schema_types.h"
#include <string>

namespace simba::messages::session_layer {
struct Logon {
  schema::structs::SBEHeader S;
};

struct Logout {
  schema::structs::SBEHeader S;
  schema::types::String256 Text; // tag =  58
};

struct HeartBeat {
  schema::structs::SBEHeader S;
};

struct SequenceReset {
  schema::structs::SBEHeader S;
  std::uint32_t NewSeqNo; // tag = 36
};
} // namespace simba::messages::session_layer