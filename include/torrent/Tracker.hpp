#pragma once
#include "networking/HttpClient.hpp"
#include "networking/Url.hpp"
#include "torrent/Peer.hpp"
#include <vector>

class Tracker {
public:
  Url url;
  Tracker() = default;
  Tracker(std::string);

  std::vector<Peer> announce(std::vector<UrlParameter>);
};
