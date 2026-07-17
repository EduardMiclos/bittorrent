#include "Bencode.hpp"
#include <iostream>
#include <regex>
#include <stdexcept>
#include <variant>

std::string Bencode::getNextDataType(const std::string data) {

  // exmaple: i42e
  const std::regex integerRegex("^i.*");

  // example: 7:bencode
  const std::regex stringRegex("^[0-9]*:");

  // example: l7bencodei-20ee
  const std::regex ListRegex("^l.*");

  // example: d7:meaningi42e4:wiki7:bencodee
  const std::regex dictRegex("^d.*");

  const std::map<std::string, std::regex> regexPatterns = {
      {"integer", integerRegex},
      {"string", stringRegex},
      {"list", ListRegex},
      {"dict", dictRegex}};

  for (auto pattern : regexPatterns) {
    if (std::regex_search(data, pattern.second))
      return pattern.first;
  }

  return "";
}

std::string Bencode::getNextString(const std::string data,
                                   int *consumedTokens) {
  std::string strLengthIndicator = data.substr(0, data.find(':'));

  int substrLength = std::stoi(strLengthIndicator);

  std::string fullRightSubstring =
      data.substr(data.find(':') + 1, data.length() - 1);
  if (substrLength > fullRightSubstring.length()) {
    throw std::runtime_error("Incorrect string length!");
  }

  if (consumedTokens)
    *consumedTokens = strLengthIndicator.length() + 1 + substrLength;

  return data.substr(data.find(':') + 1, substrLength);
}

int Bencode::getNextInteger(const std::string data, int *consumedTokens) {
  int endPosition = data.find('e');
  if (consumedTokens)
    *consumedTokens = *consumedTokens + endPosition + 1;

  return std::strtoll(data.substr(1, endPosition - 1).c_str(), nullptr, 10);
}

std::string Bencode::removeNextDictKey(std::string data, int *consumedTokens) {
  int keyLength = data.find(':');
  int substrLength = std::stoi(data.substr(0, keyLength));

  if (consumedTokens)
    *consumedTokens = *consumedTokens + keyLength + 1 + substrLength;

  return data.substr(data.find(':') + substrLength + 1);
}

Bencode Bencode::decodeNext(std::string data, int *consumedTokens) {
  std::string dataType = getNextDataType(data);

  if (!consumedTokens)
    consumedTokens = new int(0);

  if (!dataType.length()) {
    throw std::runtime_error("Invalid bencode string. Decoding failed!");
  }

  if (dataType == "integer") {
    int value = getNextInteger(data, consumedTokens);
    return Bencode(value);
  }

  if (dataType == "string") {
    std::string value = getNextString(data, consumedTokens);
    return Bencode(value);
  }

  if (dataType == "dict") {
    Bencode::Dict dict;

    data = data.substr(1);
    *consumedTokens = *consumedTokens + 1;
    while (true) {
      int consumed = 0;

      std::string dictKey = getNextString(data);
      data = removeNextDictKey(data, consumedTokens);
      *consumedTokens = *consumedTokens + consumed;

      dict[dictKey] = decodeNext(data, &consumed);

      data = data.substr(consumed);
      *consumedTokens = *consumedTokens + consumed;

      if (data[0] == 'e') {
        *consumedTokens = *consumedTokens + 1;
        break;
      }
    }

    return Bencode(dict);
  }

  if (dataType == "list") {
    Bencode::List list;

    data = data.substr(1);
    *consumedTokens = *consumedTokens + 1;

    while (true) {
      int consumed = 0;
      list.push_back(decodeNext(data, &consumed));

      data = data.substr(consumed);
      *consumedTokens = *consumedTokens + consumed;

      if (data[0] == 'e') {
        *consumedTokens = *consumedTokens + 1;
        break;
      }
    }

    return Bencode(list);
  }

  return Bencode("");
}

const Bencode &Bencode::at(const std::string &key) const {
  if (auto dict = std::get_if<Dict>(&value_)) {
    auto it = dict->find(key);
    if (it == dict->end())
      throw std::out_of_range("Key not found: " + key);
    return it->second;
  }
  throw std::runtime_error("Bencode value is not a dictionary");
}

const Bencode &Bencode::at(const size_t index) const {
  if (auto list = std::get_if<List>(&value_)) {
    return list->at(index);
    ;
  }
  throw std::runtime_error("Bencode value is not a List");
}
