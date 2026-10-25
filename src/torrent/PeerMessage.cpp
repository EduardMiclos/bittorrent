
#include "torrent/PeerMessage.hpp"
#include <stdexcept>
#include <stdint.h>

bool PeerMessage::isValidMessageType(std::byte messageType) {
  switch (messageType) {
  case PeerMessageType::Choke:
  case PeerMessageType::Unchoke:
  case PeerMessageType::Interested:
  case PeerMessageType::Uninterested:
  case PeerMessageType::Have:
  case PeerMessageType::Bitfield:
  case PeerMessageType::Request:
  case PeerMessageType::Piece:
  case PeerMessageType::Cancel:
    return true;

  default:
    return false;
  }
}

PeerMessage::PeerMessage(const std::vector<std::byte> &message) {
  if (message.empty())
    throw std::invalid_argument("Empty peer message");

  if (!isValidMessageType(message[0]))
    throw std::invalid_argument("Unuspported peer message type");

  this->messageType = message[0];
  this->payload = std::vector(message.begin() + 1, message.end());
}

std::vector<std::byte> PeerMessage::serialize() {
  uint32_t messageLength = 1 + payload.size();
  std::vector<std::byte> message;
  message.reserve(4 + messageLength);

  message.push_back(static_cast<std::byte>(messageLength >> 24));
  message.push_back(static_cast<std::byte>(messageLength >> 16));
  message.push_back(static_cast<std::byte>(messageLength >> 8));
  message.push_back(static_cast<std::byte>(messageLength));
  message.push_back(this->messageType);

  message.insert(message.end(), this->payload.begin(), this->payload.end());

  return message;
}
