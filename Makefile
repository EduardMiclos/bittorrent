CXX ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -Wpedantic -Iinclude -g -O0
LDFLAGS  ?=
LDLIBS   ?= -ljsoncpp

BUILD    ?= build
TARGET   ?= bittorrent
SRC := $(wildcard src/*.cpp src/torrent/*.cpp src/networking/*.cpp)
OBJ := $(patsubst src/%.cpp,$(BUILD)/%.o,$(SRC))

.PHONY: all build run clean dirs

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

run: build
	@$(BUILD)/$(TARGET) help

clean:
	@rm -rf $(BUILD)
