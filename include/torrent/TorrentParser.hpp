#include "Bencode.hpp"
#include <filesystem>

class TorrentParser {
private:
  std::filesystem::path torrentPath;
  bencode::data torrentContent;

public:
  TorrentParser();
  TorrentParser(std::string);

  int load();
  int load(std::string);
  std::string getTrackerURL();
  std::string getInfoHash();
  long long getFilesLength();
};
