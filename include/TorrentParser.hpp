#include "Bencode.hpp"
#include <filesystem>

class TorrentParser {
private:
  std::filesystem::path torrentPath;
  std::string torrentContent;
  Bencode bencode;

public:
  TorrentParser();
  TorrentParser(std::string);

  int load();
  int load(std::string);
  std::string getTracker();
};
