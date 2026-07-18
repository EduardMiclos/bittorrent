#include "UrlParser.hpp"

std::string URLParser::getProtocol(std::string url) {
  return url.substr(0, url.find(":"));
}

std::string URLParser::getHost(std::string url) {
  size_t hostStartPos = url.find(":") + 3;
  std::string urlHostStart = url.substr(hostStartPos);
  return urlHostStart.substr(0, urlHostStart.find("/"));
}

std::string URLParser::getResource(std::string url) {
  size_t hostStartPos = url.find(":") + 3;
  std::string urlHostStart = url.substr(hostStartPos);
  return urlHostStart.substr(urlHostStart.find("/") + 1);
}
