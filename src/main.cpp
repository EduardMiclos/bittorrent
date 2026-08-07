#include "torrent/Peer.hpp"
#include "torrent/PeerConnection.hpp"
#include "torrent/TorrentClient.hpp"

int main() {
  TorrentClient torrentClient;
  TorrentParser parser;
  parser.load("/tmp/test.torrent");

  // torrentClient.setTorrent("input/alicesadventures19033gut.torrent");
  // torrentClient.setTorrent("/tmp/test.torrent");

  PeerConnection peerConnection = PeerConnection(Peer("127.0.0.1", 51413));
  bool isOk = peerConnection.handshake(torrentClient.uid, parser.getInfoHash());
  std::cout << (isOk == true ? "ok\n" : "not ok\n");

  return 0;
}
