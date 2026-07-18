#pragma once
#include <string>

class Url {
public:
  Url() = default;
  Url(std::string);

  std::string protocol;
  std::string host;
  int port;
  std::string resource;
};
