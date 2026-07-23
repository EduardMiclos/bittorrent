#pragma once

#include <openssl/sha.h>
#include <string>

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
