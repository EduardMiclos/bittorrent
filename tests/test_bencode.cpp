#include "../include/Bencode.hpp"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <variant>

#include <gtest/gtest.h>

TEST(BencodeTest, Parses_Dict_WithSimplePairs_ReturnsDict) {
  Bencode parser;
  const std::string input = "d6:lengthi123e4:name3:fooe";

  parser = parser.decodeNext(input);

  EXPECT_TRUE(std::holds_alternative<Bencode::Dict>(parser.value()));
}

TEST(BencodeTest, Parses_List_WithMixedTypes_ReturnsList) {
  Bencode parser;
  const std::string input = "l7:bencodei-20ee";

  parser = parser.decodeNext(input);

  EXPECT_TRUE(std::holds_alternative<Bencode::List>(parser.value()));
}

TEST(BencodeTest, Parses_Integer_ReturnsInt64) {
  Bencode parser;
  const std::string input = "i2354e";

  parser = parser.decodeNext(input);

  EXPECT_TRUE(std::holds_alternative<int64_t>(parser.value()));
}

TEST(BencodeTest, Parses_String_ReturnsByteString) {
  Bencode parser;
  const std::string input = "12:eduardmiclos";

  parser = parser.decodeNext(input);

  EXPECT_TRUE(std::holds_alternative<std::string>(parser.value()));
}

TEST(BencodeTest, Parses_String_WithExactLength_ConsumesExactBytes) {
  Bencode parser;
  const std::string input = "7:bencode";

  parser = parser.decodeNext(input);

  const std::string &value = std::get<std::string>(parser.value());

  EXPECT_EQ(value, "bencode");
}

TEST(BencodeTest, Errors_OnStringLengthGreaterThanAvailable) {
  Bencode parser;
  const std::string input = "12:bencode";

  EXPECT_THROW(parser.decodeNext(input), std::runtime_error);
}

TEST(BencodeTest, Parses_String_PartialLength_ReturnsRequestedBytes) {
  Bencode parser;
  const std::string input = "3:bencode";

  parser = parser.decodeNext(input);

  const std::string &value = std::get<std::string>(parser.value());

  EXPECT_EQ(value, "ben");
}

TEST(BencodeTest, Parses_Integer_PositiveValue) {
  Bencode parser;
  const std::string input = "i241235e";

  parser = parser.decodeNext(input);

  const int64_t value = std::get<int64_t>(parser.value());

  EXPECT_EQ(value, 241235);
}

TEST(BencodeTest, Parses_Integer_NegativeValue) {
  Bencode parser;
  const std::string input = "i-20e";

  parser = parser.decodeNext(input);

  const int64_t value = std::get<int64_t>(parser.value());

  EXPECT_EQ(value, -20);
}

TEST(BencodeTest, Parses_Dict_MultiplePairs_CorrectValues) {
  Bencode parser;
  const std::string input = "d7:meaningi42e4:wiki7:bencodee";

  parser = parser.decodeNext(input);

  const auto &dict = std::get<Bencode::Dict>(parser.value());

  ASSERT_EQ(dict.size(), 2);

  ASSERT_TRUE(dict.contains("meaning"));
  ASSERT_TRUE(dict.contains("wiki"));

  EXPECT_EQ(std::get<int64_t>(dict.at("meaning").value()), 42);
  EXPECT_EQ(std::get<std::string>(dict.at("wiki").value()), "bencode");
}

TEST(BencodeTest, Parses_List_WithMixedTypes_CorrectValues) {
  Bencode parser;
  const std::string input = "l7:bencodei-20ee";

  parser = parser.decodeNext(input);

  const auto &list = std::get<Bencode::List>(parser.value());

  ASSERT_EQ(list.size(), 2);

  EXPECT_EQ(std::get<std::string>(list.at(0).value()), "bencode");
  EXPECT_EQ(std::get<int64_t>(list.at(1).value()), -20);
}

TEST(BencodeTest, At_WithDictionaryKey_ReturnsCorrectValue) {
  Bencode parser;
  const std::string input = "d7:meaningi42e4:wiki7:bencodee";

  parser = parser.decodeNext(input);

  EXPECT_EQ(std::get<int64_t>(parser.at("meaning").value()), 42);
  EXPECT_EQ(std::get<std::string>(parser.at("wiki").value()), "bencode");
}

TEST(BencodeTest, At_WithListIndex_ReturnsCorrectValue) {
  Bencode parser;
  const std::string input = "l7:bencodei-20ee";

  parser = parser.decodeNext(input);

  EXPECT_EQ(std::get<std::string>(parser.at(0).value()), "bencode");
  EXPECT_EQ(std::get<int64_t>(parser.at(1).value()), -20);
}

