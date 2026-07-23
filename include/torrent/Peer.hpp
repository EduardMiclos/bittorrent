#pragma once

#include <cstdint>
#include <string>

class Peer {
public:
  std::string ipAddress;
  uint16_t port;

public:
  Peer() = default;
  Peer(std::string_view peerBytes);
};
