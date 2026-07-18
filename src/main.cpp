#include "TorrentParser.hpp"
#include <iostream>

int main() {
  TorrentParser torrentParser("input/ubuntu-22.04.5-desktop-amd64.iso.torrent");
  torrentParser.load();
  std::cout << torrentParser.getTracker();
  return 0;
}
