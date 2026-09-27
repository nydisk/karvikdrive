#include <iostream>
#include <string>
#include <windows.h>
#include <winhttp.h>
#include "TileDownloader.hpp"
#include "HttpClient.hpp"

#pragma comment(lib, "winhttp.lib")

std::string TileDownloader::getTileCacheName(int x, int y) {
    return std::to_string(x) + "_" + std::to_string(y) + ".jpg";
}

bool TileDownloader::isCached(int x, int y) {
    return fs::exists(kCacheDirectory + getTileCacheName(x, y));
}

std::vector<uint8_t> TileDownloader::retrieveFromCache(int x, int y) {
    try {
        fs::last_write_time(kCacheDirectory + getTileCacheName(x, y), std::chrono::file_clock::now()); // update for LRU
    }
    catch (std::exception& e) {
        std::cout << "uh oh\n * " << e.what() << '\n';
    }
    std::ifstream f(kCacheDirectory + getTileCacheName(x, y), std::ios::binary);
    return { std::istreambuf_iterator<char>(f),{} };
}

void TileDownloader::cache(int x, int y, const std::vector<uint8_t>& data) {
    //std::cout << "caching new tile x=" << x << "; y=" << y << '\n';
    fs::create_directories(kCacheDirectory);
    std::ofstream f(kCacheDirectory + getTileCacheName(x, y), std::ios::binary);
    f.write((char*)data.data(), data.size());
    tilesCached++;
    if (tilesCached % 1000 == 0) {
        cleanUpCache();
    }
}

uint64_t TileDownloader::cacheSizeMB() {
    if (!fs::exists(kCacheDirectory)) return -1;
    uint64_t sz = 0;
    for (const auto& file : fs::directory_iterator(kCacheDirectory))
        sz += (uint64_t)file.file_size();
    return sz / 1024 / 1024;
}

void TileDownloader::cleanUpCache() {
    uint64_t cacheSize = cacheSizeMB();
    if (cacheSize < Cfg::cacheCfg.maximumCacheSizeMB) return;
    // uh oh!! :(
    uint64_t toEvaporate = cacheSize - (uint64_t)(Cfg::cacheCfg.maximumCacheSizeMB * Cfg::cacheCfg.cacheKeepPercentage);

    std::cout << "cache cleanup:\n";
    std::cout << "  size     = " << cacheSize << '\n';
    std::cout << "  maxSize  = " << Cfg::cacheCfg.maximumCacheSizeMB << '\n';
    std::cout << "  toRemove = " << toEvaporate << '\n';

    using FileEntry = std::pair<fs::path, fs::file_time_type>;
    std::vector<FileEntry> files{};
    for (const auto& entry : fs::directory_iterator(kCacheDirectory))
        files.push_back({ entry.path(), entry.last_write_time() });

    std::sort(files.begin(), files.end(), [](const FileEntry& a, const FileEntry& b) { return a.second < b.second; });

    uint64_t evaporated = 0;
    for (const auto& [path, _] : files) {
        if (evaporated >= toEvaporate * 1024 * 1024) break;
        uint64_t fileSize = fs::file_size(path);
        fs::remove(path);
        evaporated += fileSize;
    }

    std::cout << "cache cleaned\n";
}

std::vector<uint8_t> TileDownloader::fetchTile(int x, int y, int z) {
    if (isCached(x, y)) return retrieveFromCache(x, y);

    std::wstring path = L"/ArcGIS/rest/services/World_Imagery/MapServer/tile/"
        + std::to_wstring(z) + L"/"
        + std::to_wstring(y) + L"/"
        + std::to_wstring(x);

    auto data = HttpClient::get(L"server.arcgisonline.com", path);
    if (!data.empty()) cache(x, y, data);
    return data;
}
