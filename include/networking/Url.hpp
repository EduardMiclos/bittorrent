#pragma once
#include <cstdint>
#include <string>

class Url {
public:
  Url() = default;
  Url(std::string);

  std::string protocol;
  std::string host;
  uint16_t port;
  std::string resource;
};
