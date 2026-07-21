#include "networking/Url.hpp"
#include <stdexcept>

Url::Url(std::string url) {
  const size_t protocolEnd = url.find("://");
  if (protocolEnd == std::string::npos) {
    throw std::invalid_argument("URL has no protocol");
  }

  this->protocol = url.substr(0, protocolEnd);

  const size_t hostStart = protocolEnd + 3;
  std::string hostSubstring = url.substr(hostStart);

  const size_t portStart = hostSubstring.find(":");
  const size_t resourceStart = hostSubstring.find("/");

  if (portStart == std::string::npos) {
    if (this->protocol == "https")
      this->port = 443;
    else
      this->port = 80;
    this->host = hostSubstring.substr(0, resourceStart);
  } else {
    std::string portSubstring = hostSubstring.substr(portStart + 1);

    this->port = stoi(portSubstring.substr(0, resourceStart));
    this->host = hostSubstring.substr(0, portStart);
  }

  this->resource = hostSubstring.substr(resourceStart);
}
