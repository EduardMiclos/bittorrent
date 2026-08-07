#include "torrent/Peer.hpp"
#include <cstdint>
#include <cstdlib>
#include <string>

Peer::Peer(std::string_view peerBytes) {
  this->ipAddress = std::to_string((unsigned char)peerBytes[0]) + '.' +
                    std::to_string((unsigned char)peerBytes[1]) + '.' +
                    std::to_string((unsigned char)peerBytes[2]) + '.' +
                    std::to_string((unsigned char)peerBytes[3]);

  this->port = std::uint16_t(std::uint8_t(peerBytes[4] << 8) |
                             std::uint8_t(peerBytes[5]));
}

Peer::Peer(std::string ipAddress, uint16_t port) {
  this->ipAddress = ipAddress;
  this->port = port;
}
