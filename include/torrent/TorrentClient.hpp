#pragma once

#include "Peer.hpp"
#include "PeerConnection.hpp"
#include "Piece.hpp"
#include "TorrentParser.hpp"
#include "Tracker.hpp"

#include <cstdint>
#include <string>
#include <thread>

namespace TrackerEvent {

// send if it's the first request to the tracker
inline constexpr std::string_view Started = "started";

// send to the tracker if the client shuts down gracefully
inline constexpr std::string_view Stopped = "stopped";

// send to the tracker when the download completes
inline constexpr std::string_view Completed = "completed";

} // namespace TrackerEvent

constexpr std::array<std::uint16_t, 5> torrentPorts{6881, 6882, 6883, 6884,
                                                    6885};

class TorrentClient {
private:
  TorrentParser parser;

  Tracker tracker;
  std::string infoHash;

  int64_t uploaded;
  int64_t downloaded;
  long long left;
  bool compact;

  uint16_t listeningPort;
  std::vector<Peer> peers;
  std::vector<PeerConnection> peerConnections;

  std::vector<Piece> pieces;

  std::vector<std::jthread> peerThreads;

public:
  std::string uid;

  TorrentClient();
  void setPeers(std::vector<Peer> peers);
  bool setTorrent(std::string torrentPath);

  void perPeerDownload(Peer peer);
  void startDownload();
};
