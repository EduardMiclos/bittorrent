#pragma once

#include "networking/TcpStream.hpp"
#include "networking/Url.hpp"

#include <string>
#include <utility>
#include <vector>

namespace HttpMethods {
inline constexpr std::string Get = "GET";
inline constexpr std::string Post = "POST";
inline constexpr std::string Put = "PUT";
inline constexpr std::string Delete = "DELETE";
} // namespace HttpMethods

namespace ConnectionType {
inline constexpr std::string_view Close = "close";
inline constexpr std::string_view KeepAlive = "keep-alive";
} // namespace ConnectionType

using UrlParameter = std::pair<std::string, std::string>;
using HttpHeader = std::pair<std::string, std::string>;

struct HttpResponse {
  int statusCode = 0;
  std::string statusMessage;
  std::vector<HttpHeader> headers;
  std::string body;

  HttpResponse(std::string);
};

struct HttpRequest {
  std::string method = HttpMethods::Get;
  std::string resource = "/";
  std::string httpVersion = "HTTP/1.1";
  std::vector<HttpHeader> headers;
  std::string body;

  [[nodiscard]] std::string toString() const;
};

class HttpClient {
private:
  TcpStream stream;

public:
  HttpResponse get(const Url &url,
                   const std::vector<UrlParameter> &parameters = {});
};
