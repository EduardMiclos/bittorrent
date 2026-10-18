#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

class Peer;

class Piece {
public:
  // bytes
  std::size_t length;
  std::string hash;
  int index;

  // [(byte_0, byte_n), (byte_k, byte_l)]
  std::vector<std::pair<int, int>> downloadedChunks;
  std::vector<std::pair<int, int>> remainingChunks{
      std::make_pair(0, this->length - 1)};

public:
  Piece(std::size_t length, std::string hash, int index);
  std::pair<int, int> getNextChunk();
  void markDownloadedChunk(int startPieceOffset, int endPieceOffset);
};
