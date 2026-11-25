#pragma once

#include "networking/TcpStream.hpp"
#include "torrent/Peer.hpp"
#include "torrent/PeerMessage.hpp"
#include "torrent/Piece.hpp"
#include <stop_token>

class PeerConnection {
public:
  TcpStream stream;
  std::string ipAddress;
  uint16_t port;

  bool peerChoking = true;
  bool peerInterested = false;

  bool amInterested = false;
  bool amChocking = true;

  size_t outstandingRequests = 0;
  const size_t MAX_OUTSTANDING_REQUESTS = 10;

  std::vector<bool> availablePieces;

  bool open();
  bool close();

  std::size_t getNextMessageLength();
  PeerMessage receiveMessage();
  bool sendMessage(PeerMessage message);

public:
  PeerConnection(std::string ipAddress, uint16_t port);

  bool isConnected();
  bool handshake(std::string clientUid, std::string infoHash);

  bool hasPiece(uint32_t pieceIndex) const;
  void updateBitfield(const std::vector<std::byte> &payload);
  void handleHave(uint32_t pieceIndex);

  Piece *getNextBestPiece(std::vector<Piece> &pieces, std::vector<bool> &pieceIsDownloaded);
  void initPeerListener(std::stop_token token);
  void initPeerTalker(std::stop_token token, std::vector<Piece> &pieces, std::vector<bool> &pieceIsDownloaded);
};
