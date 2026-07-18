#include <string>

class TcpStream {
private:
  int socketFd = -1;

public:
  TcpStream() = default;

  int connect(const std::string &host, const std::string &port);
  void writeAll(const std::string &data);
  std::string readAll();
};
