#pragma once

#include <cstdint>
#include <string>
#include <string_view>

class Peer {
public:
  std::string ipAddress;
  uint16_t port;

public:
  Peer() = default;
  Peer(std::string_view peerBytes);
  Peer(std::string ipAddress, uint16_t port);
};
