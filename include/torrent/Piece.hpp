#pragma once

#include <string>

class Piece {
public:
  std::size_t length;
  std::string hash;
  int index;

public:
  Piece(std::size_t length, std::string hash, int index);
};
