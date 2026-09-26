#include "HttpClient.hpp"
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

std::vector<uint8_t> HttpClient::get(const std::wstring& host, const std::wstring& path) {
    HINTERNET session = WinHttpOpen(L"KarvikDrive/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, nullptr, nullptr, 0);
    HINTERNET connect = WinHttpConnect(session, host.c_str(),
        INTERNET_DEFAULT_HTTPS_PORT, 0);
    HINTERNET request = WinHttpOpenRequest(connect, L"GET", path.c_str(),
        nullptr, nullptr, nullptr, WINHTTP_FLAG_SECURE);

    WinHttpSendRequest(request, nullptr, 0, nullptr, 0, 0, 0);
    WinHttpReceiveResponse(request, nullptr);

    DWORD statusCode = 0;
    DWORD statusCodeSize = sizeof(statusCode);
    WinHttpQueryHeaders(request,
        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        nullptr, &statusCode, &statusCodeSize, nullptr);

    std::vector<uint8_t> data;
    if (statusCode == 200) {
        DWORD bytesRead = 0;
        uint8_t buf[4096]{};
        while (WinHttpReadData(request, buf, sizeof(buf), &bytesRead) && bytesRead > 0)
            data.insert(data.end(), buf, buf + bytesRead);
    }

    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connect);
    WinHttpCloseHandle(session);
    return data;
}

std::string HttpClient::post(const std::wstring& host, const std::wstring& path, const std::string& body) {
    HINTERNET session = WinHttpOpen(L"KarvikDrive/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, nullptr, nullptr, 0);
    HINTERNET connect = WinHttpConnect(session, host.c_str(),
        INTERNET_DEFAULT_HTTPS_PORT, 0);
    HINTERNET request = WinHttpOpenRequest(connect, L"POST", path.c_str(),
        nullptr, nullptr, nullptr, WINHTTP_FLAG_SECURE);

    std::wstring headers = L"Content-Type: application/x-www-form-urlencoded";
    WinHttpSendRequest(request,
        headers.c_str(), (DWORD)headers.size(),
        (void*)body.c_str(), (DWORD)body.size(),
        (DWORD)body.size(), 0);
    WinHttpReceiveResponse(request, nullptr);

    std::string response;
    DWORD bytesRead = 0;
    char buf[4096]{};
    while (WinHttpReadData(request, buf, sizeof(buf), &bytesRead) && bytesRead > 0)
        response.append(buf, bytesRead);

    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connect);
    WinHttpCloseHandle(session);
    return response;
}