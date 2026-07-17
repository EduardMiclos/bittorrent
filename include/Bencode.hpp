#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>

/** Bencode class for decoding Bencode data.
 * This class provides functionality to decode Bencode formatted strings.
 * The values will be stored as a variant type that can hold integers, strings,
 * lists, or dictionaries.
 */
class Bencode {
public:
  using List = std::vector<Bencode>;
  using Dict = std::map<std::string, Bencode>;
  using Value = std::variant<int64_t, std::string, List, Dict>;

  Bencode() = default;
  explicit Bencode(Value v) : value_(std::move(v)) {}

  static std::string getNextDataType(const std::string data);
  static int getNextInteger(const std::string data, int *consumedTokens = nullptr);
  static std::string getNextString(const std::string data, int *consumedTokens = nullptr);
  static std::string removeNextDictKey(std::string data, int *consumedTokens = nullptr);

  Bencode decodeNext(std::string data, int *consumedTokens = nullptr);

  const Value& value() const noexcept { return value_; }
  Value& value() noexcept { return value_; }         

  const Bencode& at(const std::string& key) const;
  const Bencode& at(const size_t index) const;

private:
  Value value_;
};