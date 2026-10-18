// #include "torrent/Peer.hpp"
// #include "torrent/PeerConnection.hpp"
#include "torrent/TorrentClient.hpp"
#include <fstream>
#include <iostream>

/*
void peerCommunication() {
  TorrentClient torrentClient;
  TorrentParser parser;
  std::vector<std::string> allPieces;

  parser.load("input/mock/test2.torrent");

  std::cout << "Info hash: " << parser.getInfoHash() << '\n';
  std::cout << "Piece count: " << parser.getPieceCount() << '\n';
  std::cout << "Piece size[0]: " << parser.getPieceSizeAt(0) << '\n';
  std::cout << "Piece Hash[0]: " << parser.getPieceHashAt(0) << '\n';

  PeerConnection peerConnection = PeerConnection(Peer("127.0.0.1", 51413));

  bool isOk = peerConnection.handshake(torrentClient.uid, parser.getInfoHash());
  std::cout << (isOk == true ? "Successful handshake!\n"
                             : "Handshake failed!\n");

  if (isOk) {
    bool messageSent = peerConnection.sendStatus(PeerStatus::Interested);

    if (messageSent) {
      std::cout << "message sent successfully!\n";
    }

    std::string response = peerConnection.receiveMessage();
    std::vector<unsigned char> bitfield(response.begin(), response.end());

    if (bitfield.at(0) == 5) {
      std::cout << "bitfield received!\n";

      bool first_piece_available = (bitfield.at(1) & 0x80) != 0;
      std::cout << "\nFirst piece is available!\n";

      // request piece

      // constructing request message
      // 00 00 00 00 // piece index
      // 00 00 00 00 // piece offset
      // 00 00 00 1a [00011010] // piece size in bytes

      std::string requestMessage;
      requestMessage.reserve(12);

      uint32_t pieceIndex = 0;
      uint32_t pieceOffset = 0;
      uint32_t pieceSize = 46;

      requestMessage.push_back(static_cast<char>((pieceIndex >> 24)));

      requestMessage.push_back(static_cast<char>((pieceIndex >> 16)));

      requestMessage.push_back(static_cast<char>((pieceIndex >> 8)));

      requestMessage.push_back(static_cast<char>((pieceIndex)));

      requestMessage.push_back(static_cast<char>((pieceOffset >> 24)));

      requestMessage.push_back(static_cast<char>((pieceOffset >> 16)));

      requestMessage.push_back(static_cast<char>((pieceOffset >> 8)));

      requestMessage.push_back(static_cast<char>((pieceOffset)));

      requestMessage.push_back(static_cast<char>((pieceSize >> 24)));

      requestMessage.push_back(static_cast<char>((pieceSize >> 16)));

      requestMessage.push_back(static_cast<char>((pieceSize >> 8)));

      requestMessage.push_back(static_cast<char>((pieceSize)));

      bool pieceRequest =
          peerConnection.sendMessage(PeerStatus::Request, requestMessage);

      std::string blockResponse = peerConnection.receiveMessage();

      sleep(1);

      pieceRequest =
          peerConnection.sendMessage(PeerStatus::Request, requestMessage);

      sleep(1);

      std::string piece = peerConnection.receiveMessage();

      if (piece.empty()) {
        std::cerr << "received piece is fucking empty";
        return;
      }

      uint32_t receivedPieceIndex =
          (static_cast<uint32_t>(static_cast<unsigned char>(piece[1])) << 24) |
          (static_cast<uint32_t>(static_cast<unsigned char>(piece[2])) << 16) |
          (static_cast<uint32_t>(static_cast<unsigned char>(piece[3])) << 8) |
          static_cast<uint32_t>(static_cast<unsigned char>(piece[4]));

      uint32_t receivedPieceOffset =
          (static_cast<uint32_t>(static_cast<unsigned char>(piece[5])) << 24) |
          (static_cast<uint32_t>(static_cast<unsigned char>(piece[6])) << 16) |
          (static_cast<uint32_t>(static_cast<unsigned char>(piece[7])) << 8) |
          static_cast<uint32_t>(static_cast<unsigned char>(piece[8]));

      std::cout << "index: " << receivedPieceIndex << '\n';
      std::cout << "offset: " << receivedPieceOffset << '\n';

      allPieces.push_back(piece.substr(9));
      std::cout << "\npiece data: " << allPieces[receivedPieceIndex] << '\n';
      std::cout << "FULL STOP\n";

      std::cout << "FULL PIECE: " << piece << '\n';

      std::ofstream torrentOutput;
      torrentOutput.open("downloaded_file.txt");
      torrentOutput << allPieces[receivedPieceIndex];
      torrentOutput.close();

      peerConnection.close();

      std::cout << "Torrent downloaded successfully!\n";
    }

    std::cout << (bitfield.at(0) == 5) << '\n';
  }
}
*/

int main() {
  // peerCommunication();

  // torrentClient.setTorrent("input/alicesadventures19033gut.torrent");
  // torrentClient.setTorrent("/tmp/test.torrent");

  return 0;
}
