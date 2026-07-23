#include <cstdint>
#include <string>

class Peer {
  std::string ipAddress;
  uint16_t port;

public:
  Peer() = default;
  Peer(std::string_view peerBytes);
};
