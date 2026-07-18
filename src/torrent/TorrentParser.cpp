#include "TorrentParser.hpp"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

TorrentParser::TorrentParser() {
  this->bencode = Bencode();
  this->torrentPath = std::filesystem::path();
  this->torrentContent = "";
}

TorrentParser::TorrentParser(std::string inputTorrent) {
  this->bencode = Bencode();
  this->torrentPath = std::filesystem::path(inputTorrent);
  this->torrentContent = "";
}

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

  this->torrentContent = buffer.str();

  return 0;
}

int TorrentParser::load(std::string inputTorrent) {
  this->torrentPath = std::filesystem::path(inputTorrent);
  return this->load();
}

std::string TorrentParser::getTracker() {
  if (this->torrentContent == "") {
    std::cerr << "No torrent loaded\n";
    return "";
  }

  Bencode parser = this->bencode;
  parser = parser.decodeNext(this->torrentContent);

  const auto &dict = std::get<Bencode::Dict>(parser.value());
  return std::get<std::string>(dict.at("announce").value());
}
