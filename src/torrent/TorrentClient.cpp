#include "torrent/TorrentClient.hpp"
#include "torrent/Tracker.hpp"

TorrentClient::TorrentClient() { this->parser = TorrentParser(); }

bool TorrentClient::setTorrent(std::string torrentPath) {
  this->parser.load(torrentPath);

  Tracker tracker = Tracker(this->parser.getTracker());
  this->trackers.push_back(tracker);
  return true;
}
