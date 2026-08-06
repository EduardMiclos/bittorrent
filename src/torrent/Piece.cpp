#include "torrent/Piece.hpp"

Piece::Piece(std::size_t length, std::string hash, int index) {
  this->length = length;
  this->hash = hash;
  this->index = index;
}
