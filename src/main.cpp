#include "torrent/TorrentClient.hpp"

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "networking/TcpStream.hpp"

int main() {
  // TorrentClient torrentClient;
  // torrentClient.setTorrent("input/romanian_books.torrent");

  TcpStream tcpStream;
  tcpStream.connect("google.com", "80");
  tcpStream.writeAll("GET / HTTP/1.1\r\nConnection: close\r\n\r\n");
  std::string response = tcpStream.readAll();

  std::cout << response;

  // TorrentParser torrentParser("input/romanian_books.torrent");
  // torrentParser.load();

  // std::string trackerURL = torrentParser.getTracker();
  // std::cout << URLParser::getHost(trackerURL) << '\n';
  // std::cout << URLParser::getProtocol(trackerURL) << '\n';
  // std::cout << URLParser::getResource(trackerURL) << '\n';

  // struct addrinfo addressHints {
  // }, *targetAddress;
  // addressHints.ai_family = AF_UNSPEC;
  // addressHints.ai_socktype = SOCK_STREAM;
  // getaddrinfo(URLParser::getHost(trackerURL).c_str(),
  //             URLParser::getProtocol(trackerURL).c_str(),
  //             &addressHints, &targetAddress);

  // int sockfd = socket(targetAddress->ai_family,
  // targetAddress->ai_socktype,
  //                     targetAddress->ai_protocol);
  // if (sockfd < 0) {
  //   std::cerr << "Unable to create socket";
  //   return -2;
  // }

  // int conn = connect(sockfd, targetAddress->ai_addr,
  // targetAddress->ai_addrlen); if (conn < 0) {
  //   std::cerr << "Unable to connect to address\n";
  //   return -3;
  // }

  // std::string request = "GET /" + URLParser::getResource(trackerURL) +
  //                       " HTTP/1.1\r\n" +
  //                       "Host: " + URLParser::getHost(trackerURL) +
  //                       "\r\nConnection: close\r\n\r\n";

  // send(sockfd, request.c_str(), request.length(), 0);
  // std::cout << "request sent";

  // char inbuff[3222];
  // if (recv(sockfd, &inbuff, 3222, 0) == -1) {
  //   std::cerr << "error reading socket content\n";
  //   return -3;
  // }

  // std::cout << inbuff;

  return 0;
}
