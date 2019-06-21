#pragma once

class FileSystem final
{
public:
    static auto ToUpper(const std::string& lower) -> const std::string;
    static auto ToLower(const std::string& upper) -> const std::string;
    static auto ToString(const std::wstring& str) -> const std::string;
    static auto ToWstring(const std::string& str) -> const std::wstring;

private:

};