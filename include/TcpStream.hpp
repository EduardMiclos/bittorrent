#include <iostream>
#include <span>

class TcpStream {

public:
  static TcpStream connect(std::string host, std::string service);

  std::size_t readBytes(std::span<std::byte> buffer);
  std::size_t writeBytes(std::span<std::byte> buffer);
}
