#pragma once
#include "networking/Url.hpp"

class Tracker {
public:
  Url url;
  Tracker() = default;
  Tracker(std::string);
};
