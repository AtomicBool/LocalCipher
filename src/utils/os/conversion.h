#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

namespace Conversion
{
    inline std::vector<uint8_t> StringToBytes(const std::string& hex)
    {
        if (hex.size() % 2 != 0)
            throw std::runtime_error("Invalid hex length");

        std::vector<uint8_t> bytes;
        bytes.reserve(hex.size() / 2);

        for (size_t i = 0; i < hex.size(); i += 2)
        {
            std::string byteStr = hex.substr(i, 2);
            uint8_t byte = static_cast<uint8_t>(
                std::stoi(byteStr, nullptr, 16)
                );

            bytes.push_back(byte);
        }

        return bytes;
    }

    inline std::string BytesToString(const std::vector<uint8_t>& data)
    {
        std::ostringstream oss;

        for (uint8_t byte : data)
        {
            oss << std::hex
                << std::setw(2)
                << std::setfill('0')
                << static_cast<int>(byte);
        }

        return oss.str();
    }

    inline std::string GBKToUTF8(const std::string& gbk)
    {
        int len = MultiByteToWideChar(936, 0, gbk.c_str(), -1, NULL, 0);
        std::wstring wstr(len, 0);
        MultiByteToWideChar(936, 0, gbk.c_str(), -1, wstr.data(), len);

        len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
        std::string utf8(len, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, utf8.data(), len, NULL, NULL);

        utf8.pop_back(); // 去掉 '\0'
        return utf8;
    }
}