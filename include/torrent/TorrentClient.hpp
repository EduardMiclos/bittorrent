#pragma once

#include "Peer.hpp"
#include "PeerConnection.hpp"
#include "Piece.hpp"
#include "TorrentParser.hpp"
#include "Tracker.hpp"

#include <cstdint>
#include <memory>
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

constexpr std::array<std::uint16_t, 5> torrentPorts{6881, 6882, 6883, 6884, 6885};

class TorrentClient {
private:
  TorrentParser parser;

  std::optional<Tracker> tracker;
  std::string infoHash;

  int64_t uploaded;
  int64_t downloaded;
  long long left;
  bool compact;

  uint16_t listeningPort;
  std::vector<Peer> peers;
  std::vector<std::shared_ptr<PeerConnection>> peerConnections;

  std::vector<Piece> pieces;
  std::vector<bool> pieceIsDownloaded;

  std::vector<std::jthread> peerThreads;
  std::map<std::string, std::thread::id> threadsMap;

public:
  std::string uid;

  TorrentClient();
  ~TorrentClient();
  std::vector<Peer> getPeers();
  void setPeers(std::vector<Peer> peers);
  bool setTorrent(std::string torrentPath);

  void perPeerDownload(Peer &peer);
  void startDownload();
  void stopDownload();
};
