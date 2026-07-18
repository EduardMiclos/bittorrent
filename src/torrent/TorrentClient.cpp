#include "torrent/TorrentClient.hpp"
#include "torrent/Tracker.hpp"

TorrentClient::TorrentClient() { this->parser = TorrentParser(); }

bool TorrentClient::setTorrent(std::string torrentPath) {
  this->parser.load(torrentPath);
  this->tracker = Tracker(this->parser.getTrackerURL());
  return true;
}
