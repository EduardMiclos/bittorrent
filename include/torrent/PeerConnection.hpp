#include "networking/TcpStream.hpp"
#include "torrent/Peer.hpp"

// based on: https://www.bittorrent.org/beps/bep_0003.html

namespace PeerConnectionStatus {

// not willing to send data
inline constexpr std::byte Choke{0}; // no payload

// ready to send data
inline constexpr std::byte Unchoke{1}; // no payload

// interested in data
inline constexpr std::byte Interested{2}; // no payload

// not interested in data
inline constexpr std::byte Uninterested{3}; // no payload

// has a specific data piece
inline constexpr std::byte Have{4};

// which pieces are available
inline constexpr std::byte Bitfield{
    5}; // payload: a bitfield with each index that downloader has sent set to 1
        // and the rest set to 0

// ask for a block
inline constexpr std::byte Request{6};

// a block of data
inline constexpr std::byte Piece{7};

// send towards the end of download
inline constexpr std::byte Cancel{8};
}; // namespace PeerConnectionStatus

class PeerConnection {
private:
  Peer peer;
  TcpStream stream;

  bool open();
  bool close();

  std::size_t getNextMessageLength();
  std::string receiveMessage();
  bool sendMessage(std::string message);

public:
  PeerConnection(Peer);

  bool handshake(std::string clientUid, std::string infoHash);
};
