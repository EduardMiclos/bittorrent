#include <string>

class URLParser {
public:
  static std::string getProtocol(std::string);
  static std::string getHost(std::string);
  static std::string getPort(std::string);
  static std::string getResource(std::string);
};
