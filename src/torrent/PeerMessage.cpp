
#include "Utils.hpp"
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

PeerMessage::PeerMessage(std::byte messageType) {
  this->messageType = messageType;
  this->payload = {};
}

PeerMessage::PeerMessage(const std::vector<std::byte> &message) {
  if (message.empty())
    throw std::invalid_argument("Empty peer message");

  if (!isValidMessageType(message[0]))
    throw std::invalid_argument("Unuspported peer message type");

  this->messageType = message[0];
  this->payload = std::vector(message.begin() + 1, message.end());
}

PeerMessage::PeerMessage(std::byte messageType, const std::vector<std::byte> message) {
  if (message.empty())
    throw std::invalid_argument("Empty peer message");

  if (!isValidMessageType(messageType))
    throw std::invalid_argument("Unsupported peer message type");

  this->messageType = messageType;
  this->payload = message;
}

std::vector<std::byte> PeerMessage::serialize() {
  uint32_t messageLength = 1 + payload.size();
  std::vector<std::byte> message;
  message.reserve(4 + messageLength);

  std::vector<std::byte> messageLengthBytes = uint32ToBytes(messageLength);

  message.insert(message.end(), messageLengthBytes.begin(), messageLengthBytes.end());
  message.push_back(this->messageType);
  message.insert(message.end(), this->payload.begin(), this->payload.end());

  return message;
}
