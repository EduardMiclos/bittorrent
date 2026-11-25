#include "torrent/PeerConnection.hpp"
#include "Utils.hpp"
#include "torrent/Piece.hpp"
#include "torrent/TorrentClient.hpp"
#include <iostream>
#include <stdexcept>
#include <stop_token>

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

  // std::cout < < < < '\n';

  if (message.size() != messageLength) {
    std::cerr << "Peer response truncated";
    return {};
  }

  return PeerMessage(message);
}

bool PeerConnection::sendMessage(PeerMessage message) {
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

bool PeerConnection::hasPiece(uint32_t pieceIndex) const { return this->availablePieces[pieceIndex]; }

void PeerConnection::updateBitfield(const std::vector<std::byte> &payload) {
  if (!this->availablePieces.size())
    this->availablePieces.assign(payload.size() * 8, false);

  short multiplier = 1;
  for (auto bitfieldByte : payload) {
    uint8_t byteInt = static_cast<uint8_t>(bitfieldByte);

    short i = 1;
    while (byteInt > 0) {
      this->availablePieces.at(8 * multiplier - i) = ((byteInt & 1) != 0);
      byteInt = (byteInt >> 1);
      i++;
    }

    multiplier += 1;
  }
}

void PeerConnection::handleHave(const uint32_t pieceIndex) {
  if (pieceIndex < 0 || pieceIndex >= this->availablePieces.size()) {
    throw std::invalid_argument("Piece index out of range");
    return;
  }

  this->availablePieces[pieceIndex] = true;
}

void PeerConnection::initPeerListener(std::stop_token token) {
  while (!token.stop_requested() && this->isConnected()) {
    PeerMessage message = this->receiveMessage();

    switch (message.messageType) {
    case PeerMessageType::Bitfield:
      this->updateBitfield(message.payload);
      break;

    case PeerMessageType::Have: {
      uint32_t haveValue = bytesToUint32(message.payload);
      this->handleHave(haveValue);
      break;
    }

    case PeerMessageType::Piece: {
    }

    case PeerMessageType::Choke:
      this->peerChoking = true;
      break;

    case PeerMessageType::Unchoke:
      this->peerChoking = false;
      break;

    case PeerMessageType::Interested:
      this->peerInterested = true;
      // do something...?
      break;

    case PeerMessageType::Uninterested:
      this->peerInterested = false;
      // do something...?
      break;

    case PeerMessageType::Request:
      // do something...?
      break;

    case PeerMessageType::Cancel:
      // do something...?
      break;

    default:
      break;
    }
  }
}

Piece *PeerConnection::getNextBestPiece(std::vector<Piece> &pieces, std::vector<bool> &pieceIsDownloaded) {
  for (size_t pieceIndex = this->availablePieces[0]; pieceIndex < this->availablePieces.size(); pieceIndex++)
    if (this->availablePieces[pieceIndex] && !pieceIsDownloaded[pieceIndex])
      return &pieces[pieceIndex];

  return nullptr;
}

void PeerConnection::initPeerTalker(std::stop_token token, std::vector<Piece> &pieces,
                                    std::vector<bool> &pieceIsDownloaded) {
  while (!token.stop_requested() && this->isConnected()) {
    if (!this->amInterested) {
      this->sendMessage(PeerMessage(PeerMessageType::Interested));
      this->amInterested = true;
    }

    if (!this->peerChoking && this->availablePieces.size()) {
      if (this->outstandingRequests < MAX_OUTSTANDING_REQUESTS) {
        Piece *nextPiece = this->getNextBestPiece(pieces, pieceIsDownloaded);

        if (nextPiece != nullptr) {
          auto chunkIndex = nextPiece->getNextChunk();

          if (chunkIndex >= 0) {
            auto request = nextPiece->serializeRequest(chunkIndex);
            PeerMessage message = PeerMessage(PeerMessageType::Request, request);

            if (this->sendMessage(message)) {
              nextPiece->setChunk(chunkIndex, ChunkState::Requested);
              this->outstandingRequests++;
            }
          }
        }
      }
    }
  }
}
