#include "torrent/PeerConnection.hpp"
#include <iomanip>
#include <iostream>

PeerConnection::PeerConnection(Peer peer) { this->peer = peer; }

bool PeerConnection::open() {
  if (this->stream.isOpen) {
    std::cerr << "TCP connection is already open";
    return false;
  }

  this->stream = TcpStream();
  if (!this->stream.connect(this->peer.ipAddress, this->peer.port))
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

  uint32_t length = (uint8_t(lengthStr[0]) << 24) |
                    (uint8_t(lengthStr[1]) << 16) |
                    (uint8_t(lengthStr[2]) << 8) | uint8_t(lengthStr[3]);
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

bool PeerConnection::sendMessage(char messageId, std::string payload) {
  if (!this->stream.isOpen) {
    std::cerr << "Cannot send message if connection is closed!";
    return false;
  }

  uint32_t messageLength = 1 + payload.size();
  std::cout << "message length: " << messageLength << '\n';

  // bittorrent protocol message buffer between peers always starts with the
  // length of the message
  char message_length[4];

  std::string message;
  message.reserve(4 + messageLength);
  message.push_back(static_cast<char>((messageLength >> 24)));

  message.push_back(static_cast<char>((messageLength >> 16)));

  message.push_back(static_cast<char>((messageLength >> 8)));

  message.push_back(static_cast<char>((messageLength)));

  std::cout << "length in bytes: " << message_length[0] << '\n';
  std::cout << "message id: " << messageId << '\n';

  message += messageId + payload;
  for (unsigned char byte : message) {
    std::cout << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<int>(byte) << ' ';
  }

  size_t bytesSent = this->stream.writeAll(message);
  return bytesSent == message.size();
}

bool PeerConnection::handshake(std::string clientUid, std::string infoHash) {
  if (!this->open())
    return false;

  std::cout << "Initiating handshake...\n";

  std::string handshakeBytes = std::string(1, char(19)) +
                               "BitTorrent protocol" + std::string(8, '\0') +
                               infoHash + clientUid;

  this->stream.writeAll(handshakeBytes);
  std::string peerResponse = this->stream.readExact(68);

  std::cout << "peer response: " << peerResponse << '\n';
  if (peerResponse.size() != 68 &&
      peerResponse.compare(28, 20, infoHash) != 0) {
    std::cerr << "Peer returned a wrong info hash...";
    return false;
  }

  std::cout << "Received " << peerResponse.size() << " bytes\n";

  for (unsigned char byte : peerResponse) {
    std::printf("%02X ", byte);
  }

  return true;
}
