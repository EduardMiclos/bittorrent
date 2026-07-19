#include "networking/HttpClient.hpp"
#include <cstddef>
#include <iostream>
#include <string>

[[nodiscard]] std::string HttpRequest::toString() const {
  std::string headerString;
  for (std::size_t i = 0; i < this->headers.size(); ++i) {
    headerString += this->headers[i].first;
    headerString += ": ";
    headerString += this->headers[i].second;
    headerString += "\r\n";
  }

  std::string requestString = this->method + " " + this->resource + " " +
                              this->httpVersion + "\r\n" + headerString +
                              "\r\n" + this->body;

  return requestString;
}

HttpResponse HttpClient::get(const Url &url,
                             const std::vector<UrlParameter> &parameters) {

  this->stream = TcpStream();
  this->stream.connect(url.host, url.port);

  std::string parameterString;
  for (std::size_t i = 0; i < parameters.size(); ++i) {
    parameterString += (i == 0 ? "?" : "&");
    parameterString += parameters[i].first;
    parameterString += "=";
    parameterString += parameters[i].second;
  }

  std::cout << "params string: " + parameterString << '\n';

  HttpRequest httpRequest{};
  httpRequest.httpVersion = "HTTP/1.1";
  httpRequest.method = HttpMethods::Get;
  httpRequest.resource = url.resource + parameterString;
  httpRequest.headers = {HttpHeader("Host", url.host),
                         HttpHeader("Connection", ConnectionType::Close)};

  this->stream.writeAll(httpRequest.toString());

  HttpResponse httpResponse{};
  std::string returnString = this->stream.readAll();

  std::cout << "RETURN STRING\n" << returnString << '\n';

  this->stream.close();
  return httpResponse;
}
