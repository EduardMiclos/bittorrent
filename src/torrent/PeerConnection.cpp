#include "torrent/PeerConnection.hpp"
#include <iostream>

PeerConnection::PeerConnection(std::string ipAddress, uint16_t port) {
  this->ipAddress = ipAddress;
  this->port = port;
}

bool PeerConnection::open() {
  if (this->stream.isOpen) {
    std::cerr << "TCP connection is already open";
    return false;
  }

  this->stream = TcpStream();
  if (!this->stream.connect(this->ipAddress, this->port))
    return true;

  std::cerr << "Failed connecting to peer...\n";
  return false;
}

bool PeerConnection::close() {
  if (!this->stream.isOpen) {
    std::cerr << "Cannot close a connection that is not open!";
    return false;
  }

  return this->stream.close();
}

bool PeerConnection::isConnected() { return this->stream.isOpen; }

std::size_t PeerConnection::getNextMessageLength() {
  if (!this->stream.isOpen) {
    std::cerr << "Cannot get message length if connection is closed!";
    return 0;
  }
  std::vector<std::byte> lengthBytes = this->stream.readExactBytes(4);

  uint32_t length =
      uint8_t(lengthBytes[0]) | uint8_t(lengthBytes[1]) | uint8_t(lengthBytes[2]) | uint8_t(lengthBytes[3]);

  return length;
}

PeerMessage PeerConnection::receiveMessage() {
  if (!this->stream.isOpen) {
    std::cerr << "Cannot receive message if connection is closed!";
    return {};
  }

  std::size_t messageLength = this->getNextMessageLength();
  std::vector<std::byte> message = this->stream.readExactBytes(messageLength);
  if (message.size() != messageLength) {
    std::cerr << "Peer response truncated";
    return {};
  }

  return PeerMessage(message);
}

bool PeerConnection::sendMessage(PeerMessage &message) {
  if (!this->stream.isOpen) {
    std::cerr << "Cannot send message if connection is closed!";
    return false;
  }

  std::vector<std::byte> bytes = message.serialize();
  return this->stream.writeAllBytes(bytes) == bytes.size();
}

bool PeerConnection::handshake(std::string clientUid, std::string infoHash) {
  if (!this->open())
    return false;

  std::string handshakeBytes =
      std::string(1, char(19)) + "BitTorrent protocol" + std::string(8, '\0') + infoHash + clientUid;

  this->stream.writeAll(handshakeBytes);
  std::string peerResponse = this->stream.readExact(68);

  if (peerResponse.size() != 68 && peerResponse.compare(28, 20, infoHash) != 0) {
    std::cerr << "Peer returned a wrong info hash...";
    return false;
  }
  return true;
}
