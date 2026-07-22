#include "Peer.hpp"
#include "TorrentParser.hpp"
#include "Tracker.hpp"

#include <string>

namespace TrackerEvent {

// send if it's the first request to the tracker
inline constexpr std::string_view Started = "started";

// send to the tracker if the client shuts down gracefully
inline constexpr std::string_view Stopped = "stopped";

// send to the tracker when the download completes
inline constexpr std::string_view Completed = "completed";

} // namespace TrackerEvent

constexpr std::array<std::uint16_t, 5> torrentPorts{6881, 6882, 6883, 6884,
                                                    6885};

class TorrentClient {
private:
  TorrentParser parser;

  Tracker tracker;
  std::string infoHash;

  std::string uid;
  int uploaded;
  int downloaded;
  long long left;
  bool compact;

  int listeningPort;
  std::vector<Peer> peers;

private:
  bool interogateTracker(std::string_view trackerEvent);

public:
  TorrentClient();
  bool setTorrent(std::string torrentPath);
  void startDownload();
};
