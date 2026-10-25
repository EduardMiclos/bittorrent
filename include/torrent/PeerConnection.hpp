#include "networking/TcpStream.hpp"
#include "torrent/Peer.hpp"
#include "torrent/PeerMessage.hpp"

class PeerConnection {
public:
  TcpStream stream;
  std::string ipAddress;
  uint16_t port;

  bool peerChoking = true;
  bool amInterested = false;

  bool open();
  bool close();

  std::size_t getNextMessageLength();
  PeerMessage receiveMessage();
  bool sendMessage(PeerMessage &message);
  bool sendStatus(std::byte status);
  std::byte receiveStatus();

public:
  PeerConnection(std::string ipAddress, uint16_t port);

  bool isConnected();
  bool handshake(std::string clientUid, std::string infoHash);
};
