#include "networking/Encode.hpp"
#include "networking/HttpClient.hpp"
#include <cstddef>
#include <sstream>
#include <stdexcept>
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

HttpResponse::HttpResponse(std::string response) {
  const std::size_t headersEnd = response.find("\r\n\r\n");

  if (headersEnd == std::string::npos) {
    throw std::runtime_error(
        "Invalid HTTP response: missing header terminator");
  }

  const std::string headerSection = response.substr(0, headersEnd);
  this->body = response.substr(headersEnd + 4);

  std::istringstream headerStream(headerSection);
  std::string statusLine;

  if (!std::getline(headerStream, statusLine))
    throw std::runtime_error("Invalid HTTP response: missing status line");

  if (!statusLine.empty() && statusLine.back() == '\r')
    statusLine.pop_back();

  std::istringstream statusStream(statusLine);
  std::string httpVersion;

  if (!(statusStream >> httpVersion >> this->statusCode))
    throw std::runtime_error("Invalid HTTP response status line");

  std::getline(statusStream, this->statusMessage);

  if (!this->statusMessage.empty() && this->statusMessage.front() == ' ')
    statusMessage.erase(0, 1);

  std::string line;

  while (std::getline(headerStream, line)) {
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }

    const std::size_t separator = line.find(':');

    if (separator == std::string::npos) {
      continue;
    }

    std::string name = line.substr(0, separator);
    std::string value = line.substr(separator + 1);

    if (!value.empty() && value.front() == ' ') {
      value.erase(0, 1);
    }

    this->headers.emplace_back(std::move(name), std::move(value));
  }
}

HttpResponse HttpClient::get(const Url &url,
                             const std::vector<UrlParameter> &parameters) {

  this->stream = TcpStream();
  this->stream.connect(url.host, url.port);

  // should look something like this (without the host part):
  // url.com?key1=value1&key2=value2&...
  std::string parameterString;
  for (std::size_t i = 0; i < parameters.size(); ++i) {

    parameterString += (i == 0 ? "?" : "&");
    parameterString += encodeURIComponent(parameters[i].first);
    parameterString += "=";
    parameterString += encodeURIComponent(parameters[i].second);
  }

  HttpRequest httpRequest{};
  httpRequest.httpVersion = "HTTP/1.1";
  httpRequest.method = HttpMethods::Get;
  httpRequest.resource = url.resource + parameterString;
  httpRequest.headers = {HttpHeader("Host", url.host),
                         HttpHeader("Connection", ConnectionType::Close)};

  this->stream.writeAll(httpRequest.toString());

  std::string returnString = this->stream.readAll();
  HttpResponse httpResponse(returnString);

  this->stream.close();
  return httpResponse;
}
