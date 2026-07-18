#include "TorrentParser.hpp"
#include "UrlParser.hpp"

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int main() {
  TorrentParser torrentParser("input/ubuntu-22.04.5-desktop-amd64.iso.torrent");
  torrentParser.load();

  std::string trackerURL = torrentParser.getTracker();
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket < 0) {
    std::cerr << "Faild to create the socket\n";
    return -1;
  }

  struct addrinfo addressHints, *targetAddress;
  memset(&addressHints, sizeof(addressHints), 0);
  addressHints.ai_family = AF_INET;
  addressHints.ai_socktype = SOCK_STREAM;
  addressHints.ai_flags = AI_PASSIVE;
  getaddrinfo(URLParser::getHost(trackerURL).c_str(),
              URLParser::getProtocol(trackerURL).c_str(), &addressHints,
              &targetAddress);

  int sockfd = socket(targetAddress->ai_family, targetAddress->ai_socktype,
                      targetAddress->ai_protocol);
  if (sockfd < 0) {
    std::cerr << "Unable to create socket";
    return -2;
  }

  int conn = connect(sockfd, targetAddress->ai_addr, targetAddress->ai_addrlen);
  if (conn < 0) {
    std::cerr << "Unable to connect to address\n";
    return -3;
  }

  // struct addrinfo hints;
  // struct addrinfo *res;

  // memset(&hints, 0, sizeof(hints));
  // hints.ai_family = AF_INET;
  // hints.ai_socktype = SOCK_STREAM;
  // hints.ai_flags = AI_PASSIVE;

  // std::cout << "ready to connect\n";
  // getaddrinfo(trackerURL.c_str(), "https", &hints, &res);

  // std::cout << "got addr info\n";
  // int sockfd = socket(res->ai_family, res->ai_socktype,
  // res->ai_protocol); std::cout << "ready to connect\n";

  // if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
  //   std::cerr << "error connecting to domain\n";
  //   return -2;
  // }

  // std::cout << "connection succeded!\n";

  // const char request[] = "GET /announce HTTP/1.1\r\n"
  //                        "Host: torrent.ubuntu.com\r\n"
  //                        "Connection: close\r\n"
  //                        "\r\n";

  // std::size_t totalSent = 0;
  // const std::size_t requestLength = std::strlen(request);
  // send(sockfd, request, requestLength, 0);

  // char inbuff[3222];
  // if (recv(sockfd, &inbuff, 3222, 0) == -1) {
  //   std::cerr << "error reading socket content\n";
  //   return -3;
  // }

  // std::cout << inbuff;

  return 0;
}
