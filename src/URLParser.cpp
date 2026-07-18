#include "UrlParser.hpp"
#include <variant>

std::string URLParser::getProtocol(std::string url) {
  return url.substr(0, url.find(":"));
}

std::string URLParser::getHost(std::string url) {
  size_t hostStartPos = url.find(":") + 3;
  std::string urlHostStart = url.substr(hostStartPos);

  size_t portPosition = urlHostStart.find(":");
  if (portPosition == std::variant_npos)
    return urlHostStart.substr(0, urlHostStart.find("/"));

  return urlHostStart.substr(0, portPosition);
}

std::string URLParser::getPort(std::string url) {
  size_t hostStartPos = url.find(":") + 3;
  std::string urlHostStart = url.substr(hostStartPos);

  size_t portPosition = urlHostStart.find(":");
  if (portPosition == std::variant_npos)
    return "";

  std::string urlPortStart = urlHostStart.substr(portPosition + 1);
  return urlPortStart.substr(0, urlPortStart.find("/"));
}

std::string URLParser::getResource(std::string url) {
  size_t hostStartPos = url.find(":") + 3;
  std::string urlHostStart = url.substr(hostStartPos);
  return urlHostStart.substr(urlHostStart.find("/") + 1);
}
