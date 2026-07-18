#include "Peer.hpp"
#include "TorrentParser.hpp"
#include "Tracker.hpp"

#include <string>

class TorrentClient {
public:
  TorrentParser parser;

  Tracker tracker;
  std::vector<Peer> peers;

public:
  TorrentClient();
  bool setTorrent(std::string);
  bool startDownload();
};