TEST(TorrentBencode, SingleFile_Minimal) {
  const std::string input = "d"
                            "8:announce"
                            "30:http://t1.example.org/announce"
                            "4:info"
                            "d"
                            "6:length"
                            "i1234e"
                            "4:name"
                            "8:file.bin"
                            "12:piece length"
                            "i16384e"
                            "6:pieces"
                            "20:aaaaaaaaaaaaaaaaaaaa"
                            "e"
                            "e";

  Bencode torrent;
  torrent = torrent.decodeNext(input);

  ASSERT_TRUE(std::holds_alternative<Bencode::Dict>(torrent.value()));

  const auto &root = std::get<Bencode::Dict>(torrent.value());

  ASSERT_TRUE(root.contains("announce"));
  ASSERT_TRUE(root.contains("info"));

  EXPECT_EQ(std::get<std::string>(root.at("announce").value()),
            "http://t1.example.org/announce");

  const auto &info = std::get<Bencode::Dict>(root.at("info").value());

  ASSERT_EQ(info.size(), 4);

  EXPECT_EQ(std::get<int64_t>(info.at("length").value()), 1234);
  EXPECT_EQ(std::get<std::string>(info.at("name").value()), "file.bin");
  EXPECT_EQ(std::get<int64_t>(info.at("piece length").value()), 16384);
  EXPECT_EQ(std::get<std::string>(info.at("pieces").value()),
            "aaaaaaaaaaaaaaaaaaaa");
}

TEST(TorrentBencode, MultiFile_WithAnnounceList_Encoding_Private) {
  const std::string input = "d"
                            "8:announce"
                            "28:http://tracker.test/announce"
                            "13:announce-list"
                            "l"
                            "l"
                            "30:http://t1.example.org/announce"
                            "e"
                            "l"
                            "37:http://backup.example.net:80/announce"
                            "e"
                            "e"
                            "8:encoding"
                            "5:UTF-8"
                            "4:info"
                            "d"
                            "5:files"
                            "l"
                            "d"
                            "6:length"
                            "i100e"
                            "4:path"
                            "l"
                            "3:dir"
                            "5:a.txt"
                            "e"
                            "e"
                            "d"
                            "6:length"
                            "i200e"
                            "4:path"
                            "l"
                            "3:dir"
                            "5:b.txt"
                            "e"
                            "e"
                            "e"
                            "4:name"
                            "8:myfolder"
                            "12:piece length"
                            "i32768e"
                            "6:pieces"
                            "40:bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
                            "7:private"
                            "i1e"
                            "e"
                            "e";

  Bencode torrent;
  torrent = torrent.decodeNext(input);

  ASSERT_TRUE(std::holds_alternative<Bencode::Dict>(torrent.value()));

  const auto &root = std::get<Bencode::Dict>(torrent.value());

  ASSERT_TRUE(root.contains("announce"));
  ASSERT_TRUE(root.contains("announce-list"));
  ASSERT_TRUE(root.contains("encoding"));
  ASSERT_TRUE(root.contains("info"));

  EXPECT_EQ(std::get<std::string>(root.at("announce").value()),
            "http://tracker.test/announce");

  EXPECT_EQ(std::get<std::string>(root.at("encoding").value()), "UTF-8");

  const auto &announceList =
      std::get<Bencode::List>(root.at("announce-list").value());

  ASSERT_EQ(announceList.size(), 2);

  const auto &firstTier = std::get<Bencode::List>(announceList.at(0).value());

  const auto &secondTier = std::get<Bencode::List>(announceList.at(1).value());

  ASSERT_EQ(firstTier.size(), 1);
  ASSERT_EQ(secondTier.size(), 1);

  EXPECT_EQ(std::get<std::string>(firstTier.at(0).value()),
            "http://t1.example.org/announce");

  EXPECT_EQ(std::get<std::string>(secondTier.at(0).value()),
            "http://backup.example.net:80/announce");

  const auto &info = std::get<Bencode::Dict>(root.at("info").value());

  EXPECT_EQ(std::get<std::string>(info.at("name").value()), "myfolder");
  EXPECT_EQ(std::get<int64_t>(info.at("piece length").value()), 32768);
  EXPECT_EQ(std::get<std::string>(info.at("pieces").value()),
            "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
  EXPECT_EQ(std::get<int64_t>(info.at("private").value()), 1);

  const auto &files = std::get<Bencode::List>(info.at("files").value());

  ASSERT_EQ(files.size(), 2);

  const auto &firstFile = std::get<Bencode::Dict>(files.at(0).value());

  const auto &secondFile = std::get<Bencode::Dict>(files.at(1).value());

  EXPECT_EQ(std::get<int64_t>(firstFile.at("length").value()), 100);
  EXPECT_EQ(std::get<int64_t>(secondFile.at("length").value()), 200);

  const auto &firstPath = std::get<Bencode::List>(firstFile.at("path").value());

  const auto &secondPath =
      std::get<Bencode::List>(secondFile.at("path").value());

  ASSERT_EQ(firstPath.size(), 2);
  ASSERT_EQ(secondPath.size(), 2);

  EXPECT_EQ(std::get<std::string>(firstPath.at(0).value()), "dir");
  EXPECT_EQ(std::get<std::string>(firstPath.at(1).value()), "a.txt");

  EXPECT_EQ(std::get<std::string>(secondPath.at(0).value()), "dir");
  EXPECT_EQ(std::get<std::string>(secondPath.at(1).value()), "b.txt");
}
