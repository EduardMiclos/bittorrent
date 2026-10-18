#include "torrent/Piece.hpp"

Piece::Piece(std::size_t length, std::string hash, int index) {
  this->length = length;
  this->hash = hash;
  this->index = index;
}

std::pair<int, int> Piece::getNextChunk() { return remainingChunks[0]; }
void Piece::markDownloadedChunk(int startPieceOffset, int endPieceOffset) {
  if (startPieceOffset > endPieceOffset)
    return;

  bool downloadedChunkInserted = false;
  for (auto it = this->downloadedChunks.begin();
       it != this->downloadedChunks.end(); ++it) {
    if (startPieceOffset < it->first) {
      this->downloadedChunks.insert(
          it, std::make_pair(startPieceOffset, endPieceOffset));
      downloadedChunkInserted = true;
      break;
    }
  }

  if (!downloadedChunkInserted)
    this->downloadedChunks.push_back(
        std::make_pair(startPieceOffset, endPieceOffset));

  for (auto it = this->remainingChunks.begin();
       it != this->remainingChunks.end(); ++it) {
    if (startPieceOffset >= it->first) {
      if (it->first == startPieceOffset && it->second == endPieceOffset) {
        this->remainingChunks.erase(it);
        break;
      }

      if (it->first != startPieceOffset)
        this->remainingChunks.insert(
            it, std::make_pair(it->first, startPieceOffset - 1));

      if (it->second != endPieceOffset)
        this->remainingChunks.insert(
            it, std::make_pair(endPieceOffset + 1, it->second));

      this->remainingChunks.erase(it);
      break;
    }
  }
}
