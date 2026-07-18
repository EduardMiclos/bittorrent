#include <string>

class TcpStream {
private:
  int socketFd = -1;

public:
  TcpStream() = default;
  ~TcpStream();

  int connect(const std::string &host, const std::string &port);
  size_t writeAll(const std::string data);
  std::string readAll();
  void close();
};
