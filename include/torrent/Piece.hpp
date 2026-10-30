#pragma once

#include <cstddef>
#include <string>
#include <vector>

enum class ChunkState { Missing, Requested, Downloaded };
const size_t CHUNK_SIZE = 16 * 1024; // bytes

class Piece {
public:
  // bytes
  std::size_t length;
  std::string hash;
  int index;

  std::vector<ChunkState> chunks;
  std::size_t lastChunkSize;

public:
  Piece(std::size_t length, std::string hash, int index);
  size_t getNextChunk();
  size_t getChunkSizeAt(size_t chunkIndex);
  bool isChunkValid(size_t chunkIndex);
  bool setChunk(size_t chunkIndex, ChunkState state);
  std::vector<std::byte> serializeRequest(size_t chunkIndex);
};
