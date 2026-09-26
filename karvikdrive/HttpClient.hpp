#pragma once
#include <string>
#include <vector>

class HttpClient {
public:
    static std::vector<uint8_t> get(const std::wstring& host, const std::wstring& path);
    static std::string post(const std::wstring& host, const std::wstring& path, const std::string& body);
};
