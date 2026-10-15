#pragma once

#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

inline std::string encodeURIComponent(std::string_view input) {
  std::ostringstream encoded;

  encoded << std::uppercase << std::hex;

  for (const unsigned char byte : input) {
    const bool isUnreserved = std::isalnum(byte) || byte == '-' ||
                              byte == '.' || byte == '_' || byte == '~';

    if (isUnreserved) {
      encoded << static_cast<char>(byte);
    } else {
      encoded << '%' << std::setw(2) << std::setfill('0')
              << static_cast<unsigned int>(byte);
    }
  }

  return encoded.str();
}

inline int hexadecimalValue(char character) {
  if (character >= '0' && character <= '9') {
    return character - '0';
  }

  if (character >= 'A' && character <= 'F') {
    return character - 'A' + 10;
  }

  if (character >= 'a' && character <= 'f') {
    return character - 'a' + 10;
  }

  return -1;
}

inline std::string decodeURIComponent(std::string_view input) {
  std::string decoded;
  decoded.reserve(input.size());

  for (std::size_t i = 0; i < input.size(); ++i) {
    if (input[i] != '%') {
      decoded.push_back(input[i]);
      continue;
    }

    if (i + 2 >= input.size()) {
      throw std::invalid_argument("Incomplete percent-encoded sequence");
    }

    const int high = hexadecimalValue(input[i + 1]);
    const int low = hexadecimalValue(input[i + 2]);

    if (high < 0 || low < 0) {
      throw std::invalid_argument("Invalid percent-encoded sequence");
    }

    const unsigned char byte = static_cast<unsigned char>((high << 4) | low);

    decoded.push_back(static_cast<char>(byte));
    i += 2;
  }

  return decoded;
}
