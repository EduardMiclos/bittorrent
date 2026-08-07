#include "torrent/PeerConnection.hpp"
#include <iostream>

PeerConnection::PeerConnection(Peer peer) { this->peer = peer; }

bool PeerConnection::open() {
  if (this->stream.isOpen) {
    std::cerr << "TCP connection is already open";
    return false;
  }

  this->stream = TcpStream();
  if (this->stream.connect(this->peer.ipAddress, this->peer.port))
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

std::size_t PeerConnection::getNextMessageLength() {
  if (!this->stream.isOpen) {
    std::cerr << "Cannot get message length if connection is closed!";
    return 0;
  }
  std::string lengthStr = this->stream.readExact(4);

  uint32_t length = (lengthStr[0] << 24) | (lengthStr[1] << 16) |
                    (lengthStr[2] << 8) | lengthStr[3];
  return length;
}

std::string PeerConnection::receiveMessage() {
  if (!this->stream.isOpen) {
    std::cerr << "Cannot receive message if connection is closed!";
    return "";
  }

  std::size_t messageLength = this->getNextMessageLength();
  std::string message = this->stream.readExact(messageLength);
  if (message.size() != messageLength) {
    std::cerr << "Peer response truncated";
    return "";
  }
  return message;
}

bool PeerConnection::sendMessage(std::string message) {
  if (!this->stream.isOpen) {
    std::cerr << "Cannot send message if connection is closed!";
    return false;
  }
  uint32_t messageLength = message.size();

  char length[] = {char(messageLength >> 24), char(messageLength >> 16),
                   char(messageLength >> 8), char(messageLength)};

  // bittorrent protocol message buffer between peers always starts with the
  // length of the message
  message = std::string(length, 4) + message;
  size_t bytesSent = this->stream.writeAll(message);
  return bytesSent == message.size();
}

bool PeerConnection::handshake(std::string clientUid, std::string infoHash) {
  std::string handshakeBytes = std::string(1, char(19)) +
                               "BitTorrent protocol" + std::string(8, '\0') +
                               infoHash + clientUid;

  // this->sendMessage(handshakeBytes);

  std::string reply = this->receiveMessage();

  if (reply.compare(28, 20, infoHash) != 0) {
    std::cout << "Peer returned a wrong info hash...";
    return false;
  }

  return true;
}
