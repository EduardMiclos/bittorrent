#include "Bencode.hpp"
#include "networking/Url.hpp"
#include "torrent/Tracker.hpp"

Tracker::Tracker(std::string trackerUrl) { this->url = Url(trackerUrl); }

std::vector<Peer> Tracker::announce(std::vector<UrlParameter> urlParameters) {
  HttpClient httpClient{};
  HttpResponse response = httpClient.get(this->url, urlParameters);

  if (response.statusCode != 200)
    return {};

  bencode::data decodedResponse = bencode::decode(response.body);
  auto *root = std::get_if<bencode::dict>(&decodedResponse.base());
  if (!root || root->find("peers") == root->end())
    return {};

  const bencode::string &peers =
      std::get<bencode::string>(decodedResponse.at("peers"));

  std::vector<Peer> returnedPeers{};
  for (std::size_t i = 0; i < peers.size(); i += 6)
    returnedPeers.push_back(Peer(std::string_view(peers).substr(i, 6)));

  return returnedPeers;
}
