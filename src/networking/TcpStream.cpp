#include "networking/TcpStream.hpp"
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>

int TcpStream::connect(const std::string &host, const std::string &port) {
  struct addrinfo addressHints {};
  struct addrinfo *targetAddress;

  addressHints.ai_family = AF_UNSPEC;
  addressHints.ai_socktype = SOCK_STREAM;

  short returnCode = getaddrinfo(host.c_str(), port.c_str(), &addressHints,
                                 (struct addrinfo **)&targetAddress);
  if (returnCode != 0) {
    std::cerr << "Error fetching address information for: " << host << ":"
              << port << '\n';
    return returnCode;
  }

  int socketFd = socket(targetAddress->ai_family, targetAddress->ai_socktype,
                        targetAddress->ai_protocol);
  if (socketFd < 0) {
    std::cerr << "Error creating the socket\n";
    return -200;
  }

  short conn = ::connect(socketFd, (struct sockaddr *)targetAddress->ai_addr,
                         targetAddress->ai_addrlen);

  if (conn < 0) {
    std::cout << "Error connecting to the address\n";
    return -205;
  }

  this->socketFd = socketFd;
  return 0;
};
