#pragma once

#include <algorithm>
#include <openssl/sha.h>
#include <stdexcept>
#include <string>
#include <vector>

inline std::string generateRandomString(short stringSize) {
  static const char alphanum[] = "0123456789"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz";
  std::string tmp_s;
  tmp_s.reserve(stringSize);

  for (int i = 0; i < stringSize; ++i) {
    tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
  }

  return tmp_s;
}

inline std::string sha1(std::string_view data) {
  unsigned char hash[SHA_DIGEST_LENGTH];

  SHA1(reinterpret_cast<const unsigned char *>(data.data()), data.size(), hash);
  return std::string{reinterpret_cast<const char *>(hash), SHA_DIGEST_LENGTH};
}

inline std::vector<std::byte> stringToBytes(std::string const &s) {
  std::vector<std::byte> bytes;
  bytes.reserve(s.size());

  std::transform(std::begin(s), std::end(s), std::back_inserter(bytes), [](char c) { return std::byte(c); });
  return bytes;
}

inline uint32_t bytesToUint32(std::vector<std::byte> bytes) {
  if (bytes.size() != 4)
    throw std::invalid_argument("Invalid number of bytes for conversion.");

  uint32_t value = (std::to_integer<uint32_t>(bytes[0] << 24)) | std::to_integer<uint32_t>(bytes[1] << 16) |
                   std::to_integer<uint32_t>(bytes[2] << 8) | std::to_integer<uint32_t>(bytes[3]);
  return value;
}

inline std::vector<std::byte> uint32ToBytes(uint32_t value) {
  std::vector<std::byte> bytes;

  bytes.push_back(static_cast<std::byte>(value >> 24));
  bytes.push_back(static_cast<std::byte>(value >> 16));
  bytes.push_back(static_cast<std::byte>(value >> 8));
  bytes.push_back(static_cast<std::byte>(value));

  return bytes;
}
