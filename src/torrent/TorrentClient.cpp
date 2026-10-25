#include "Config.hpp"
#include "Utils.hpp"
#include "networking/HttpClient.hpp"
#include "torrent/TorrentClient.hpp"
#include "torrent/Tracker.hpp"
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
}

void TorrentClient::setPeers(std::vector<Peer> peers) { this->peers = peers; }

void TorrentClient::perPeerDownload(Peer peer) {

  PeerConnection peerConnection = PeerConnection(peer.ipAddress, peer.port);

  // initiate handshake with peer
  bool handshakeSuccess = peerConnection.handshake(this->uid, this->infoHash);
  if (!handshakeSuccess) {
    std::cout << "Error during peer handshake [peer: " << peer.ipAddress << ":" << peer.port << "]\n";
    return;
  }

  while (peerConnection.isConnected()) {
    PeerMessage message = peerConnection.receiveMessage();

    swich(message.type) {
    case PeerMessageType::Bitfield:
      break;
    }
  }

  // start listening for peer messages on new thread

  // advertise as interested for downloading
  peerConnection.sendStatus(PeerStatus::Interested);
  std::byte status = peerConnection.receiveStatus();
  peerConnection.lastStatus = status;
  if (status == PeerStatus::Unchoke) {
    // ask for piece
  }

  peerConnection.close();
}

void TorrentClient::startDownload() {
  if (this->peers.size() == 0) {
    std::cerr << "No peers available for download.\n";
    return;
  }

  for (Peer peer : this->peers) {
    // start a new thread for each peer connection
    this->peerThreads.emplace_back([this, peer]() { perPeerDownload(peer); });

    PeerConnection peerConnection = PeerConnection(peer.ipAddress, peer.port);

    // initiate handshake with peer
    bool handshakeSuccess = peerConnection.handshake(this->uid, this->infoHash);

    if (handshakeSuccess) {
      // start listening for peer messages
      // start new thread

      // advertise as interested for downloading
      peerConnection.sendStatus(PeerStatus::Interested);
      std::byte status = peerConnection.receiveStatus();

      // good news, peer is willing to transfer pieces
      if (status == PeerStatus::Unchoke) {
        // ask for piece
      }
    } else {
      peerConnection.close();
    }
  }
}

bool TorrentClient::setTorrent(std::string torrentPath) {
  std::cout << "Torrent client initialized...\n";

  this->parser.load(torrentPath);

  this->tracker = Tracker(this->parser.getTrackerURL());
  this->infoHash = this->parser.getInfoHash();
  this->left = this->parser.getTotalSize();

  for (int i = 0; i < this->parser.getPieceCount(); i++)
    this->pieces.emplace_back(this->parser.getPieceSizeAt(i), this->parser.getPieceHashAt(i), i);

  std::vector<Peer> peers = this->tracker.announce(
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
  return true;
}
