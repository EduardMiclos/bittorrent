#pragma once
#include <string>

class TcpStream {
private:
  int socketFd = -1;

public:
  TcpStream() = default;
  ~TcpStream();

  int connect(const std::string &host, const short &port);
  size_t writeAll(const std::string data);
  std::string readAll();
  std::string readExact(const size_t nBytes);
  void close();
};
