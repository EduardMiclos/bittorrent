#include "Utils.hpp"
#include "networking/HttpClient.hpp"
#include "torrent/TorrentClient.hpp"
#include "torrent/Tracker.hpp"

TorrentClient::TorrentClient() {
  this->parser = TorrentParser();
  this->uid = generateRandomString(20);
  this->uploaded = 0;
  this->downloaded = 0;
  this->left = 0;

  // set this to true to accept compact responses from trackers;
  // trackers do this in order to save bandwith, I guess it's better to support
  // it than not to, considering that there are trackers that enforce compact
  // responses; best to always be set to true
  this->compact = true;
  this->listeningPort = torrentPorts[0];
}

bool TorrentClient::interogateTracker(std::string_view trackerEvent) {
  HttpClient httpClient{};

  std::cout << this->tracker.url.protocol << "://" << this->tracker.url.host
            << ":" << this->tracker.url.port << "/"
            << this->tracker.url.resource << '\n';

  HttpResponse response = httpClient.get(
      this->tracker.url,
      {UrlParameter("info_hash", this->infoHash),
       UrlParameter("peer_id", this->uid),
       UrlParameter("port", std::to_string(this->listeningPort)),
       UrlParameter("uploaded", std::to_string(this->uploaded)),
       UrlParameter("downloaded", std::to_string(this->downloaded)),
       UrlParameter("left", std::to_string(this->left)),
       UrlParameter("compact", this->compact ? "1" : "0"),
       UrlParameter("event", trackerEvent)});

  if (response.statusCode != 200) {
    return false;
  }

  std::cout << response.body << '\n';
  return true;
}

bool TorrentClient::setTorrent(std::string torrentPath) {
  this->parser.load(torrentPath);

  this->tracker = Tracker(this->parser.getTrackerURL());
  this->infoHash = this->parser.getInfoHash();

  bool interogateSuccess = this->interogateTracker(TrackerEvent::Started);
  if (!interogateSuccess) {
    std::cerr << "Failed interogating the torrent tracker\n";
    return false;
  }

  return true;
}
