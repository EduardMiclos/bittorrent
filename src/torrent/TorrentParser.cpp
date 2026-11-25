#include "Bencode.hpp"
#include "Config.hpp"
#include "Utils.hpp"
#include "torrent/TorrentParser.hpp"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

TorrentParser::TorrentParser() { this->torrentPath = std::filesystem::path(); };

TorrentParser::TorrentParser(std::string inputTorrent) { this->torrentPath = std::filesystem::path(inputTorrent); };

int TorrentParser::load() {
  std::cout << "Loading torrent file...\n";
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

  std::cout << "Torrent file loaded.\n";
  this->torrentContent = bencode::decode(buffer.str());
  std::cout << "Torrent file decoded.\n";

  this->totalSize = this->_getTotalSize();
  this->pieceSize = this->_getPieceSize();
  this->pieceHashes = this->_getPieceHashes();
  this->pieceCount = this->_getPieceCount();

  return 0;
};

int TorrentParser::load(std::string inputTorrent) {
  this->torrentPath = std::filesystem::path(inputTorrent);
  return this->load();
};

template <typename T> std::optional<T> TorrentParser::findInDict(const bencode::data &data, const std::string &key) {
  if (auto *dict = std::get_if<bencode::dict>(&data.base())) {
    auto it = dict->find(key);

    if (it != dict->end()) {
      if (auto *value = std::get_if<T>(&it->second.base()))
        return *value;
    }
  }

  return std::nullopt;
}

std::string TorrentParser::getTrackerURL() {
  auto tracker = this->findInDict<std::string>(this->torrentContent, "announce");

  return tracker.value_or("");
};

std::string TorrentParser::getInfoHash() {
  if (auto *dict = std::get_if<bencode::dict>(&this->torrentContent.base())) {
    auto info = this->torrentContent["info"];

    std::string encodedInfo = bencode::encode(info);
    return sha1(encodedInfo);
  }

  return "";
}

std::size_t TorrentParser::getPieceSizeAt(int pieceIndex) {
  if (pieceIndex < 0)
    return 0;

  if (pieceIndex < this->pieceCount - 1)
    return this->pieceSize;

  if (pieceIndex == this->pieceCount - 1)
    return this->totalSize - (this->pieceCount - 1) * this->pieceSize;

  return 0;
}

std::string TorrentParser::getPieceHashAt(int pieceIndex) {
  if (pieceIndex < 0)
    return "";

  if (pieceIndex >= this->pieceCount)
    return "";

  return this->pieceHashes.substr(pieceIndex * PIECE_HASH_SIZE, PIECE_HASH_SIZE);
}

int TorrentParser::getPieceCount() { return this->pieceCount; }

std::size_t TorrentParser::getTotalSize() { return this->totalSize; }

std::size_t TorrentParser::_getTotalSize() {
  long long totalLength = 0;

  auto info = this->torrentContent["info"];

  if (auto *infoDict = std::get_if<bencode::dict>(&info.base())) {
    if (infoDict->find("length") != infoDict->end()) {
      // single file torrent
      totalLength = std::get<bencode::integer>(info["length"]);
    } else {
      // multi-file torrent
      // accumulate lengths from all files
      bencode::list files = std::get<bencode::list>(info["files"]);
      for (const auto &f : files)
        totalLength += std::get<bencode::integer>(f.at("length"));
    }
  }

  return totalLength;
}

std::size_t TorrentParser::_getPieceSize() {
  std::size_t pieceLength = 0;
  auto info = this->torrentContent["info"];

  if (auto *infoDict = std::get_if<bencode::dict>(&info.base())) {
    if (infoDict->find("piece length") != infoDict->end())
      pieceLength = std::get<bencode::integer>(info["piece length"]);
  }

  return pieceLength;
}

int TorrentParser::_getPieceCount() { return this->pieceHashes.size() / PIECE_HASH_SIZE; }

std::string TorrentParser::_getPieceHashes() {
  std::string pieceHashes{};
  auto info = this->torrentContent["info"];

  if (auto *infoDict = std::get_if<bencode::dict>(&info.base())) {
    if (infoDict->find("pieces") != infoDict->end())
      pieceHashes = std::get<bencode::string>(info["pieces"]);
  }

  return pieceHashes;
}
