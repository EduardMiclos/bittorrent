#include "Utils.hpp"
#include "networking/HttpClient.hpp"
#include "torrent/TorrentClient.hpp"
#include "torrent/Tracker.hpp"

TorrentClient::TorrentClient() {
  this->parser = TorrentParser();
  this->uid = generateRandomString(20);
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
}

bool TorrentClient::setTorrent(std::string torrentPath) {
  this->parser.load(torrentPath);

  this->tracker = Tracker(this->parser.getTrackerURL());
  this->infoHash = this->parser.getInfoHash();
  this->left = this->parser.getFilesLength();

  std::vector<Peer> peers = this->tracker.announce(
      {UrlParameter("info_hash", this->infoHash),
       UrlParameter("peer_id", this->uid),
       UrlParameter("port", std::to_string(this->listeningPort)),
       UrlParameter("uploaded", std::to_string(this->uploaded)),
       UrlParameter("downloaded", std::to_string(this->downloaded)),
       UrlParameter("left", std::to_string(this->left)),
       UrlParameter("compact", this->compact ? "1" : "0"),
       UrlParameter("event", TrackerEvent::Started)});

  if (!peers.size()) {
    std::cerr << "Failed announcing the torrent tracker\n";
    return false;
  }

  this->peers = peers;

  return true;
}
