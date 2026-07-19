#include "networking/TcpStream.hpp"
#include <cstdlib>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

int TcpStream::connect(const std::string &host, const short &port) {
  struct addrinfo addressHints {};
  struct addrinfo *targetAddress;

  // accept both IPv4 and IPv6
  addressHints.ai_family = AF_UNSPEC;

  // TCP stream
  addressHints.ai_socktype = SOCK_STREAM;

  // the port defines the protocol, so the second argument can be either the
  // service or the port [e.g.: 80 means http, 443 means https]
  short returnCode =
      getaddrinfo(host.c_str(), std::to_string(port).c_str(), &addressHints,
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
    std::cerr << "Error connecting to the address\n";
    return -205;
  }

  this->socketFd = socketFd;
  return 0;
};

size_t TcpStream::writeAll(const std::string data) {
  if (this->socketFd < 0) {
    std::cerr << "Cannot write to non-existent socket\n";
    return 0;
  }

  size_t totalSent = 0;

  while (totalSent < data.size()) {

    // making use of string pointers & index value to send next chunk of data
    ssize_t bytesSent = send(this->socketFd, data.data() + totalSent,
                             data.size() - totalSent, 0);
    if (bytesSent < 0) {
      std::cerr << "Error sending bytes\n";
      break;
    }

    // this might also occur if internet goes down, will probably need
    // to treat it differently in the future but it's fine for now
    // [I trust my ISP]
    if (bytesSent == 0) {
      std::cerr << "Socket closed during data transmission\n";
      break;
    }

    totalSent += bytesSent;
  }

  return totalSent;
};

std::string TcpStream::readAll() {
  if (this->socketFd < 0) {
    std::cerr << "Cannot read from non-existent socket\n";
    return "";
  }

  std::string returnString;
  char buff[4096];

  while (true) {
    ssize_t bytesRead = recv(this->socketFd, buff, 1024, 0);
    if (bytesRead < 0) {
      std::cerr << "Error reading bytes\n";
      break;
    }

    if (bytesRead == 0)
      break;

    returnString.append(buff, static_cast<std::size_t>(bytesRead));
  }

  return returnString;
}

void TcpStream::close() {
  if (this->socketFd < 0) {
    return;
  }
  ::close(this->socketFd);
}

// this will prove to be useful when talking to peers
std::string TcpStream::readExact(const size_t nBytes) {
  if (this->socketFd < 0) {
    std::cerr << "Cannot read from non-existent socket\n";
    return "";
  }

  std::string returnString;
  char buff[4096];

  while (true) {
    ssize_t bytesRead = recv(this->socketFd, buff, nBytes, 0);
    if (bytesRead < 0) {
      std::cerr << "Error reading bytes\n";
      break;
    }

    if (bytesRead == 0)
      break;

    returnString.append(buff, static_cast<std::size_t>(bytesRead));

    if (returnString.size() >= nBytes)
      break;
  }

  return returnString;
}

TcpStream::~TcpStream() { this->close(); }
