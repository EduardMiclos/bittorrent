#include "Config.hpp"
#include "Utils.hpp"
#include "networking/HttpClient.hpp"
#include "torrent/TorrentClient.hpp"
#include "torrent/Tracker.hpp"
#include <memory>
#include <thread>

TorrentClient::TorrentClient() {
  this->parser = TorrentParser();
  this->uid = generateRandomString(CLIENT_UID_SIZE);
  this->uploaded = 0;
  this->downloaded = 0;
  this->left = 0;

  // set this to true to accept compact responses from trackers;
  // trackers do this in order to save bandwith, I guess it's better to support
  // it than not to, considering that there are trackers that enforce compact
  // responses; best to always be set to true
  this->compact = true;
  this->listeningPort = torrentPorts[0];
  this->peers = {};

  this->pieces = {};
  this->pieceIsDownloaded = {};
}
TorrentClient::~TorrentClient() { this->stopDownload(); }
std::vector<Peer> TorrentClient::getPeers() { return this->peers; }
void TorrentClient::setPeers(std::vector<Peer> peers) { this->peers = peers; }

void TorrentClient::startDownload() {
  if (this->peers.empty()) {
    std::cerr << "No peers available for download.\n";
    return;
  }

  for (const Peer &peer : this->peers) {
    // start a new thread for each peer connection
    auto conn = std::make_shared<PeerConnection>(peer.ipAddress, peer.port);

    if (!conn->handshake(this->uid, this->infoHash)) {
      std::cerr << "Error during peer handhake [peer: " << peer.ipAddress << ":" << peer.port << "]\n";
      return;
    }

    this->peerConnections.push_back(conn);

    this->peerThreads.emplace_back([conn](std::stop_token stopToken) { conn->initPeerListener(stopToken); });
    this->threadsMap.emplace(peer.uid + "_LISTENER", peerThreads.back().get_id());

    this->peerThreads.emplace_back([this, conn](std::stop_token stopToken) {
      conn->initPeerTalker(stopToken, this->pieces, this->pieceIsDownloaded);
    });
    this->threadsMap.emplace(peer.uid + "_TALKER", peerThreads.back().get_id());
  }
}

void TorrentClient::stopDownload() {
  for (auto &thread : this->peerThreads)
    thread.request_stop();

  for (auto &conn : peerConnections)
    conn->close();

  for (auto &thread : this->peerThreads)
    if (thread.joinable())
      thread.join();

  this->peerThreads.clear();
  this->threadsMap.clear();
  this->peerConnections.clear();
}

bool TorrentClient::setTorrent(std::string torrentPath) {
  std::cout << "Torrent client initialized...\n";

  this->parser.load(torrentPath);

  auto trackerURL = this->parser.getTrackerURL();

  if (!trackerURL.empty()) {
    this->tracker.emplace(trackerURL);
  }

  this->infoHash = this->parser.getInfoHash();
  this->left = this->parser.getTotalSize();

  this->pieceIsDownloaded.resize(this->parser.getPieceCount(), false);
  for (int i = 0; i < this->parser.getPieceCount(); i++)
    this->pieces.emplace_back(this->parser.getPieceSizeAt(i), this->parser.getPieceHashAt(i), i);

  if (this->tracker) {
    std::vector<Peer> peers = this->tracker->announce(
        {UrlParameter("info_hash", this->infoHash), UrlParameter("peer_id", this->uid),
         UrlParameter("port", std::to_string(this->listeningPort)),
         UrlParameter("uploaded", std::to_string(this->uploaded)),
         UrlParameter("downloaded", std::to_string(this->downloaded)), UrlParameter("left", std::to_string(this->left)),
         UrlParameter("compact", this->compact ? "1" : "0"), UrlParameter("event", TrackerEvent::Started)});

    if (!peers.size()) {
      std::cerr << "The tracker didn't return any peer.\n";
      return false;
    }
    this->peers = peers;
  } else
    std::cout << "No tracker available. Please configure peers manually!\n";
  return true;
}
