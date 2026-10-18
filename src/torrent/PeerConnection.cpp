#include "torrent/PeerConnection.hpp"
#include <iomanip>
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

bool PeerConnection::sendStatus(std::byte status) {
  return this->sendMessage(status, "");
}

std::byte PeerConnection::receiveStatus() {
  std::string receivedMessage = this->receiveMessage();
  const char *c = receivedMessage.c_str();

  std::byte status = static_cast<std::byte>(c[0]);
  this->lastStatus = status;

  return status;
}

bool PeerConnection::sendMessage(std::byte messageId, std::string payload) {
  if (!this->stream.isOpen) {
    std::cerr << "Cannot send message if connection is closed!";
    return false;
  }

  uint32_t messageLength = 1 + payload.size();

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
  std::cout << "message id: " << (char)messageId << '\n';

  message += (char)messageId + payload;
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

  std::string handshakeBytes = std::string(1, char(19)) +
                               "BitTorrent protocol" + std::string(8, '\0') +
                               infoHash + clientUid;

  this->stream.writeAll(handshakeBytes);
  std::string peerResponse = this->stream.readExact(68);

  if (peerResponse.size() != 68 &&
      peerResponse.compare(28, 20, infoHash) != 0) {
    std::cerr << "Peer returned a wrong info hash...";
    return false;
  }

  for (unsigned char byte : peerResponse) {
    std::printf("%02X ", byte);
  }

  return true;
}
