#pragma once
#include <cstdint>
#include <string>
#include <vector>

class TcpStream {
private:
  int socketFd = -1;

public:
  TcpStream() = default;
  ~TcpStream();

  bool isOpen;

  int connect(const std::string &host, uint16_t port);
  size_t writeAll(const std::string &data);
  size_t writeAllBytes(const std::vector<std::byte> &data);
  std::string readAll();
  std::string readExact(const size_t nBytes);
  std::vector<std::byte> readExactBytes(const size_t nBytes);
  bool close();
};
