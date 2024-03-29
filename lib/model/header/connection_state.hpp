#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

#include "enum_hash.hpp"

namespace mbd {

enum class connection_state : std::uint8_t {
  VALID = 0,
  CONNECTED,
  NOT_CONNECTED,
  ERR_SAME_DIRECTION,
  ERR_DIFF_TYPE,
  ERR_IN_PORT_INVALID,
  ERR_OUT_PORT_INVALID,
  ERR_IN_PORT_CONNECTED,
  ERR_SAME_MODEL
};

const inline std::string connection_info(connection_state state) {
  const std::unordered_map<connection_state, std::string, enum_class_hash>
      info = {
          {connection_state::VALID, "Valid connection"},
          {connection_state::CONNECTED, "Connection established"},
          {connection_state::NOT_CONNECTED, "Not connected"},
          {connection_state::ERR_SAME_MODEL,
           "Error cannot connect ports on the same model."},
          {connection_state::ERR_SAME_DIRECTION, "Error same port direction"},
          {connection_state::ERR_DIFF_TYPE, "Error ports have different types"},
          {connection_state::ERR_IN_PORT_INVALID,
           "Error input port index is invalid"},
          {connection_state::ERR_OUT_PORT_INVALID,
           "Error output port index is invalid"},
          {connection_state::ERR_IN_PORT_CONNECTED,
           "Error input port is already connected"}};

  return info.at(state);
}

} // namespace mbd