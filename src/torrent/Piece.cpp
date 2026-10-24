#include "torrent/Piece.hpp"
#include <iostream>
#include <math.h>

Piece::Piece(std::size_t length, std::string hash, int index) {
  this->length = length;
  this->hash = hash;
  this->index = index;

  const size_t CHUNK_SIZE = 16 * 1024; // bytes
  this->chunks.reserve(std::ceil(length / CHUNK_SIZE));

  this->lastChunkSize = length - CHUNK_SIZE * (this->chunks.capacity() - 1);
}

bool Piece::isChunkValid(size_t chunkIndex) { return this->chunks[chunkIndex] == ChunkState::Missing; }
size_t Piece::getNextChunk() {
  for (size_t i = 0; i < this->chunks.size(); i++)
    if (this->chunks[i] == ChunkState::Missing)
      return i;

  return -1;
}
bool Piece::setChunk(size_t chunkIndex, ChunkState state) {
  switch (state) {
  case ChunkState::Downloaded:
    if (state == ChunkState::Downloaded) {
      std::cout << "[WARNING] This chunk is already marked as downloaded.\n";
      return false;
    }
    if (state == ChunkState::Missing) {
      std::cout << "[WARNING] Cannot mark chunk as downloaded. Missing intermediary 'Requested' state!\n";
      return false;
    }
    break;
  case ChunkState::Requested:
    if (state == ChunkState::Requested) {
      std::cout << "[WARNING] This chunk is already marked for downloading.\n";
      return false;
    }
    if (state == ChunkState::Downloaded) {
      std::cout << "[WARNING] Cannot mark this chunk as requested. It's already downloaded!\n";
      return false;
    }
    break;
  default:
    break;
  }

  this->chunks[chunkIndex] = state;
  return true;
}
