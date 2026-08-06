#pragma once

#include "Bencode.hpp"
#include <filesystem>

class TorrentParser {
private:
  std::filesystem::path torrentPath;
  bencode::data torrentContent;

  std::size_t totalSize = 0;
  std::size_t pieceSize = 0;
  std::string pieceHashes;
  int pieceCount = 0;

private:
  std::size_t _getTotalSize();
  std::size_t _getPieceSize();
  std::string _getPieceHashes();
  int _getPieceCount();

public:
  TorrentParser();
  TorrentParser(std::string);

  int load();
  int load(std::string);
  std::string getTrackerURL();
  std::string getInfoHash();

  std::size_t getPieceSizeAt(int pieceIndex);
  std::string getPieceHashAt(int pieceIndex);
  int getPieceCount();
  std::size_t getTotalSize();
};
