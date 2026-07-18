CXX ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -Wpedantic -Iinclude -g -O0
LDFLAGS  ?=
LDLIBS   ?= -ljsoncpp

BUILD    ?= build
TARGET   ?= btclient
SRC := $(wildcard src/*.cpp src/torrent/*.cpp src/networking/*.cpp)
OBJ := $(patsubst src/%.cpp,$(BUILD)/%.o,$(SRC))
LIB_SRC := $(filter-out src/main.cpp,$(SRC))
LIB_OBJ := $(patsubst src/%.cpp,$(BUILD)/%.o,$(LIB_SRC))

GTEST_DIR ?= googletest
GTEST_INC := $(GTEST_DIR)/googletest/include
GTEST_LIB := $(GTEST_DIR)/build/lib
GTEST_A   := $(GTEST_LIB)/libgtest.a
GTEST_MAIN_A := $(GTEST_LIB)/libgtest_main.a

TEST_CXXFLAGS := $(CXXFLAGS) -I$(GTEST_INC)

TEST_DIR   ?= tests
TEST_SRCS  := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJS  := $(patsubst $(TEST_DIR)/%.cpp,$(BUILD)/tests/%.o,$(TEST_SRCS))
TEST_BIN   := $(BUILD)/tests/test_all

.PHONY: all build run clean dirs test test-run test-clean

all: build

build: dirs $(BUILD)/$(TARGET)

dirs:
	mkdir -p $(BUILD)
	mkdir -p $(BUILD)/torrent
	mkdir -p $(BUILD)/networking
	mkdir -p $(BUILD)/root

$(BUILD)/$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(LDLIBS) -o $@

$(BUILD)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: dirs $(TEST_BIN)

$(TEST_BIN): $(TEST_OBJS) $(LIB_OBJ) $(GTEST_A) $(GTEST_MAIN_A)
	$(CXX) $(TEST_OBJS) $(LIB_OBJ) \
	    $(GTEST_MAIN_A) $(GTEST_A) \
	    -pthread $(LDFLAGS) $(LDLIBS) -o $(TEST_BIN)

$(BUILD)/tests/%.o: $(TEST_DIR)/%.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

test-run: test
	@$(TEST_BIN) --gtest_color=yes

test-clean:
	@rm -f $(TEST_BIN) $(TEST_OBJS)

run: build
	@$(BUILD)/$(TARGET) help

clean:
	@rm -rf $(BUILD)
