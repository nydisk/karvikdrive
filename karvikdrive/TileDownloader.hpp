#pragma once
#include <vector>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include "Configuration.hpp"

using Cfg = Configuration;
namespace fs = std::filesystem;

class TileDownloader {
	inline static const std::string kCacheDirectory = "cache/";
	inline static uint64_t tilesCached = 0;

	static std::string getTileCacheName(int x, int y);
	static bool isCached(int x, int y);
	static std::vector<uint8_t> retrieveFromCache(int x, int y);
	static void cache(int x, int y, const std::vector<uint8_t>& data);
	static uint64_t cacheSizeMB();
public:
	static void cleanUpCache();
	static std::vector<uint8_t> fetchTile(int x, int y, int z);
};