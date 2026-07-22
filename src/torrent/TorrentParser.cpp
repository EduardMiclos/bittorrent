#include "Bencode.hpp"
#include "Utils.hpp"
#include "torrent/TorrentParser.hpp"
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

TorrentParser::TorrentParser() { this->torrentPath = std::filesystem::path(); };

TorrentParser::TorrentParser(std::string inputTorrent) {
  this->torrentPath = std::filesystem::path(inputTorrent);
};

int TorrentParser::load() {
  if (this->torrentPath.extension() != ".torrent") {
    std::cerr << "Incorrect file type. Must be a torrent file [.torrent]\n";
    return -1;
  }

  if (!std::filesystem::exists(this->torrentPath)) {
    std::cerr << "Unable to find the torrent file\n";
    return -2;
  }

  if (!std::filesystem::file_size(this->torrentPath)) {
    std::cerr << "The torrent file is empty\n";
    return -3;
  }

  std::ifstream torrentFile(this->torrentPath);
  std::stringstream buffer;
  buffer << torrentFile.rdbuf();

  this->torrentContent = bencode::decode(buffer.str());
  return 0;
};

int TorrentParser::load(std::string inputTorrent) {
  this->torrentPath = std::filesystem::path(inputTorrent);
  return this->load();
};

std::string TorrentParser::getTrackerURL() {
  if (auto *dict = std::get_if<bencode::dict>(&this->torrentContent.base())) {
    auto elem = this->torrentContent["announce"];
    return std::get<bencode::string>(elem);
  }

  return "";
};

std::string TorrentParser::getInfoHash() {
  if (auto *dict = std::get_if<bencode::dict>(&this->torrentContent.base())) {
    auto info = this->torrentContent["info"];

    std::string encodedInfo = bencode::encode(info);
    return sha1(encodedInfo);
  }

  return "";
}

long long TorrentParser::getFilesLength() {
  long long totalLength = 0;

  auto info = this->torrentContent["info"];

  if (auto *infoDict = std::get_if<bencode::dict>(&info.base())) {
    if (infoDict->find("length") != infoDict->end()) {
      // single file torrent
      totalLength = std::get<bencode::integer>(info["length"]);
    } else {
      // multi-files torrent
      // accumulate lengths from all files
      bencode::list files = std::get<bencode::list>(info["files"]);
      for (const auto &f : files)
        totalLength += std::get<bencode::integer>(f.at("length"));
    }
  }

  return totalLength;
}
