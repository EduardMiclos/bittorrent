#include "networking/Url.hpp"
#include "torrent/Tracker.hpp"

Tracker::Tracker(std::string trackerUrl) { this->url = Url(trackerUrl); }
